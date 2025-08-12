#define W25Q64_C

#include "w25q64.h"
#include "sc32_conf.h"
#include "os.h"
#include "log.h"
#include <string.h>

static SemaphoreHandle_t dma_done = NULL;

BaseType_t w25q64_init(void) {
    if (dma_done == NULL) {
        dma_done = xSemaphoreCreateBinary();
        if (dma_done == NULL) {
            OS_PRTF(ERRO_LOG, "create dma_done fail!\n");
            return pdFAIL;
        }
    }

    GPIO_WriteBit(CHIP_GPIO_GRP, CHIP_GPIO_PIN, 1);

    uint8_t    data[8] = {0};
    w25q64_arg arg;

    w25q64_ctl(Reset, NULL);
    vTaskDelay(1);

    arg.data = data;
    arg.size = sizeof(data);
    w25q64_ctl(ReadUniqueID, &arg);
    if (*(uint64_t *)data == W25Q64_Unique_ID) {
        OS_PRTF(INFO_LOG, "id match: 0x%llX", *(uint64_t *)data);
    } else {
        OS_PRTF(INFO_LOG, "id unmatch: 0x%llX", *(uint64_t *)data);
    }

    w25q64_ctl(WriteDisable, NULL);
    OS_PRTF(INFO_LOG, "Write Disable!");

    arg.size = 1;
    w25q64_ctl(ReadSR1, &arg);
    OS_PRTF(INFO_LOG, "SR1: 0x%X!", data[0]);
    w25q64_ctl(ReadSR2, &arg);
    OS_PRTF(INFO_LOG, "SR2: 0x%X!", data[0]);
    w25q64_ctl(ReadSR3, &arg);
    OS_PRTF(INFO_LOG, "SR3: 0x%X!", data[0]);

    /********** 进行整片擦除 **********/

#if CHIP_ERASE
    w25q64_ctl(WriteEnable, NULL);

    uint8_t    SR1;
    w25q64_arg erase_arg = {
        .data = &SR1,
        .size = 1,
    };

    w25q64_ctl(EraseChip, NULL);
    do {
        vTaskDelay(10);
        w25q64_ctl(ReadSR1, &erase_arg);
    } while (SR1 & 0x1);
#endif  // CHIP_ERASE

    OS_PRTF(NEWS_LOG, "Initialize Finish!");

    return pdPASS;
}

