#define ST77916_C

#include <st77916.h>
#include <rtthread.h>
#include <log.h>
// #include <lvgl.h>
// #include <lv_port_disp.h>

static void gpio_init(void) {
    GPIO_InitTypeDef gpio_init_struct = {.GPIO_DriveLevel = 0,
                                         .GPIO_Mode       = GPIO_Mode_OUT_PP};

    /* LCD复位引脚 */
    gpio_init_struct.GPIO_Pin = GPIO_Pin_14;
    GPIO_Init(GPIOA, &gpio_init_struct);

    /* LCD片选引脚 */
    gpio_init_struct.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOB, &gpio_init_struct);
}

int st77916_init(void) {
    GPIO_WriteBit(CHIP_GPIO_GRP, CHIP_GPIO_PIN, 1);
    GPIO_WriteBit(RESET_GPIO_GRP, RESET_GPIO_PIN, 1);
    rt_thread_delay(1);
    GPIO_WriteBit(RESET_GPIO_GRP, RESET_GPIO_PIN, 0);
    rt_thread_delay(1);
    GPIO_WriteBit(RESET_GPIO_GRP, RESET_GPIO_PIN, 1);
    rt_thread_delay(120);
    GPIO_WriteBit(CHIP_GPIO_GRP, CHIP_GPIO_PIN, 1);
    OS_PRTF(INFO_LOG, "reset done!\n");

    uint8_t       data[2];
    st77916_arg_t arg;

    st77916_ctl(Wake, NULL);
    rt_thread_delay(120);
    OS_PRTF(INFO_LOG, "wake done!\n");

    data[0]  = 0x0;
    arg.data = data;
    arg.size = 1;
    st77916_ctl(SetRAMReadMode, &arg);
    OS_PRTF(INFO_LOG, "set read mode!\n");

    data[0]  = 0x55;
    arg.data = data;
    arg.size = 1;
    st77916_ctl(SetColorFmt, &arg);
    OS_PRTF(INFO_LOG, "set color fmt!\n");

    data[0]  = 0x00;
    data[1]  = 0x08;
    arg.data = data;
    arg.size = 2;
    st77916_ctl(SetRGB, &arg);
    OS_PRTF(INFO_LOG, "set rgb fmt!\n");

    st77916_ctl(OnReverse, NULL);
    OS_PRTF(INFO_LOG, "reverse done!\n");

    st77916_ctl(OnDisplay, NULL);
    rt_thread_delay(50);

    OS_PRTF(NEWS_LOG, "init done!\n");

    return RT_EOK;
}

__attribute__((always_inline)) void st77916_dma_irq(void) {
    GPIO_WriteBit(CHIP_GPIO_GRP, CHIP_GPIO_PIN, 1);
    SPI_DMACmd(USE_SPI, SPI_DMAReq_TX, DISABLE);
    DMA_Cmd(USE_DMA, DISABLE);
    LVGL_DONE(LVGL_DISP);
}

__attribute__((optnone)) void st77916_ctl(const st77916_cmd_t         cmd,
                                          const st77916_arg_t * const arg) {
    GPIO_WriteBit(MODE_GPIO_GRP, MODE_GPIO_PIN, 0);
    GPIO_WriteBit(CHIP_GPIO_GRP, CHIP_GPIO_PIN, 0);

    SPI_SendData(USE_SPI, cmd);
    while (SPI_GetFlagStatus(USE_SPI, SPI_FLAG_QTWIF) == 0) {}

    if ((arg != NULL) && (arg->size > 0)) {
        GPIO_WriteBit(MODE_GPIO_GRP, MODE_GPIO_PIN, 1);
        switch (cmd) {
            case Write: {
                DMA_SetSrcAddress(USE_DMA, (uint32_t)arg->data);
                DMA_SetCurrDataCounter(USE_DMA, arg->size);
                DMA_Cmd(USE_DMA, ENABLE);
                SPI_DMACmd(USE_SPI, SPI_DMAReq_TX, ENABLE);
                DMA_SoftwareTrigger(USE_DMA);
                return;
            }
            default: {
                for (uint8_t i = 0; i < arg->size; ++i) {
                    SPI_SendData(USE_SPI, arg->data[i]);
                    while (SPI_GetFlagStatus(USE_SPI, SPI_FLAG_QTWIF) == 0) {}
                }
                break;
            }
        }
    }

    GPIO_WriteBit(CHIP_GPIO_GRP, CHIP_GPIO_PIN, 1);
}

void st77916_async_fill(const st77916_area_t * const area,
                        const void * const           buf,
                        const uint8_t                pixel_size) {
    uint8_t       data[4];
    st77916_arg_t arg = {.data = data};

    /* 设置列地址 */
    data[0]  = area->x1 >> 8;
    data[1]  = area->x1;
    data[2]  = area->x2 >> 8;
    data[3]  = area->x2;
    arg.size = 4;
    st77916_ctl(SetColumn, &arg);

    /* 设置行地址 */
    data[0]  = area->y1 >> 8;
    data[1]  = area->y1;
    data[2]  = area->y2 >> 8;
    data[3]  = area->y2;
    arg.size = 4;
    st77916_ctl(SetRow, &arg);

    /* 设置开始传输 */
    arg.data = (const uint8_t *)buf;
    arg.size = lv_area_get_size((const lv_area_t *)area) * pixel_size;
    st77916_ctl(Write, &arg);
}

#if st77916_TEST

#    define TEST_WIDTH 240
#    define TEST_HIGHT 320
#    define FLUSH_SIZE 4
#    define FLUSH_CNT (TEST_HIGHT / FLUSH_SIZE)
#    define DATA_SIZE (TEST_WIDTH * FLUSH_SIZE * 2)

static uint8_t test_data_r[DATA_SIZE] = {0};
static uint8_t test_data_b[DATA_SIZE] = {0};

void st77916_test(void * thread_args) {
    OS_PRTF(NEWS_LOG, "start test!\n");

    for (uint16_t i = 0; i < DATA_SIZE; i++) {
        if (i % 2 == 0) {
            test_data_r[i] = 0x00;
            test_data_b[i] = 0x1F;
        } else {
            test_data_r[i] = 0xF8;
            test_data_b[i] = 0x00;
        }
    }

    uint8_t        flag = 0;
    st77916_area_t area = {.x1 = 0, .x2 = TEST_WIDTH - 1};

    while (1) {
        for (uint8_t i = 0; i < FLUSH_CNT; i++) {
            area.y1 = i * FLUSH_SIZE;
            area.y2 = area.y1 + FLUSH_SIZE - 1;
            if (flag) {
                st77916_async_fill(&area, test_data_b, 2);
            } else {
                st77916_async_fill(&area, test_data_r, 2);
            }
            rt_thread_delay(3);
        }
        flag = (flag) ? 0 : 1;
    }
}

#endif
