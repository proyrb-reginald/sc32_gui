#include <main.h>
#include INC_RTOS
#include INC_LOG
#include <fs_if.h>
#include <drvs_if.h>

int main(void) {
    {
        /**
         * @brief 片上ROM文件系统测试。
         * @retval
         * @warning
         * @note
         */
        fs_mnt(FS_ROM);
        fs_rcd_boot_cnt();
        PRTF_OS_LOG(NEWS_LOG, "rom boot cnt: %u\n", fs_get_boot_cnt());
        fs_ls("0:/");
        fs_ls("0:/sys");
        fs_unmnt();
    }
    {
        /**
         * @brief 外部Flash文件系统测试。
         * @retval
         * @warning
         * @note
         */
        fs_mnt(FS_FLASH);
        fs_rcd_boot_cnt();
        PRTF_OS_LOG(NEWS_LOG, "flash boot cnt: %u\n", fs_get_boot_cnt());
        fs_ls("1:/");
        fs_ls("1:/sys");
        fs_unmnt();
    }
    // {
    //     /**
    //      * @brief EEPROM测试任务。
    //      * @retval
    //      * @warning
    //      * @note
    //      */
    //     rt_thread_t tid = rt_thread_create("rom", rom_test, RT_NULL, 512,
    //                                        RT_THREAD_PRIORITY_MAX - 1, 10);
    //     if (tid != RT_NULL) {
    //         rt_thread_startup(tid);
    //     } else {
    //         PRTF_OS_LOG(ERRO_LOG, "create thread fail!\n");
    //     }
    // }

    /**
     * @brief 主任务，用于周期性输出系统内存使用情况。
     * @retval
     * @warning
     * @note
     */
    while (1) {
        rt_uint32_t total, used, max_used;
        rt_memory_info(&total, &used, &max_used);
        PRTF_OS_LOG(NEWS_LOG, "heap: %u(%u)/%u\n", used, max_used, total);
        RTOS_DELAY_IF(3000);
    }
}