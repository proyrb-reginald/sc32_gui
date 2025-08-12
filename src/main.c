#include <rtthread.h>

int main(void) {
    while(1) {
        rt_kprintf("Hello\n");
        rt_thread_delay(3000);
    }
}
