#include "w25q_cfg.h"
#include "w25q_if.h"
#include INC_LOG
#include INC_RTOS
#include <string.h>

__attribute__((noinline)) static void gpio_init(void) {
    GPIO_InitTypeDef gpio_struct = {.GPIO_DriveLevel = 0, .GPIO_Mode = GPIO_Mode_OUT_PP};
    gpio_struct.GPIO_Pin         = CHIP_GPIO_PIN;
    GPIO_Init(CHIP_GPIO_GRP, &gpio_struct);
}

__attribute__((noinline)) static void _w25q_send_bytes(const uint8_t * const data,
                                                       const uint32_t        size) {
    QSPI_Write_ComSet(QSPI_DEV, QSPI_LMode_1Line, QSPI_DWidth_8bit, QSPI_CLKONLY_OFF);
    for (uint32_t index = size; index > 0; --index) {
        QSPI_SendData8(QSPI_DEV, data[index - 1]);
        while (QSPI_GetFlagStatus(QSPI_DEV, QSPI_Flag_BUSY)) {}
    }
}

__attribute__((optnone)) void w25q_ctl(const w25q_cmd_t * const cmd) {
    GPIO_WriteBit(CHIP_GPIO_GRP, CHIP_GPIO_PIN, 0);

    /********** 发送指令 **********/

    switch (cmd->ins) {
        case Reset:
            QSPI_Write_ComSet(QSPI_DEV, QSPI_LMode_1Line, QSPI_DWidth_16bit,
                              QSPI_CLKONLY_OFF);
            QSPI_SendData16(QSPI_DEV, Reset);
            break;
        default:
            QSPI_Write_ComSet(QSPI_DEV, QSPI_LMode_1Line, QSPI_DWidth_8bit,
                              QSPI_CLKONLY_OFF);
            QSPI_SendData8(QSPI_DEV, cmd->ins);
            break;
    }
    while (QSPI_GetFlagStatus(QSPI_DEV, QSPI_Flag_BUSY)) {}

    /********** 发送参数 **********/

    if ((cmd->data != RT_NULL) && (cmd->size > 0)) {
        switch (cmd->ins) {
            case ReadUniqueID:
                /********** 发送4个空字节 **********/

                QSPI_Write_ComSet(QSPI_DEV, QSPI_LMode_1Line, QSPI_DWidth_8bit,
                                  QSPI_CLKONLY_OFF);
                for (uint8_t i = 0; i < 4; i++) {
                    QSPI_SendData8(QSPI_DEV, 0);
                    while (QSPI_GetFlagStatus(QSPI_DEV, QSPI_Flag_BUSY)) {}
                }

                /********** 接收8个字节 **********/

                QSPI_Read_ComSet(QSPI_DEV, QSPI_LMode_1Line, QSPI_DWidth_8bit, cmd->size);
                QSPI_ReceiveMultipleData(QSPI_DEV, (uint32_t *)cmd->data, cmd->size);
                break;
            case ReadSR1:
            case ReadSR2:
            case ReadSR3:
                /********** 接收1个字节时钟 **********/

                QSPI_Read_ComSet(QSPI_DEV, QSPI_LMode_1Line, QSPI_DWidth_8bit, 1);
                QSPI_CLKONLYSet(QSPI_DEV, QSPI_CLKONLY_ON);
                while (QSPI_GetFlagStatus(QSPI_DEV, QSPI_Flag_BUSY)) {}
                cmd->data[0] = QSPI_ReceiveData8(QSPI_DEV);
                break;
            // case ReadDataSPI:
            // case ReadDataQSPI: {
            //     /********** 从高到低发送3个字节的地址 **********/

            //     _w25q_send_bytes(arg->adr, 3);

            //     /********** 发送1个空字节 **********/

            //     if (cmd == ReadDataQSPI) {
            //         QSPI_Write_ComSet(QSPI_DEV, QSPI_LMode_1Line, QSPI_DWidth_8bit,
            //                           QSPI_CLKONLY_OFF);
            //         QSPI_SendData8(QSPI_DEV, 0);
            //         while (QSPI_GetFlagStatus(QSPI_DEV, QSPI_Flag_BUSY)) {}
            //     }

            //     QSPI_Read_ComSet(
            //         QSPI_DEV, (cmd == ReadDataQSPI) ? QSPI_LMode_4Line :
            //         QSPI_LMode_1Line, QSPI_DWidth_8bit, arg->size);

            //     if (cmd == ReadDataQSPI) {
            //         QSPI_ReceiveMultipleData(QSPI0, (uint32_t *)arg->data,
            //         arg->size);
            //     } else {
            //         /********** 用DMA连续接收字节流 **********/

            //         if (xSemaphoreTake(dma_mutex, portMAX_DELAY) == pdTRUE) {
            //             DMA_SetDstAddress(USE_DMA_RX, (uint32_t)arg->data);
            //             DMA_SetCurrDataCounter(USE_DMA_RX, arg->size);
            //             DMA_Cmd(USE_DMA_RX, ENABLE);
            //             QSPI_DMACmd(QSPI_DEV, QSPI_DMAReq_RX, ENABLE);
            //             QSPI_CLKONLYSet(QSPI_DEV, QSPI_CLKONLY_ON);

            //             if (xSemaphoreTake(dma_done, portMAX_DELAY) == pdTRUE) {
            //                 QSPI_DMACmd(QSPI_DEV, QSPI_DMAReq_RX, DISABLE);
            //                 DMA_Cmd(USE_DMA_RX, DISABLE);
            //             }

            //             xSemaphoreGive(dma_mutex);
            //         }
            //     }

            // } break;
            // case Erase4KB:
            // case Erase32KB:
            // case Erase64KB: {
            //     /********** 从高到低发送3个字节的地址 **********/

            //     _w25q_send_bytes(arg->adr, 3);
            // } break;
            // case ProgramSPI:
            // case ProgramQSPI: {
            //     /********** 从高到低发送3个字节的地址 **********/

            //     _w25q_send_bytes(arg->adr, 3);

            //     /********** 用DMA连续接收字节流 **********/

            //     QSPI_Write_ComSet(
            //         QSPI_DEV, (cmd == ProgramQSPI) ? QSPI_LMode_4Line :
            //         QSPI_LMode_1Line, QSPI_DWidth_8bit, QSPI_CLKONLY_OFF);

            //     if (xSemaphoreTake(dma_mutex, portMAX_DELAY) == pdTRUE) {
            //         DMA_SetSrcAddress(USE_DMA_TX, (uint32_t)arg->data);
            //         DMA_SetCurrDataCounter(USE_DMA_TX, arg->size);
            //         DMA_Cmd(USE_DMA_TX, ENABLE);
            //         QSPI_DMACmd(QSPI_DEV, QSPI_DMAReq_TX, ENABLE);
            //         DMA_SoftwareTrigger(USE_DMA_TX);

            //         if (xSemaphoreTake(dma_done, portMAX_DELAY) == pdTRUE) {
            //             QSPI_DMACmd(QSPI_DEV, QSPI_DMAReq_TX, DISABLE);
            //             DMA_Cmd(USE_DMA_TX, DISABLE);
            //         }

            //         xSemaphoreGive(dma_mutex);
            //     }
            // } break;
            default: break;
        }
    }

    GPIO_WriteBit(CHIP_GPIO_GRP, CHIP_GPIO_PIN, 1);
}

