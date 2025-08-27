#include "gc9a01_cfg.h"
#include "gc9a01_if.h"
#include <sc32_conf.h>
#include INC_LOG
#include INC_RTOS
#ifdef USE_LVGL
#    include <lvgl.h>
#endif

#define LCD_X_RANGE 240
#define LCD_Y_RANGE 240

#define WHITE 0xFFFF
#define BLACK 0x0000
#define BLUE 0x001F
#define BRED 0XF81F
#define GRED 0XFFE0
#define GBLUE 0X07FF
#define RED 0xF800
#define MAGENTA 0xF81F
#define GREEN 0x07E0
#define CYAN 0x7FFF
#define YELLOW 0xFFE0
#define BROWN 0XBC40
#define BRRED 0XFC07
#define GRAY 0X8430
#define DARKBLUE 0X01CF
#define LIGHTBLUE 0X7D7C
#define GRAYBLUE 0X5458
#define LIGHTGREEN 0X841F
#define LGRAY 0XC618
#define LGRAYBLUE 0XA651
#define LBBLUE 0X2B12

__attribute__((optnone)) static void lcd_write_bus(const uint8_t data) {
    CHIP_GPIO_GRP->PIN &= (~CHIP_GPIO_PIN);
    QSPI_DEV->QSPI_DATA = data;
    while (QSPI_DEV->QSPI_STS & QSPI_Flag_BUSY) {}
    CHIP_GPIO_GRP->PIN |= CHIP_GPIO_PIN;
}

static void lcd_write_8_data(const uint8_t data) {
    DC_GPIO_GRP->PIN |= DC_GPIO_PIN;
    lcd_write_bus(data);
}

static void lcd_write_16_data(const uint16_t data) {
    DC_GPIO_GRP->PIN |= DC_GPIO_PIN;
    lcd_write_bus(data >> 8);
    lcd_write_bus(data);
}

__attribute__((noinline)) static void lcd_write_reg(const uint8_t data) {
    DC_GPIO_GRP->PIN &= (~DC_GPIO_PIN);
    lcd_write_bus(data);
    DC_GPIO_GRP->PIN |= DC_GPIO_PIN;
}

static void
lcd_set_win(uint16_t start_p_x, uint16_t start_p_y, uint16_t end_p_x, uint16_t end_p_y) {
    lcd_write_reg(0x2a);
    lcd_write_16_data(start_p_x);
    lcd_write_16_data(end_p_x);
    lcd_write_reg(0x2b);
    lcd_write_16_data(start_p_y);
    lcd_write_16_data(end_p_y);
}

static void gpio_init(void) {
    GPIO_InitTypeDef gpio_struct = {.GPIO_DriveLevel = 0, .GPIO_Mode = GPIO_Mode_OUT_PP};

    gpio_struct.GPIO_Pin = RESET_GPIO_PIN;
    GPIO_Init(RESET_GPIO_GRP, &gpio_struct);

    gpio_struct.GPIO_Pin = CHIP_GPIO_PIN;
    GPIO_Init(CHIP_GPIO_GRP, &gpio_struct);

    gpio_struct.GPIO_Pin = DC_GPIO_PIN;
    GPIO_Init(DC_GPIO_GRP, &gpio_struct);

    QSPI_Write_ComSet(QSPI_DEV, QSPI_LMode_1Line, QSPI_DWidth_8bit, QSPI_CLKONLY_OFF);
}