__inline__ void w25q64_dma_irq(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(dma_done, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void _w25q64_send_bytes(const uint8_t * const data, const uint32_t size) {
    QSPI_Write_ComSet(USE_QSPI, QSPI_LMode_1Line, QSPI_DWidth_8bit, QSPI_CLKONLY_OFF);
    for (uint32_t index = size; index > 0; --index) {
        QSPI_SendData8(USE_QSPI, data[index - 1]);
        while (QSPI_GetFlagStatus(USE_QSPI, QSPI_Flag_BUSY)) {}
    }
}

__attribute__((optnone)) void w25q64_ctl(const w25q64_cmd         cmd,
                                         const w25q64_arg * const arg) {
    GPIO_WriteBit(CHIP_GPIO_GRP, CHIP_GPIO_PIN, 0);

    /********** 发送指令 **********/

    switch (cmd) {
        case Reset: {
            QSPI_Write_ComSet(USE_QSPI, QSPI_LMode_1Line, QSPI_DWidth_16bit,
                              QSPI_CLKONLY_OFF);
            QSPI_SendData16(USE_QSPI, Reset);
        } break;
        default: {
            QSPI_Write_ComSet(USE_QSPI, QSPI_LMode_1Line, QSPI_DWidth_8bit,
                              QSPI_CLKONLY_OFF);
            QSPI_SendData8(USE_QSPI, cmd);
        } break;
    }
    while (QSPI_GetFlagStatus(USE_QSPI, QSPI_Flag_BUSY)) {}

    /********** 发送附带参数 **********/

    if (arg) {
        switch (cmd) {
            case ReadUniqueID: {
                /********** 发送4个空字节 **********/

                uint8_t dummy[4] = {0};
                _w25q64_send_bytes(dummy, sizeof(dummy));

                /********** 接收8个字节 **********/

                QSPI_Read_ComSet(USE_QSPI, QSPI_LMode_1Line, QSPI_DWidth_8bit, 8);
                QSPI_ReceiveMultipleData(USE_QSPI, (uint32_t *)arg->data, arg->size);
            } break;
            case ReadSR1:
            case ReadSR2:
            case ReadSR3: {
                /********** 发送1个字节时钟 **********/

                QSPI_Read_ComSet(USE_QSPI, QSPI_LMode_1Line, QSPI_DWidth_8bit, 1);
                QSPI_CLKONLYSet(USE_QSPI, QSPI_CLKONLY_ON);
                while (QSPI_GetFlagStatus(USE_QSPI, QSPI_Flag_BUSY)) {}

                /********** 接收1个字节 **********/

                arg->data[0] = QSPI_ReceiveData8(USE_QSPI);
            } break;
            case ReadDataSPI:
            case ReadDataQSPI: {
                /********** 从高到低发送3个字节的地址 **********/

                _w25q64_send_bytes(arg->adr, 3);

                /********** 发送1个空字节 **********/

                if (cmd == ReadDataQSPI) {
                    QSPI_Write_ComSet(USE_QSPI, QSPI_LMode_1Line, QSPI_DWidth_8bit,
                                      QSPI_CLKONLY_OFF);
                    QSPI_SendData8(USE_QSPI, 0);
                    while (QSPI_GetFlagStatus(USE_QSPI, QSPI_Flag_BUSY)) {}
                }

                QSPI_Read_ComSet(
                    USE_QSPI, (cmd == ReadDataQSPI) ? QSPI_LMode_4Line : QSPI_LMode_1Line,
                    QSPI_DWidth_8bit, arg->size);

                if (cmd == ReadDataQSPI) {
                    QSPI_ReceiveMultipleData(QSPI0, (uint32_t *)arg->data, arg->size);
                } else {
                    /********** 用DMA连续接收字节流 **********/

                    if (xSemaphoreTake(dma_mutex, portMAX_DELAY) == pdTRUE) {
                        DMA_SetDstAddress(USE_DMA_RX, (uint32_t)arg->data);
                        DMA_SetCurrDataCounter(USE_DMA_RX, arg->size);
                        DMA_Cmd(USE_DMA_RX, ENABLE);
                        QSPI_DMACmd(USE_QSPI, QSPI_DMAReq_RX, ENABLE);
                        QSPI_CLKONLYSet(USE_QSPI, QSPI_CLKONLY_ON);

                        if (xSemaphoreTake(dma_done, portMAX_DELAY) == pdTRUE) {
                            QSPI_DMACmd(USE_QSPI, QSPI_DMAReq_RX, DISABLE);
                            DMA_Cmd(USE_DMA_RX, DISABLE);
                        }

                        xSemaphoreGive(dma_mutex);
                    }
                }

            } break;
            case Erase4KB:
            case Erase32KB:
            case Erase64KB: {
                /********** 从高到低发送3个字节的地址 **********/

                _w25q64_send_bytes(arg->adr, 3);
            } break;
            case ProgramSPI:
            case ProgramQSPI: {
                /********** 从高到低发送3个字节的地址 **********/

                _w25q64_send_bytes(arg->adr, 3);

                /********** 用DMA连续接收字节流 **********/

                QSPI_Write_ComSet(
                    USE_QSPI, (cmd == ProgramQSPI) ? QSPI_LMode_4Line : QSPI_LMode_1Line,
                    QSPI_DWidth_8bit, QSPI_CLKONLY_OFF);

                if (xSemaphoreTake(dma_mutex, portMAX_DELAY) == pdTRUE) {
                    DMA_SetSrcAddress(USE_DMA_TX, (uint32_t)arg->data);
                    DMA_SetCurrDataCounter(USE_DMA_TX, arg->size);
                    DMA_Cmd(USE_DMA_TX, ENABLE);
                    QSPI_DMACmd(USE_QSPI, QSPI_DMAReq_TX, ENABLE);
                    DMA_SoftwareTrigger(USE_DMA_TX);

                    if (xSemaphoreTake(dma_done, portMAX_DELAY) == pdTRUE) {
                        QSPI_DMACmd(USE_QSPI, QSPI_DMAReq_TX, DISABLE);
                        DMA_Cmd(USE_DMA_TX, DISABLE);
                    }

                    xSemaphoreGive(dma_mutex);
                }
            } break;
            default: break;
        }
    }

    GPIO_WriteBit(CHIP_GPIO_GRP, CHIP_GPIO_PIN, 1);
}
