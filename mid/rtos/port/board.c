#include <rtthread.h>
#include <bsp.h>
#include <sc32_conf.h>
#include <drvs_if.h>

void rt_hw_board_init(void) {
    rcc_init();
    uart1_init();
    // qspi0_init();
    qspi1_init();
    dma0_init();
    // dma1_init();
    // dma2_init();

#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    extern int Image$$RW_IRAM1$$ZI$$Limit;
    rt_system_heap_init((void *)&Image$$RW_IRAM1$$ZI$$Limit,
                        (void *)(0x20000000 + 32 * 1024));
#endif
}

// INIT_PREV_EXPORT(ep15301t_init);

#ifdef RT_USING_CONSOLE
void rt_hw_console_output(const char * str) {
    rt_enter_critical();  // 进入临界区（关闭调度器）
    const uint32_t len = rt_strlen(str);
    for (uint32_t i = 0; i < len; ++i) {
        printf_uart->UART_DATA = (str[i] & (uint16_t)0x01FF);
        while (!(printf_uart->UART_STS & UART_Flag_TX)) {}
        printf_uart->UART_STS = UART_Flag_TX;
    }
    rt_exit_critical();  // 退出临界区（恢复调度器）
}
#endif

#ifdef RT_USING_FINSH
char rt_hw_console_getchar(void) {
    char ch = -1;
    return ch;
}
#endif