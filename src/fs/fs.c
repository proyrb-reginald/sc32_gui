#include "fs_cfg.h"
#include "fs_if.h"
#include INC_LOG
#include INC_RTOS

static FATFS    fs;
static uint32_t g_boot_cnt = 0;

FRESULT fs_mnt(const fs_dev_t drv) {
    FRESULT res = 0;
    switch (drv) {
        case FS_ROM:
            res = f_mount(&fs, STR_VALUE(DEV_ROM) ":", 1);
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

                    res = f_mkfs(STR_VALUE(DEV_ROM) ":", &opt, work, FF_MAX_SS);
                    if (res == FR_OK) {
                        PRTF_OS_LOG(NEWS_LOG, "f_mkfs ok\n");
                    } else {
                        PRTF_OS_LOG(ERRO_LOG, "f_mkfs fail with %d\n", res);
                        res = 2;
                    }

                    res = f_mount(&fs, STR_VALUE(DEV_ROM) ":", 1);
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

FRESULT fs_rcd_boot_cnt(void) {
    FRESULT  res;
    FIL      fil;
    UINT     bw;
    uint32_t cnt;

    /* 1. 确保 /sys 目录存在，不存在就创建 */
    res = f_mkdir(ROOT_SYS_D);
    if (res != FR_OK && res != FR_EXIST) {
        PRTF_OS_LOG(ERRO_LOG, "mkdir fail with %u!\n", res);
        return res;
    }

    /* 2. 尝试打开文件 */
    res = f_open(&fil, BOOT_FILE_P, FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
    if (res != FR_OK) {
        PRTF_OS_LOG(ERRO_LOG, "open fail with %u!\n", res);
        return res;
    }

    /* 3. 读已有值或新建文件 */
    if (f_size(&fil) >= sizeof(uint32_t)) {
        /* 文件已存在并且有数据，读出旧值 */
        res = f_read(&fil, &cnt, sizeof(cnt), &bw);
        if (res != FR_OK || bw != sizeof(cnt)) {
            f_close(&fil);
            PRTF_OS_LOG(ERRO_LOG, "read fail with %u!\n", res);
            return res;
        }
    } else {
        /* 新建文件，初始化为 1 */
        cnt = 0;
    }

    /* 4. 更新启动次数 */
    cnt++;

    /* 5. 写回文件，覆盖原内容 */
    res = f_lseek(&fil, 0);
    if (res != FR_OK) {
        PRTF_OS_LOG(ERRO_LOG, "seek fail with %u!\n", res);
        return res;
    }
    res = f_write(&fil, &cnt, sizeof(cnt), &bw);
    if (res != FR_OK) {
        PRTF_OS_LOG(ERRO_LOG, "write fail with %u!\n", res);
        return res;
    }

    /* 6. 截断文件长度，防止旧数据残留（可选） */
    res = f_truncate(&fil);
    if (res != FR_OK) {
        PRTF_OS_LOG(ERRO_LOG, "truncate fail with %u!\n", res);
        return res;
    }

    /* 7. 更新全局变量 */
    res = f_close(&fil);
    if (res != FR_OK) {
        PRTF_OS_LOG(ERRO_LOG, "truncate fail with %u!\n", res);
        return res;
    }
    g_boot_cnt = cnt;

    return res;
}

uint32_t fs_get_boot_cnt(void) {
    return g_boot_cnt;
}

void fs_ls(const char * const path) {
    DIR     dir;
    FILINFO fno;
    PRTF_OS_LOG(NEWS_LOG, "list %s\n", path);
    if (f_opendir(&dir, path) == FR_OK) {
        while (f_readdir(&dir, &fno) == FR_OK && fno.fname[0]) {
            PRTF_LOG(NEWS_LOG, "%-16s ", fno.fname);
            if (fno.fattrib & AM_DIR) {
                PRTF_LOG(NEWS_LOG, "[DIR]\n");
            } else {
                PRTF_LOG(NEWS_LOG, "%lu bytes\n", fno.fsize);
            }
        }
        f_closedir(&dir);
    }
}