int lcd_init(void) {
    gpio_init();

    GPIO_WriteBit(CHIP_GPIO_GRP, CHIP_GPIO_PIN, 1);
    GPIO_WriteBit(RESET_GPIO_GRP, RESET_GPIO_PIN, 1);
    RTOS_DELAY_IF(1);
    GPIO_WriteBit(RESET_GPIO_GRP, RESET_GPIO_PIN, 0);
    RTOS_DELAY_IF(1);
    GPIO_WriteBit(RESET_GPIO_GRP, RESET_GPIO_PIN, 1);
    RTOS_DELAY_IF(120);

    /********** 生产商初始化指令 **********/

#ifdef USE_FACT_INIT

    lcd_write_reg(0xEF);
    lcd_write_reg(0xEB);
    lcd_write_8_data(0x14);

    lcd_write_reg(0xFE);
    lcd_write_reg(0xEF);

    lcd_write_reg(0xEB);
    lcd_write_8_data(0x14);

    lcd_write_reg(0x84);
    lcd_write_8_data(0x40);
    lcd_write_reg(0x85);
    lcd_write_8_data(0xFF);
    lcd_write_reg(0x86);
    lcd_write_8_data(0xFF);
    lcd_write_reg(0x87);
    lcd_write_8_data(0xFF);
    lcd_write_reg(0x88);
    lcd_write_8_data(0x0A);
    lcd_write_reg(0x89);
    lcd_write_8_data(0x21);

    lcd_write_reg(0x8A);
    lcd_write_8_data(0x00);

    lcd_write_reg(0x8B);
    lcd_write_8_data(0x80);

    lcd_write_reg(0x8C);
    lcd_write_8_data(0x01);

    lcd_write_reg(0x8D);
    lcd_write_8_data(0x01);

    lcd_write_reg(0x8E);
    lcd_write_8_data(0xFF);

    lcd_write_reg(0x8F);
    lcd_write_8_data(0xFF);

    lcd_write_reg(0xB6);
    lcd_write_8_data(0x00);
    lcd_write_8_data(0x20);

    lcd_write_reg(0x36);
    lcd_write_8_data(0x08);

    lcd_write_reg(0x3A);
    lcd_write_8_data(0x55);

    lcd_write_reg(0x90);
    lcd_write_8_data(0x08);
    lcd_write_8_data(0x08);
    lcd_write_8_data(0x08);
    lcd_write_8_data(0x08);

    lcd_write_reg(0xBD);
    lcd_write_8_data(0x06);

    lcd_write_reg(0xBC);
    lcd_write_8_data(0x00);

    lcd_write_reg(0xFF);
    lcd_write_8_data(0x60);
    lcd_write_8_data(0x01);
    lcd_write_8_data(0x04);

    lcd_write_reg(0xC3);
    lcd_write_8_data(0x13);

    lcd_write_reg(0xC4);
    lcd_write_8_data(0x13);

    lcd_write_reg(0xC9);
    lcd_write_8_data(0x22);

    lcd_write_reg(0xBE);
    lcd_write_8_data(0x11);

    lcd_write_reg(0xE1);
    lcd_write_8_data(0x10);
    lcd_write_8_data(0x0E);

    lcd_write_reg(0xDF);
    lcd_write_8_data(0x21);
    lcd_write_8_data(0x0c);
    lcd_write_8_data(0x02);

    lcd_write_reg(0xF0);
    lcd_write_8_data(0x45);
    lcd_write_8_data(0x09);
    lcd_write_8_data(0x08);
    lcd_write_8_data(0x08);
    lcd_write_8_data(0x26);
    lcd_write_8_data(0x2A);

    lcd_write_reg(0xF1);
    lcd_write_8_data(0x43);
    lcd_write_8_data(0x70);
    lcd_write_8_data(0x72);
    lcd_write_8_data(0x36);
    lcd_write_8_data(0x37);
    lcd_write_8_data(0x6F);

    lcd_write_reg(0xF2);
    lcd_write_8_data(0x45);
    lcd_write_8_data(0x09);
    lcd_write_8_data(0x08);
    lcd_write_8_data(0x08);
    lcd_write_8_data(0x26);
    lcd_write_8_data(0x2A);

    lcd_write_reg(0xF3);
    lcd_write_8_data(0x43);
    lcd_write_8_data(0x70);
    lcd_write_8_data(0x72);
    lcd_write_8_data(0x36);
    lcd_write_8_data(0x37);
    lcd_write_8_data(0x6F);

    lcd_write_reg(0xED);
    lcd_write_8_data(0x1B);
    lcd_write_8_data(0x0B);

    lcd_write_reg(0xAE);
    lcd_write_8_data(0x77);

    lcd_write_reg(0xCD);
    lcd_write_8_data(0x63);

    lcd_write_reg(0x70);
    lcd_write_8_data(0x07);
    lcd_write_8_data(0x07);
    lcd_write_8_data(0x04);
    lcd_write_8_data(0x0E);
    lcd_write_8_data(0x0F);
    lcd_write_8_data(0x09);
    lcd_write_8_data(0x07);
    lcd_write_8_data(0x08);
    lcd_write_8_data(0x03);

    lcd_write_reg(0xE8);
    lcd_write_8_data(0x34);

    lcd_write_reg(0x62);
    lcd_write_8_data(0x18);
    lcd_write_8_data(0x0D);
    lcd_write_8_data(0x71);
    lcd_write_8_data(0xED);
    lcd_write_8_data(0x70);
    lcd_write_8_data(0x70);
    lcd_write_8_data(0x18);
    lcd_write_8_data(0x0F);
    lcd_write_8_data(0x71);
    lcd_write_8_data(0xEF);
    lcd_write_8_data(0x70);
    lcd_write_8_data(0x70);

    lcd_write_reg(0x63);
    lcd_write_8_data(0x18);
    lcd_write_8_data(0x11);
    lcd_write_8_data(0x71);
    lcd_write_8_data(0xF1);
    lcd_write_8_data(0x70);
    lcd_write_8_data(0x70);
    lcd_write_8_data(0x18);
    lcd_write_8_data(0x13);
    lcd_write_8_data(0x71);
    lcd_write_8_data(0xF3);
    lcd_write_8_data(0x70);
    lcd_write_8_data(0x70);

    lcd_write_reg(0x64);
    lcd_write_8_data(0x28);
    lcd_write_8_data(0x29);
    lcd_write_8_data(0xF1);
    lcd_write_8_data(0x01);
    lcd_write_8_data(0xF1);
    lcd_write_8_data(0x00);
    lcd_write_8_data(0x07);

    lcd_write_reg(0x66);
    lcd_write_8_data(0x3C);
    lcd_write_8_data(0x00);
    lcd_write_8_data(0xCD);
    lcd_write_8_data(0x67);
    lcd_write_8_data(0x45);
    lcd_write_8_data(0x45);
    lcd_write_8_data(0x10);
    lcd_write_8_data(0x00);
    lcd_write_8_data(0x00);
    lcd_write_8_data(0x00);

    lcd_write_reg(0x67);
    lcd_write_8_data(0x00);
    lcd_write_8_data(0x3C);
    lcd_write_8_data(0x00);
    lcd_write_8_data(0x00);
    lcd_write_8_data(0x00);
    lcd_write_8_data(0x01);
    lcd_write_8_data(0x54);
    lcd_write_8_data(0x10);
    lcd_write_8_data(0x32);
    lcd_write_8_data(0x98);

    lcd_write_reg(0x74);
    lcd_write_8_data(0x10);
    lcd_write_8_data(0x85);
    lcd_write_8_data(0x80);
    lcd_write_8_data(0x00);
    lcd_write_8_data(0x00);
    lcd_write_8_data(0x4E);
    lcd_write_8_data(0x00);

    lcd_write_reg(0x98);
    lcd_write_8_data(0x3e);
    lcd_write_8_data(0x07);

    lcd_write_reg(0x34);
    lcd_write_reg(0x21);
    lcd_write_reg(0x11);
    // lcd_write_reg(0x38); //Idle Mode OFF
    RTOS_DELAY_IF(120);
    lcd_write_reg(0x29);
    RTOS_DELAY_IF(10);
#endif

    PRTF_OS_LOG(NEWS_LOG, "init ok!\n");
    return RT_EOK;
}