uint8_t w25q_init(void) {
    gpio_init();

    GPIO_WriteBit(CHIP_GPIO_GRP, CHIP_GPIO_PIN, 1);

    uint8_t    data[8] = {0};
    w25q_cmd_t cmd     = {.data = data};

    cmd.ins  = Reset;
    cmd.size = 0;
    w25q_ctl(&cmd);
    RTOS_DELAY_IF(1);

    cmd.ins  = ReadUniqueID;
    cmd.size = sizeof(data);
    w25q_ctl(&cmd);

    const uint64_t id = *(uint64_t *)data;
    PRTF_OS_LOG(INFO_LOG, "id: 0x");
    for (uint8_t i = 0; i < sizeof(data); i++) {
        PRTF_LOG(INFO_LOG, "%x", data[i]);
    }
    PRTF_LOG(INFO_LOG, "\n");

    cmd.ins  = WriteDisable;
    cmd.size = 0;
    w25q_ctl(&cmd);
    PRTF_OS_LOG(INFO_LOG, "Write Disable!\n");

    cmd.ins  = ReadSR1;
    cmd.size = 1;
    w25q_ctl(&cmd);
    PRTF_OS_LOG(INFO_LOG, "SR1: 0x%X!\n", data[0]);

    cmd.ins = ReadSR2;
    w25q_ctl(&cmd);
    PRTF_OS_LOG(INFO_LOG, "SR2: 0x%X!\n", data[0]);

    cmd.ins = ReadSR3;
    w25q_ctl(&cmd);
    PRTF_OS_LOG(INFO_LOG, "SR3: 0x%X!\n", data[0]);

    /********** 进行整片擦除 **********/

#if CHIP_ERASE
    w25q_ctl(WriteEnable, NULL);

    uint8_t  SR1;
    w25q_arg erase_arg = {
        .data = &SR1,
        .size = 1,
    };

    w25q_ctl(EraseChip, NULL);
    do {
        RTOS_DELAY_IF(10);
        w25q_ctl(ReadSR1, &erase_arg);
    } while (SR1 & 0x1);
#endif  // CHIP_ERASE

    PRTF_OS_LOG(NEWS_LOG, "Initialize Finish!\n");

    return 0;
}

__inline__ void w25q_dma_irq(void) {
    // BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // xSemaphoreGiveFromISR(dma_done, &xHigherPriorityTaskWoken);
    // portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}