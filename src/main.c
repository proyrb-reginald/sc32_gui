#include <main.h>
#include INC_RTOS
#include INC_LOG
#include <fs_if.h>
#include <drvs_if.h>

int main(void) {
    fs_mnt(FS_ROM);
    fs_rcd_boot_cnt();
    PRTF_OS_LOG(NEWS_LOG, "boot cnt: %u\n", fs_get_boot_cnt());
    fs_ls("/");
    fs_ls("/sys");
    fs_unmnt(FS_ROM);

    rt_thread_t tid = rt_thread_create("test", w25q_test, RT_NULL, 1024,
                                       RT_THREAD_PRIORITY_MAX - 1, 10);
    if (tid != RT_NULL) {
        rt_thread_startup(tid);
    } else {
        PRTF_OS_LOG(ERRO_LOG, "thread create fail!\n");
    }

    while (1) {
        rt_uint32_t total, used, max_used;
        rt_memory_info(&total, &used, &max_used);
        PRTF_OS_LOG(NEWS_LOG, "heap: %u(%u)/%u\n", used, max_used, total);
        RTOS_DELAY_IF(3000);
    }
}