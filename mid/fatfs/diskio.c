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
#define LOG_LEV NEWS_LOG
#define LOG_IF_PRTF rt_kprintf
#define LOG_IF_GET_TICK rt_tick_get

/* Declarations of the platform and disk functions in the project */
#include <sc32_conf.h>
#include <rtthread.h>
#include <log.h>
#include <build_time.h>
#include <time.h>
#include <drvs_if.h>
#include <fs_cfg.h>

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
            PRTF_OS_LOG(NEWS_LOG, "flash disk init!\n");
            break;
#endif
#ifdef USE_FS_FLASH
        case DEV_FLASH: break;
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
        case DEV_FLASH: break;
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

DRESULT disk_read(BYTE   pdrv,   /* Physical drive nmuber to identify the drive */
                  BYTE * buff,   /* Data buffer to store read data */
                  LBA_t  sector, /* Start sector in LBA */
                  UINT   count   /* Number of sectors to read */
) {
    DRESULT  res = RES_PARERR;
    uint32_t adr = ROM_FS_START_ADR + sector * SECTOR_SIZE;
    switch (pdrv) {
#ifdef USE_FS_ROM
        case DEV_ROM:
            for (UINT i = 0; i < count; i++) {
                if (IAP_ReadByteArray(adr, buff, SECTOR_SIZE) < SECTOR_SIZE) {
                    PRTF_OS_LOG(ERRO_LOG, "read sct: %u cnt: %u adr: 0x%x fail!\n",
                                sector, count, adr);
                    return RES_ERROR;
                }
                adr += SECTOR_SIZE;
                buff += SECTOR_SIZE;
            }
            res = RES_OK;
            break;
#endif
#ifdef USE_FS_FLASH
        case DEV_FLASH: break;
#endif
#ifdef USE_FS_SD
        case DEV_SD: break;
#endif
        default: break;
    }
    return res;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0
DRESULT disk_write(BYTE pdrv, const BYTE * buff, LBA_t sector, UINT count) {
    DRESULT  res;
    uint32_t adr = ROM_FS_START_ADR + sector * ROM_SCT_SIZE;
    switch (pdrv) {
#    ifdef USE_FS_ROM
        case DEV_ROM:
            IAP_Unlock();
            IAP_EraseSector(adr / ROM_SCT_SIZE);
            IAP_WriteCmd(ENABLE);
            for (UINT i = 0; i < count; i++) {
                if (IAP_ProgramByteArray(adr, (uint8_t *)buff, ROM_SCT_SIZE) <
                    ROM_SCT_SIZE) {
                    IAP_Lock();
                    PRTF_OS_LOG(ERRO_LOG, "write sct: %u cnt: %u adr: 0x%x fail!\n",
                                sector, count, adr);
                    return RES_ERROR;
                }
                adr += ROM_SCT_SIZE;
                buff += ROM_SCT_SIZE;
            }
            IAP_Lock();
            res = RES_OK;
            break;
#    endif
#    ifdef USE_FS_FLASH
        case DEV_FLASH: break;
#    endif
#    ifdef USE_FS_SD
        case DEV_SD: break;
#    endif
        default: break;
    }
    return res;
}
#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void * buff) {
    DRESULT res = RES_PARERR;
    switch (pdrv) {
#ifdef USE_FS_ROM
        case DEV_ROM:
            switch (cmd) {
                case CTRL_SYNC: res = RES_OK; break;
                case GET_SECTOR_COUNT:
                    *(LBA_t *)buff = ROM_FS_SCT_CNT;
                    res            = RES_OK;
                    break;
                case GET_SECTOR_SIZE:
                    *(WORD *)buff = ROM_SCT_SIZE;
                    res           = RES_OK;
                    break;
                case GET_BLOCK_SIZE:
                    *(DWORD *)buff = 1;
                    res            = RES_OK;
                    break;
                default: break;
            }
#endif
#ifdef USE_FS_FLASH
        case DEV_FLASH: break;
#endif
#ifdef USE_FS_SD
        case DEV_SD: break;
#endif
    }
    return res;
}