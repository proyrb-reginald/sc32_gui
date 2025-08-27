#include "ep15301t_cfg.h"
#include "ep15301t_if.h"
#include <sc32_conf.h>
#include INC_LOG
#include INC_RTOS

__attribute__((noinline)) static void gpio_init(void) {
    GPIO_InitTypeDef gpio_struct = {.GPIO_DriveLevel = 0, .GPIO_Mode = GPIO_Mode_OUT_PP};

    gpio_struct.GPIO_Pin = RESET_GPIO_PIN;
    GPIO_Init(RESET_GPIO_GRP, &gpio_struct);

    gpio_struct.GPIO_Pin = CHIP_GPIO_PIN;
    GPIO_Init(CHIP_GPIO_GRP, &gpio_struct);

    gpio_struct.GPIO_Pin = BACK_GPIO_PIN;
    GPIO_Init(BACK_GPIO_GRP, &gpio_struct);
}

__attribute__((optnone)) void ep15301t_ctl(const ep15301t_cmd_t * const cmd) {
    GPIO_WriteBit(CHIP_GPIO_GRP, CHIP_GPIO_PIN, 0);

    QSPI_Write_ComSet(QSPI_DEV, QSPI_LMode_1Line, QSPI_DWidth_8bit, QSPI_CLKONLY_OFF);
    QSPI_SendData8(QSPI_DEV, 0x38);

    /********** 发送指令 **********/

    QSPI_Write_ComSet(QSPI_DEV, QSPI_LMode_4Line, QSPI_DWidth_8bit, QSPI_CLKONLY_OFF);

    QSPI_SendData8(QSPI_DEV, 0x00);
    while (QSPI_GetFlagStatus(QSPI_DEV, QSPI_Flag_BUSY)) {}

    QSPI_SendData8(QSPI_DEV, cmd->ins);
    while (QSPI_GetFlagStatus(QSPI_DEV, QSPI_Flag_BUSY)) {}

    QSPI_SendData8(QSPI_DEV, 0x00);
    while (QSPI_GetFlagStatus(QSPI_DEV, QSPI_Flag_BUSY)) {}

    /********** 发送参数 **********/

    if ((cmd->data != NULL) && (cmd->size > 0)) {
        switch (cmd->ins) {
#ifdef USE_DMA
            case Write_RAM:
                DMA_SetSrcAddress(DMA_DEV, (uint32_t)cmd->data);
                DMA_SetCurrDataCounter(DMA_DEV, cmd->size);
                DMA_Cmd(DMA_DEV, ENABLE);
                QSPI_DMACmd(QSPI_DEV, QSPI_DMAReq_TX, ENABLE);
                DMA_SoftwareTrigger(DMA_DEV);
                return;
#endif
            default:
                PRTF_OS_LOG(NEWS_LOG, "send size: %u\n", cmd->size);
                QSPI_SendMultipleData(QSPI_DEV, cmd->data, cmd->size);
                break;
        }
    }

    GPIO_WriteBit(CHIP_GPIO_GRP, CHIP_GPIO_PIN, 1);
}

int ep15301t_init(void) {
    gpio_init();

    GPIO_WriteBit(BACK_GPIO_GRP, BACK_GPIO_PIN, 0);
    GPIO_WriteBit(BACK_GPIO_GRP, BACK_GPIO_PIN, 1);
    GPIO_WriteBit(CHIP_GPIO_GRP, CHIP_GPIO_PIN, 1);
    GPIO_WriteBit(RESET_GPIO_GRP, RESET_GPIO_PIN, 1);
    RTOS_DELAY_IF(10);
    GPIO_WriteBit(RESET_GPIO_GRP, RESET_GPIO_PIN, 0);
    RTOS_DELAY_IF(20);
    GPIO_WriteBit(RESET_GPIO_GRP, RESET_GPIO_PIN, 1);
    RTOS_DELAY_IF(20);
    GPIO_WriteBit(CHIP_GPIO_GRP, CHIP_GPIO_PIN, 1);

    /********** 生产商初始化指令 **********/

    uint8_t        data[14];
    ep15301t_cmd_t cmd = {.data = data};

#ifdef USE_FACT_INIT
#    define SET_CMD(x) cmd.ins = x;
#    define SET_ARG(x)                                                                   \
        cmd.data[0] = x;                                                                 \
        cmd.size    = 1;                                                                 \
        ep15301t_ctl(&cmd);
#    define ADD_ARG(i, x) cmd.data[i] = x;
#    define SEND_MUL_CMD(x)                                                              \
        cmd.size = x;                                                                    \
        ep15301t_ctl(&cmd);
#    define SEND_SIG_CMD(x)                                                              \
        cmd.ins  = x;                                                                    \
        cmd.size = 0;                                                                    \
        ep15301t_ctl(&cmd);
#    include "ep15301t.init"
#else
    //
#endif

    PRTF_OS_LOG(NEWS_LOG, "init ok!\n");
    return RT_EOK;
}

