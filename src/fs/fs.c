#include "fs_cfg.h"
#include "fs_if.h"
#include INC_LOG
#include INC_RTOS

static FATFS fs;

uint8_t fs_mnt(const fs_dev_t drv) {
    uint8_t res = 0;
    switch (drv) {
        case FS_ROM:
            res = f_mount(&fs, "0:", 1);
            if (res != FR_OK) {
                PRTF_OS_LOG(ERRO_LOG, "mnt fail with %d!\n", res);

                BYTE * const work = rt_malloc(FF_MAX_SS);
                if (work == RT_NULL) {
                    PRTF_OS_LOG(ERRO_LOG, "rt_malloc fail!\n");
                    res = 1;
                } else {
                    MKFS_PARM opt = {
                        .fmt     = FM_FAT,
                        .n_fat   = 1,
                        .align   = 1,
                        .n_root  = 16,
                        .au_size = 512,
                    };

                    res = f_mkfs("0:", &opt, work, FF_MAX_SS);
                    if (res == FR_OK) {
                        PRTF_OS_LOG(NEWS_LOG, "f_mkfs ok\n");
                    } else {
                        PRTF_OS_LOG(ERRO_LOG, "f_mkfs fail with %d\n", res);
                        res = 2;
                    }

                    res = f_mount(&fs, "0:", 1);
                    if (res != FR_OK) {
                        PRTF_OS_LOG(ERRO_LOG, "mnt fail with %d!\n", res);
                        res = 3;
                    } else {
                        PRTF_OS_LOG(ERRO_LOG, "mnt ok!\n");
                    }
                    rt_free((void *)work);
                }
            } else {
                PRTF_OS_LOG(ERRO_LOG, "mnt ok!\n");
            }
            break;
        case FS_FLASH: break;
        case FS_SD: break;
        default: break;
    }
    return res;
}