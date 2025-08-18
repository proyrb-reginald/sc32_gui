/**
 * @file rom_cfg.h
 * @author proyrb (proyrb@yeah.net)
 * @brief 片上ROM配置。
 * @version 0.1
 * @date 2025-08-18
 * @copyright Copyright (c) 2025
 * @note
 */

#ifndef __ROM_CFG_H__
#define __ROM_CFG_H__

/****************************** 导入头文件 ******************************/

#include <sc32_conf.h>

/****************************** 驱动配置 ******************************/

/* 配置片上ROM基地址 */
#define ROM_BASE_ADR FLASH_BASE

/* 配置片上ROM块大小（即最小物理擦除扇区） */
#define ROM_SCT_SIZE SECTOR_SIZE

/* 配置片上ROM块的最大擦写次数 */
#define ROM_SCT_CYCLE 10000

/* 配置片上文件系统相对ROM的地址偏移量 */
#define ROM_FS_OFS_ADR 0x00028000

/* 配置片上文件系统偏移后的起始地址 */
#define ROM_FS_START_ADR (ROM_BASE_ADR + ROM_FS_OFS_ADR)

/* 配置片上文件系统尾地址 */
#define ROM_FS_END_ADR (ROM_BASE_ADR + FLASH_SIZE)

/* 配置片上文件系统的扇区数量 */
#define ROM_FS_SCT_CNT ((FLASH_SIZE - ROM_FS_OFS_ADR) / ROM_SCT_SIZE)

#endif