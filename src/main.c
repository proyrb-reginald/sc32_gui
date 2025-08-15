#include <main.h>
#include INC_RTOS
#include INC_LOG
#include <fs_if.h>

int main(void) {
    /* rt_thread_t tid =
        rt_thread_create("ebd_fs_test", ebd_fs_test, RT_NULL, (12 * 64), 2, 10);

    if (tid != RT_NULL)
        rt_thread_startup(tid); */

    ebd_fs_init();
    fs_ls_dir(ebd_fs_hdl(), "/");
    while (1) {
        rt_uint32_t total, used, max_used;
        rt_memory_info(&total, &used, &max_used);
        PRTF_OS_LOG(NEWS_LOG, "heap: %u(%u)/%u\n", used, max_used, total);
        RTOS_DELAY_IF(5000);
    }
}