void ep15301t_fill(const ep15301t_area_t * const area,
                   const void * const            buf,
                   const uint8_t                 unit_size) {
    uint8_t        data[4];
    ep15301t_cmd_t cmd = {.data = data};

    data[0]  = area->x1 >> 8;
    data[1]  = area->x1;
    data[2]  = area->x2 >> 8;
    data[3]  = area->x2;
    cmd.ins  = Set_X_RAG;
    cmd.size = 4;
    ep15301t_ctl(&cmd);

    data[0]  = area->y1 >> 8;
    data[1]  = area->y1;
    data[2]  = area->y2 >> 8;
    data[3]  = area->y2;
    cmd.ins  = Set_Y_RAG;
    cmd.size = 4;
    ep15301t_ctl(&cmd);

    cmd.ins  = Write_RAM;
    cmd.data = (uint8_t *)buf;
    cmd.size = ((area->x2 - area->x1) * (area->y2 - area->y1)) * unit_size;
    PRTF_OS_LOG(NEWS_LOG, "area: (%u, %u, %u, %u) crt size: %u\n", area->x1, area->x2,
                area->y1, area->y2, cmd.size);
    ep15301t_ctl(&cmd);
}

#ifdef USE_TEST

#    define TEST_WIDTH 360
#    define TEST_HIGHT 360
#    define FLUSH_SIZE 2
#    define FLUSH_CNT (TEST_HIGHT / FLUSH_SIZE)
#    define DATA_SIZE (TEST_WIDTH * FLUSH_SIZE * 2)

static uint8_t test_data_r[DATA_SIZE] = {0};
static uint8_t test_data_b[DATA_SIZE] = {0};

__attribute__((noreturn)) void ep15301t_test(void * thread_args) {
    ep15301t_init();
    PRTF_OS_LOG(NEWS_LOG, "start test!\n");

    for (uint16_t i = 0; i < DATA_SIZE; i++) {
        if (i % 2 == 0) {
            test_data_r[i] = 0x00;
            test_data_b[i] = 0x1F;
        } else {
            test_data_r[i] = 0xF8;
            test_data_b[i] = 0x00;
        }
    }

    uint8_t         flag = 0;
    ep15301t_area_t area = {.x1 = 0, .x2 = TEST_WIDTH - 1};

    while (1) {
        for (uint8_t i = 0; i < FLUSH_CNT; i++) {
            area.y1 = i * FLUSH_SIZE;
            area.y2 = area.y1 + FLUSH_SIZE - 1;
            if (flag) {
                ep15301t_fill(&area, test_data_b, 2);
            } else {
                ep15301t_fill(&area, test_data_r, 2);
            }
            RTOS_DELAY_IF(10);
        }
        flag = (flag) ? 0 : 1;
    }
}

#endif

__attribute__((always_inline)) void ep15301t_dma_irq(void) {
    GPIO_WriteBit(CHIP_GPIO_GRP, CHIP_GPIO_PIN, 1);
    QSPI_DMACmd(QSPI_DEV, QSPI_DMAReq_TX, DISABLE);
#ifdef USE_DMA
    DMA_Cmd(DMA_DEV, DISABLE);
#endif

#ifdef USE_LVGL
    LVGL_DONE(LVGL_DISP);
#endif
}