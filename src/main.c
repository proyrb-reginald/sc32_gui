#include <main.h>
#include INC_RTOS
#include INC_LOG
#include <drvs_if.h>
#include <fs_if.h>

int main(void) {
    ebd_fs_init();
    fs_reset_boot_cnt(ebd_fs_hdl());

    while (1) {
        // fs_update_boot_cnt(ebd_fs_hdl());
        // PRTF_OS_LOG(NEWS_LOG, "boot_cnt: %u\n", fs_get_boot_cnt(ebd_fs_hdl()));

        rt_uint32_t total, used, max_used;
        rt_memory_info(&total, &used, &max_used);
        PRTF_OS_LOG(NEWS_LOG, "heap: %u(%u)/%u\n", used, max_used, total);

        RTOS_DELAY_IF(1000);
    }
}