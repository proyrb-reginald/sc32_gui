/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2025        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"     /* Basic definitions of FatFs */
#include "diskio.h" /* Declarations FatFs MAI */

/* 启用日志 */
#define LOG_LEV INFO_LOG
#define LOG_IF_PRTF rt_kprintf
#define LOG_IF_GET_TICK rt_tick_get
#define RTOS_DELAY_IF rt_thread_delay

/* Declarations of the platform and disk functions in the project */
#include <sc32_conf.h>
#include <rtthread.h>
#include <fs_cfg.h>
#include <log.h>
#include <build_time.h>
#include <time.h>
#include <drvs_if.h>

/* Mapping of physical drive number for each drive */
#ifndef DEV_ROM
#    define DEV_ROM 0 /* Map FTL to physical drive 0 */
#endif
#ifndef DEV_FLASH
#    define DEV_FLASH 1 /* Map FTL to physical drive 1 */
#endif
#ifndef DEV_SD
#    define DEV_SD 2 /* Map MMC/SD card to physical drive 2 */
#endif

/*-----------------------------------------------------------------------*/
/* Get current time                                                      */
/*-----------------------------------------------------------------------*/

DWORD get_fattime(void) {
    time_t      rawtime  = (time_t)(BUILD_TIME + rt_tick_get() / 1000);
    struct tm * timeinfo = localtime(&rawtime);

    if (timeinfo == NULL) {
        return 0;
    }

    return (DWORD)(timeinfo->tm_year - 80) << 25 | (DWORD)(timeinfo->tm_mon + 1) << 21 |
           (DWORD)timeinfo->tm_mday << 16 | (DWORD)timeinfo->tm_hour << 11 |
           (DWORD)timeinfo->tm_min << 5 | (DWORD)timeinfo->tm_sec >> 1;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(BYTE pdrv) {
    DSTATUS status = STA_NOINIT;
    switch (pdrv) {
#ifdef USE_FS_ROM
        case DEV_ROM:
            status = 0;
            PRTF_OS_LOG(NEWS_LOG, "rom disk init!\n");
            break;
#endif
#ifdef USE_FS_FLASH
        case DEV_FLASH:
            status = w25q_init();
            PRTF_OS_LOG(NEWS_LOG, "flash disk init with %u!\n", status);
            break;
#endif
#ifdef USE_FS_SD
        case DEV_SD: break;
#endif
        default: break;
    }
    return status;
}

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(BYTE pdrv) {
    DSTATUS status = STA_NOINIT;

    switch (pdrv) {
#ifdef USE_FS_ROM
        case DEV_ROM: status = 0; break;
#endif
#ifdef USE_FS_FLASH
        case DEV_FLASH: status = 0; break;
#endif
#ifdef USE_FS_SD
        case DEV_SD: break;
#endif
    }

    return status;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(BYTE pdrv, BYTE * buf, LBA_t sector, UINT count) {
    DRESULT res = RES_PARERR;

    switch (pdrv) {
#ifdef USE_FS_ROM
#    define SCT_SIZE ROM_SCT_SIZE
        case DEV_ROM: {
            uint32_t adr = ROM_FS_START_ADR + sector * SCT_SIZE;

            for (UINT i = 0; i < count; i++) {
                if (IAP_ReadByteArray(adr, buf, SCT_SIZE) < SCT_SIZE) {
                    PRTF_OS_LOG(ERRO_LOG, "read sct: %u cnt: %u adr: 0x%08x fail!\n",
                                sector, count, adr);
                    return RES_ERROR;
                }
                adr += SCT_SIZE;
                buf += SCT_SIZE;
            }

            res = RES_OK;
            break;
        }
#    undef SCT_SIZE
#endif

#ifdef USE_FS_FLASH
#    define SCT_SIZE (4 * 1024)
        case DEV_FLASH: {
            uint32_t adr = sector * SCT_SIZE;

            PRTF_OS_LOG(INFO_LOG, "read sct: %u cnt: %u adr: 0x%08x\n", sector, count,
                        adr);

            uint8_t * const raw_buf = buf;

            for (UINT i = 0; i < count; i++) {
                w25q_cmd_t cmd = {.ins  = ReadDataQSPI,
                                  .adr  = {.byte = {adr >> 16, adr >> 8, adr >> 0}},
                                  .data = buf,
                                  .size = SCT_SIZE};
                w25q_ctl(&cmd);

                adr += SCT_SIZE;
                buf += SCT_SIZE;
            }

            PRTF_OS_LOG(INFO_LOG, "data:\n");
            for (uint16_t i = 0; i < 64; i++) {
                PRTF_LOG(INFO_LOG, "0x%02x ", raw_buf[i]);
            }
            PRTF_LOG(INFO_LOG, "\n");

            res = RES_OK;
            break;
        }
#    undef SCT_SIZE
#endif

#ifdef USE_FS_SD
#    define SCT_SIZE
        case DEV_SD: break;
#    undef SCT_SIZE
#endif
        default: break;
    }

    return res;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0
DRESULT disk_write(BYTE pdrv, const BYTE * buf, LBA_t sector, UINT count) {
    DRESULT res;

    switch (pdrv) {
#    ifdef USE_FS_ROM
#        define SCT_SIZE ROM_SCT_SIZE
        case DEV_ROM: {
            uint32_t adr = ROM_FS_START_ADR + sector * SCT_SIZE;

            IAP_Unlock();
            IAP_EraseSector(adr / SCT_SIZE);
            IAP_WriteCmd(ENABLE);

            for (UINT i = 0; i < count; i++) {
                if (IAP_ProgramByteArray(adr, (uint8_t *)buf, SCT_SIZE) < SCT_SIZE) {
                    IAP_Lock();
                    PRTF_OS_LOG(ERRO_LOG, "write sct: %u cnt: %u adr: 0x%x fail!\n",
                                sector, count, adr);
                    return RES_ERROR;
                }
                adr += SCT_SIZE;
                buf += SCT_SIZE;
            }

            IAP_Lock();
            res = RES_OK;

            break;
        }
#        undef SCT_SIZE
#    endif

#    ifdef USE_FS_FLASH
#        define SCT_SIZE (4 * 1024)
        case DEV_FLASH: {
            uint32_t adr = sector * SCT_SIZE;

            PRTF_OS_LOG(INFO_LOG, "write sct: %u cnt: %u adr: 0x%08x\n", sector, count,
                        adr);

            PRTF_OS_LOG(INFO_LOG, "data:\n");
            for (uint16_t i = 0; i < 64; i++) {
                PRTF_LOG(INFO_LOG, "0x%02x ", buf[i]);
            }
            PRTF_LOG(INFO_LOG, "\n");

            for (UINT i = 0; i < count; i++) {
                // 擦除扇区（4KB）
                w25q_cmd_t cmd = {.ins = WriteEnable, .size = 0};
                w25q_ctl(&cmd);

                cmd.ins         = Erase4KB;
                cmd.adr.byte[0] = adr >> 16;
                cmd.adr.byte[1] = adr >> 8;
                cmd.adr.byte[2] = adr >> 0;
                cmd.size        = 0;
                w25q_ctl(&cmd);

                // 等待擦除完成
                uint8_t sr1 = 0;
                cmd.ins     = ReadSR1;
                cmd.data    = &sr1;
                cmd.size    = 1;
                do {
                    RTOS_DELAY_IF(3);
                    w25q_ctl(&cmd);
                } while (sr1 & 0x01);

                // 写入扇区：拆分为 16 页 × 256 字节
                for (int page = 0; page < 16; page++) {
                    cmd.ins  = WriteEnable;
                    cmd.size = 0;
                    w25q_ctl(&cmd);

                    cmd.ins         = ProgramQSPI;
                    cmd.adr.byte[0] = (adr + page * 256) >> 16;
                    cmd.adr.byte[1] = (adr + page * 256) >> 8;
                    cmd.adr.byte[2] = (adr + page * 256) >> 0;
                    cmd.data        = (uint8_t *)buf + page * 256;
                    cmd.size        = 256;
                    w25q_ctl(&cmd);

                    // 等待写入完成
                    cmd.ins  = ReadSR1;
                    cmd.data = &sr1;
                    cmd.size = 1;
                    do {
                        RTOS_DELAY_IF(2);
                        w25q_ctl(&cmd);
                    } while (sr1 & 0x01);
                }

                adr += SCT_SIZE;
                buf += SCT_SIZE;
            }

            res = RES_OK;
            break;
        }
#        undef SCT_SIZE
#    endif

#    ifdef USE_FS_SD
#        define SCT_SIZE
        case DEV_SD: break;
#        undef SCT_SIZE
#    endif
        default: break;
    }
    return res;
}
#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void * buf) {
    DRESULT res = RES_PARERR;

    switch (pdrv) {
#ifdef USE_FS_ROM
        case DEV_ROM: {
            switch (cmd) {
                case CTRL_SYNC: res = RES_OK; break;
                case GET_SECTOR_COUNT:
                    *(LBA_t *)buf = ROM_FS_SCT_CNT;
                    res           = RES_OK;
                    break;
                case GET_SECTOR_SIZE:
                    *(WORD *)buf = ROM_SCT_SIZE;
                    res          = RES_OK;
                    break;
                case GET_BLOCK_SIZE:
                    *(DWORD *)buf = ROM_SCT_SIZE;
                    res           = RES_OK;
                    break;
                default: break;
            }
            break;
        }
#endif
#ifdef USE_FS_FLASH
        case DEV_FLASH: {
            switch (cmd) {
                case CTRL_SYNC: res = RES_OK; break;
                case GET_SECTOR_COUNT:
                    *(LBA_t *)buf = (16 * 255);
                    res           = RES_OK;
                    break;
                case GET_SECTOR_SIZE:
                    *(WORD *)buf = (4 * 1024);
                    res          = RES_OK;
                    break;
                case GET_BLOCK_SIZE:
                    *(DWORD *)buf = (4 * 1024);
                    res           = RES_OK;
                    break;
                default: break;
            }
            break;
        }
#endif
#ifdef USE_FS_SD
        case DEV_SD: break;
#endif
    }

    return res;
}