#define __FS_C__

#include "fs_cfg.h"
#include "fs_if.h"
#include INC_LOG
#include INC_RTOS

static FATFS fs;

#ifdef USE_FS_ROM
static uint32_t rom_boot_cnt = 0;
#    define FS_ROM_CHAR STR_VALUE(DEV_ROM) ":"
#endif

#ifdef USE_FS_FLASH
static uint32_t flash_boot_cnt = 0;
#    define FS_FLASH_CHAR STR_VALUE(DEV_FLASH) ":"
#endif

#ifdef USE_FS_SD
static uint32_t sd_boot_cnt = 0;
#    define FS_SD_CHAR STR_VALUE(DEV_SD) ":"
#endif

static fs_dev_t crt_dev = FS_NONE;

FRESULT fs_mnt(const fs_dev_t dev) {
    crt_dev = dev;

    const char * dev_char;
    switch (crt_dev) {
#ifdef USE_FS_ROM
        case FS_ROM: dev_char = FS_ROM_CHAR; break;
#endif
#ifdef USE_FS_FLASH
        case FS_FLASH: dev_char = FS_FLASH_CHAR; break;
#endif
#ifdef USE_FS_SD
        case FS_SD: dev_char = FS_SD_CHAR; break;
#endif
        default: PRTF_OS_LOG(ERRO_LOG, "no dev to mnt!\n"); return FR_INVALID_DRIVE;
    }

    FRESULT res = f_mount(&fs, dev_char, 1);
    if (res != FR_OK) {
        PRTF_OS_LOG(ERRO_LOG, "mnt fail with %d!\n", res);

        BYTE * const work_buf = rt_malloc_align(FF_MAX_SS, 4);
        if (work_buf == RT_NULL) {
            PRTF_OS_LOG(ERRO_LOG, "rt_malloc fail!\n");
            res = FR_NOT_ENOUGH_CORE;
        } else {
            MKFS_PARM opt = {.fmt = FM_FAT, .align = 1, .n_root = 16};
            switch (dev) {
                case FS_ROM:
                    opt.n_fat   = 1;
                    opt.au_size = 512;
                    break;
                case FS_FLASH:
                    opt.n_fat   = 2;
                    opt.au_size = (4 * 1024);
                    break;
                case FS_SD: break;
                default: break;
            }

            res = f_mkfs(dev_char, &opt, work_buf, FF_MAX_SS);
            if (res != FR_OK) {
                PRTF_OS_LOG(ERRO_LOG, "f_mkfs fail with %d\n", res);
                rt_free_align((void *)work_buf);
            } else {
                PRTF_OS_LOG(NEWS_LOG, "f_mkfs ok\n");

                res = f_mount(&fs, dev_char, 1);
                if (res != FR_OK) {
                    PRTF_OS_LOG(ERRO_LOG, "mnt fail with %d!\n", res);
                } else {
                    crt_dev = dev;
                    PRTF_OS_LOG(ERRO_LOG, "mnt ok!\n");
                }
            }
        }
    } else {
        PRTF_OS_LOG(ERRO_LOG, "mnt ok!\n");
    }

    return res;
}

FRESULT fs_unmnt(void) {
    const char * dev_char;
    switch (crt_dev) {
#ifdef USE_FS_ROM
        case FS_ROM: dev_char = FS_ROM_CHAR; break;
#endif
#ifdef USE_FS_FLASH
        case FS_FLASH: dev_char = FS_FLASH_CHAR; break;
#endif
#ifdef USE_FS_SD
        case FS_SD: dev_char = FS_SD_CHAR; break;
#endif
        default: PRTF_OS_LOG(ERRO_LOG, "no dev to unmnt!\n"); return FR_INVALID_DRIVE;
    }

    FRESULT res = f_unmount(dev_char);
    if (res != FR_OK) {
        PRTF_OS_LOG(ERRO_LOG, "unmnt fail with %u!\n", res);
    } else {
        crt_dev = FS_NONE;
        PRTF_OS_LOG(NEWS_LOG, "unmnt ok!\n");
    }

    return res;
}