void lcd_fill(const lcd_area_t * area, const uint8_t * buf) {
    lcd_set_win(area->start_p_x, area->start_p_y, area->end_p_x, area->end_p_y);
    lcd_write_reg(0x2c);
    const uint32_t total =
        (area->end_p_y - area->start_p_y + 1) * (area->end_p_x - area->start_p_x + 1) * 2;
    CHIP_GPIO_GRP->PIN &= (~CHIP_GPIO_PIN);
#ifdef USE_LVGL
    DMA_SetSrcAddress(DMA_DEV, (uint32_t)buf);
    DMA_SetCurrDataCounter(DMA_DEV, total);
    DMA_Cmd(DMA_DEV, ENABLE);
    QSPI_DMACmd(QSPI_DEV, QSPI_DMAReq_TX, ENABLE);
    DMA_SoftwareTrigger(DMA_DEV);
#else
    for (uint32_t i = 0; i < total; i++) {
        QSPI_DEV->QSPI_DATA = buf[i];
        while (QSPI_DEV->QSPI_STS & QSPI_Flag_BUSY) {}
    }
    CHIP_GPIO_GRP->PIN |= CHIP_GPIO_PIN;
#endif
}

#ifdef USE_TEST

#    define TEST_WIDTH 64
#    define TEST_HIGHT 64
#    define X_OFS 64
#    define Y_OFS 64
#    define DATA_SIZE (TEST_WIDTH * TEST_HIGHT * 2)

static uint8_t test_data_r[DATA_SIZE] = {0};
static uint8_t test_data_b[DATA_SIZE] = {0};

__attribute__((noreturn)) void lcd_test(void * thread_args) {
    lcd_init();
    PRTF_OS_LOG(NEWS_LOG, "start test!\n");

    for (uint16_t i = 0; i < DATA_SIZE; i++) {
        if (i % 2 == 0) {
            test_data_r[i] = 0xFF;
            test_data_b[i] = 0x00;
        } else {
            test_data_r[i] = 0xFF;
            test_data_b[i] = 0x00;
        }
    }

    uint8_t    flag = 0;
    lcd_area_t area = {.start_p_x = 0 + X_OFS,
                       .start_p_y = 0 + Y_OFS,
                       .end_p_x   = TEST_WIDTH + X_OFS,
                       .end_p_y   = TEST_HIGHT + Y_OFS};

    while (1) {
        if (flag) {
            lcd_fill(&area, test_data_r);
        } else {
            lcd_fill(&area, test_data_b);
        }
        RTOS_DELAY_IF(80);
        flag = (flag) ? 0 : 1;
    }
}

#endif

#ifdef USE_DMA
__attribute__((always_inline)) void lcd_dma_irq(void) {
    CHIP_GPIO_GRP->PIN |= CHIP_GPIO_PIN;
    QSPI_DMACmd(QSPI_DEV, QSPI_DMAReq_TX, DISABLE);
    DMA_Cmd(DMA_DEV, DISABLE);

#    ifdef USE_LVGL
    LVGL_DONE(LVGL_DISP);
#    endif
}
#endif