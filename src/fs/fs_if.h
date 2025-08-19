/**
 * @file fs_if.h
 * @author proyrb (proyrb@yeah.net)
 * @brief 顶层文件系统接口。
 * @version 0.1
 * @date 2025-08-18
 * @copyright Copyright (c) 2025
 * @note
 */

#ifndef __FS_IF_H__
#define __FS_IF_H__

/****************************** 导入头文件 ******************************/

#include <ff.h>

/****************************** 宏定义 ******************************/

#define STR(x) #x
#define STR_VALUE(x) STR(x)

/****************************** 类型定义 ******************************/

typedef enum {
    FS_NONE = -1,
    FS_ROM,
    FS_FLASH,
    FS_SD,
} fs_dev_t;

/****************************** 函数接口 ******************************/

/**
 * @brief 挂载指定文件系统。
 * @param dev 设备类型。
 * @return FRESULT
 * @retval 参考FRESULT枚举。
 * @warning
 * @note
 */
FRESULT fs_mnt(const fs_dev_t dev);

/**
 * @brief 卸载指定文件系统。
 * @param dev
 * @return FRESULT
 * @retval 参考FRESULT枚举。
 * @warning
 * @note
 */
FRESULT fs_unmnt(void);

/**
 * @brief 记录系统启动次数。
 * @return FRESULT
 * @retval 参考FRESULT枚举。
 * @warning
 * @note
 */
FRESULT fs_rcd_boot_cnt(void);

/**
 * @brief 返回当前系统启动次数。
 * @retval uint32_t
 * @retval 当前系统启动次数。
 * @warning
 * @note
 */
uint32_t fs_get_boot_cnt(void);

/**
 * @brief 输出指定目录内容及其属性。
 * @param path
 * @return FRESULT
 * @retval 参考FRESULT枚举。
 * @warning
 * @note
 */
FRESULT fs_ls(const char * const path);

#endif