FRESULT fs_rcd_boot_cnt(void) {
    /* 1. 确保 /sys 目录存在，不存在就创建 */
    FRESULT res;
    switch (crt_dev) {
#ifdef USE_FS_ROM
        case FS_ROM: res = f_mkdir(FS_ROM_CHAR ROOT_SYS_D); break;
#endif
#ifdef USE_FS_FLASH
        case FS_FLASH: res = f_mkdir(FS_FLASH_CHAR ROOT_SYS_D); break;
#endif
#ifdef USE_FS_SD
        case FS_SD: res = f_mkdir(FS_SD_CHAR ROOT_SYS_D); break;
#endif
        default: return FR_INVALID_DRIVE;
    }

    if (res != FR_OK && res != FR_EXIST) {
        PRTF_OS_LOG(ERRO_LOG, "mkdir fail with %u!\n", res);
    } else {
        /* 2. 尝试打开文件 */
        FIL fil;
        switch (crt_dev) {
#ifdef USE_FS_ROM
            case FS_ROM:
                res = f_open(&fil, FS_ROM_CHAR BOOT_FILE_P,
                             FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
                break;
#endif
#ifdef USE_FS_FLASH
            case FS_FLASH:
                res = f_open(&fil, FS_FLASH_CHAR BOOT_FILE_P,
                             FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
                break;
#endif
#ifdef USE_FS_SD
            case FS_SD:
                res = f_open(&fil, FS_SD_CHAR BOOT_FILE_P,
                             FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
                break;
#endif
            default: return FR_INVALID_DRIVE;
        }
        if (res != FR_OK) {
            PRTF_OS_LOG(ERRO_LOG, "open fail with %u!\n", res);
        } else {
            /* 3. 读已有值或新建文件 */
            UINT     bw;
            uint32_t cnt;
            if (f_size(&fil) >= sizeof(uint32_t)) {
                /* 文件已存在并且有数据，读出旧值 */
                res = f_read(&fil, &cnt, sizeof(cnt), &bw);
                if (res != FR_OK || bw != sizeof(cnt)) {
                    f_close(&fil);
                    PRTF_OS_LOG(ERRO_LOG, "read fail with %u!\n", res);
                    return res;
                }
            } else {
                /* 新建文件，初始化为 0 */
                cnt = 0;
            }

            /* 4. 更新启动次数 */
            cnt++;

            /* 5. 写回文件，覆盖原内容 */
            res = f_lseek(&fil, 0);
            if (res != FR_OK) {
                PRTF_OS_LOG(ERRO_LOG, "seek fail with %u!\n", res);
            } else {
                res = f_write(&fil, &cnt, sizeof(cnt), &bw);
                if (res != FR_OK) {
                    PRTF_OS_LOG(ERRO_LOG, "write fail with %u!\n", res);
                } else {
                    /* 6. 截断文件长度，防止旧数据残留（可选） */
                    res = f_truncate(&fil);
                    if (res != FR_OK) {
                        PRTF_OS_LOG(ERRO_LOG, "truncate fail with %u!\n", res);
                    } else {
                        /* 7. 更新全局变量 */
                        res = f_close(&fil);
                        if (res != FR_OK) {
                            PRTF_OS_LOG(ERRO_LOG, "truncate fail with %u!\n", res);
                        } else {
                            switch (crt_dev) {
#ifdef USE_FS_ROM
                                case DEV_ROM: rom_boot_cnt = cnt; break;
#endif
#ifdef USE_FS_FLASH
                                case DEV_FLASH: flash_boot_cnt = cnt; break;
#endif
#ifdef USE_FS_SD
                                case DEV_SD: sd_boot_cnt = cnt; break;
#endif
                                default: break;
                            }
                        }
                    }
                }
            }
        }
    }

    return res;
}

uint32_t fs_get_boot_cnt(void) {
    switch (crt_dev) {
#ifdef USE_FS_ROM
        case DEV_ROM: return rom_boot_cnt;
#endif
#ifdef USE_FS_FLASH
        case DEV_FLASH: return flash_boot_cnt;
#endif
#ifdef USE_FS_SD
        case DEV_SD: return sd_boot_cnt;
#endif
        default: return 0;
    }
}

FRESULT fs_ls(const char * const path) {
    DIR     dir;
    FRESULT res = f_opendir(&dir, path);

    if (res != FR_OK) {
        PRTF_OS_LOG(ERRO_LOG, "open dir fail with %u\n", res);
    } else {
        FILINFO fno;
        PRTF_OS_LOG(NEWS_LOG, "list %s\n", path);
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

    return res;
}