#include <main.h>
#include INC_RTOS
#include INC_LOG
#include <ep15301t_if.h>

int main(void) {
    while (1) {
        ep15301t_init();
        PRTF_OS_LOG(NEWS_LOG, "ok\n");
        RTOS_DELAY_IF(3000);
    }
}
