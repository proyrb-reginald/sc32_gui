#include <bsp.h>
#include <sc32_conf.h>
#include <rtthread.h>
#include <drvs_if.h>

void rcc_init(void) {
    if (RCC_Unlock(0xFF) != SUCCESS)
        while (1) {}

    RCC_ITConfig(DISABLE);
    RCC_HXTCmd(DISABLE);
    RCC_LXTCmd(DISABLE);
    RCC_HIRCCmd(ENABLE);
    RCC_LIRCCmd(DISABLE);
    RCC_HIRCDIV1Cmd(ENABLE);
    RCC_HCLKConfig(RCC_SYSCLK_Div1);

    if (RCC_SYSCLKConfig(RCC_SYSCLKSource_HIRC) != SUCCESS)
        while (1) {}

    NVIC_SetPriority(SysTick_IRQn, 0);
    NVIC_EnableIRQ(SysTick_IRQn);
    RCC_SystickCLKConfig(RCC_SysTickSource_HCLK_DIV8);
    RCC_SystickSetCounter(8000);
    RCC_SystickCmd(ENABLE);
}

void uart1_init(void) {
    UART_InitTypeDef uart_struct = {.UART_ClockFrequency = 64000000,
                                    .UART_BaudRate       = 115200,
                                    .UART_Mode           = UART_Mode_10B};

    RCC_APB0Config(RCC_HCLK_Div1);
    RCC_APB0Cmd(ENABLE);
    RCC_APB0PeriphClockCmd(RCC_APB0Periph_UART1, ENABLE);
    UART_Init(UART1, &uart_struct);
    UART_TXCmd(UART1, ENABLE);
    Printf_UartInit(UART1);
}

void qspi0_init(void) {
    QSPI_InitTypeDef qspi_struct = {.QSPI_SShift    = QSPI_SShift_OFF,
                                    .QSPI_Prescaler = QSPI_Prescaler_8,
                                    .QSPI_Mode      = QSPI_Mode_QSPI,
                                    .QSPI_CPMode    = QSPI_CPMode_Low};

    RCC_APB0Config(RCC_HCLK_Div1);
    RCC_APB0Cmd(ENABLE);
    RCC_APB0PeriphClockCmd(RCC_APB0Periph_QTWI0, ENABLE);
    QSPI_Init(QSPI0, &qspi_struct);
    QSPI_Cmd(QSPI0, ENABLE);
}

void qspi1_init(void) {
    QSPI_InitTypeDef qspi_struct = {.QSPI_SShift    = QSPI_SShift_OFF,
                                    .QSPI_Prescaler = QSPI_Prescaler_16,
                                    .QSPI_Mode      = QSPI_Mode_QSPI,
                                    .QSPI_CPMode    = QSPI_CPMode_Low};

    RCC_APB1Config(RCC_HCLK_Div1);
    RCC_APB1Cmd(ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_QTWI1, ENABLE);
    QSPI_Init(QSPI1, &qspi_struct);
    QSPI_Cmd(QSPI1, ENABLE);
}

void spi2_init(void) {
    RCC_APB0PeriphClockCmd(RCC_APB0Periph_QTWI2, ENABLE);  // 使能时钟
    SPI_InitTypeDef spi_struct = {.SPI_Mode      = SPI_Mode_Master,
                                  .SPI_DataSize  = SPI_DataSize_8B,
                                  .SPI_CPHA      = SPI_CPHA_1Edge,
                                  .SPI_CPOL      = SPI_CPOL_Low,
                                  .SPI_FirstBit  = SPI_FirstBit_MSB,
                                  .SPI_Prescaler = SPI_Prescaler_4};
    SPI_Init(SPI2, &spi_struct);                // 初始化
    SPI_PinRemapConfig(SPI2, SPI_PinRemap_C);   // 设置引脚映射
    SPI_ITConfig(SPI2, SPI_IT_INTEN, DISABLE);  // 禁用中断
    SPI_DMACmd(SPI2, SPI_DMAReq_TX, DISABLE);   // 关闭发送DMA请求
    SPI_DMACmd(SPI2, SPI_DMAReq_RX, DISABLE);   // 关闭发送DMA请求
    SPI_Cmd(SPI2, ENABLE);                      // 使能
}

