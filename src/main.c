#include <main.h>
#include INC_RTOS
#include INC_LOG
#include <fs_if.h>

int main(void) {
    fs_mnt(FS_ROM);
    fs_rcd_boot_cnt();
    PRTF_OS_LOG(NEWS_LOG, "boot cnt: %u\n", fs_get_boot_cnt());
    while (1) {
        rt_uint32_t total, used, max_used;
        rt_memory_info(&total, &used, &max_used);
        PRTF_OS_LOG(NEWS_LOG, "heap: %u(%u)/%u\n", used, max_used, total);
        RTOS_DELAY_IF(2000);
    }
}