void dma0_init(void) {
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA, ENABLE);
    DMA_InitTypeDef dma_struct = {.DMA_Priority     = DMA_Priority_HIGH,
                                  .DMA_CircularMode = DMA_CircularMode_Disable,
                                  .DMA_DataSize     = DMA_DataSize_Byte,
                                  .DMA_TargetMode   = DMA_TargetMode_FIXED,
                                  .DMA_SourceMode   = DMA_SourceMode_INC,
                                  .DMA_Burst        = DMA_Burst_Disable,
                                  .DMA_Request      = DMA_Request_TWI_SPI2_TX,
                                  .DMA_DstAddress   = (uint32_t)&(SPI2->SPI_DATA)};
    DMA_Init(DMA0, &dma_struct);
    DMA_ITConfig(DMA0, DMA_IT_INTEN, ENABLE);
    DMA_ITConfig(DMA0, DMA_IT_TCIE, ENABLE);
    NVIC_SetPriority(DMA0_IRQn, 1);
    NVIC_EnableIRQ(DMA0_IRQn);
    DMA_Cmd(DMA0, DISABLE);
}

void dma1_init(void) {
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA, ENABLE);
    DMA_InitTypeDef dma_struct = {.DMA_Priority     = DMA_Priority_MEDIUM,
                                  .DMA_CircularMode = DMA_CircularMode_Disable,
                                  .DMA_DataSize     = DMA_DataSize_Byte,
                                  .DMA_TargetMode   = DMA_TargetMode_FIXED,
                                  .DMA_SourceMode   = DMA_SourceMode_INC,
                                  .DMA_Burst        = DMA_Burst_Disable,
                                  .DMA_Request      = DMA_Request_TWI_QSPI0_TX,
                                  .DMA_DstAddress   = (uint32_t)&(QSPI0->QSPI_DATA)};
    DMA_Init(DMA1, &dma_struct);
    DMA_ITConfig(DMA1, DMA_IT_INTEN, ENABLE);
    DMA_ITConfig(DMA1, DMA_IT_TCIE, ENABLE);
    NVIC_SetPriority(DMA1_IRQn, 1);
    NVIC_EnableIRQ(DMA1_IRQn);
    DMA_Cmd(DMA1, DISABLE);
}

void dma2_init(void) {
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA, ENABLE);
    DMA_InitTypeDef dma_struct = {.DMA_Priority     = DMA_Priority_MEDIUM,
                                  .DMA_CircularMode = DMA_CircularMode_Disable,
                                  .DMA_DataSize     = DMA_DataSize_Byte,
                                  .DMA_TargetMode   = DMA_TargetMode_INC,
                                  .DMA_SourceMode   = DMA_SourceMode_FIXED,
                                  .DMA_Burst        = DMA_Burst_Disable,
                                  .DMA_Request      = DMA_Request_TWI_QSPI0_RX,
                                  .DMA_SrcAddress   = (uint32_t)&(QSPI0->QSPI_DATA)};
    DMA_Init(DMA2, &dma_struct);
    DMA_ITConfig(DMA2, DMA_IT_INTEN, ENABLE);
    DMA_ITConfig(DMA2, DMA_IT_TCIE, ENABLE);
    NVIC_SetPriority(DMA2_IRQn, 1);
    NVIC_EnableIRQ(DMA2_IRQn);
    DMA_Cmd(DMA2, DISABLE);
}

__attribute__((interrupt("IRQ"))) void SysTick_Handler(void) {
    rt_interrupt_enter();
    rt_tick_increase();

#ifdef USE_LVGL
    lv_tick_inc(1);
#endif

    rt_interrupt_leave();
}

__attribute__((interrupt("IRQ"))) void UART1_3_5_IRQHandler(void) {
    rt_interrupt_enter();
    if (UART_GetFlagStatus(printf_uart, UART_Flag_RX)) {
        UART_ClearFlag(printf_uart, UART_Flag_RX);
    }
    rt_interrupt_leave();
}

// __attribute__((interrupt("IRQ"))) void DMA0_IRQHandler(void) {
//     rt_interrupt_enter();
//     DMA_ClearFlag(DMA0, DMA_FLAG_GIF | DMA_FLAG_TCIF | DMA_FLAG_HTIF | DMA_FLAG_TEIF);
//     rt_interrupt_leave();
// }

// __attribute__((interrupt("IRQ"))) void DMA1_IRQHandler(void) {
//     rt_interrupt_enter();
//     DMA_ClearFlag(DMA1, DMA_FLAG_GIF | DMA_FLAG_TCIF | DMA_FLAG_HTIF | DMA_FLAG_TEIF);
//     w25q_dma_tx_irq();
//     rt_interrupt_leave();
// }

// __attribute__((interrupt("IRQ"))) void DMA2_IRQHandler(void) {
//     rt_interrupt_enter();
//     DMA_ClearFlag(DMA2, DMA_FLAG_GIF | DMA_FLAG_TCIF | DMA_FLAG_HTIF | DMA_FLAG_TEIF);
//     w25q_dma_rx_irq();
//     rt_interrupt_leave();
// }