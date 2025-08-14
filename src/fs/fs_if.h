/**
 * @file fs_if.h
 * @author proyrb (proyrb@yeah.net)
 * @brief 顶层文件系统接口。
 * @version 0.1
 * @date 2025-08-14
 * @copyright Copyright (c) 2025
 * @note
 */

#ifndef __FS_IF_H__
#define __FS_IF_H__

/****************************** 导入头文件 ******************************/

#include <lfs.h>
#include "ebd/ebd_fs_if.h"

/****************************** 类型定义 ******************************/

typedef struct lfs_info lfs_info;
typedef enum lfs_error  lfs_error;

/****************************** 函数接口 ******************************/

/**
 * @brief 获取系统启动次数。
 * @param lfs 文件系统实例。
 * @return lfs_error
 * @retval 非负数表示启动的次数。
 * @retval -1 表示文件读取数据量有误。
 * @retval 其余请参考lfs_error枚举值。
 * @warning
 * @note
 */
lfs_error fs_get_boot_cnt(lfs_t * const lfs);

/**
 * @brief 更新系统启动次数。
 * @param lfs 文件系统实例。
 * @return lfs_error
 * @retval -1 表示文件写入数据量有误。
 * @retval 其余请参考lfs_error枚举值。
 * @warning
 * @note
 */
lfs_error fs_update_boot_cnt(lfs_t * const lfs);

/**
 * @brief 重置系统启动次数。
 * @param lfs 文件系统实例。
 * @return lfs_error
 * @retval 请参考lfs_error枚举值。
 * @warning
 * @note
 */
lfs_error fs_reset_boot_cnt(lfs_t * const lfs);

/**
 * @brief 列出指定目录的内容。
 * @param lfs 文件系统实例。
 * @param dir 指定的目录。
 * @return lfs_error
 * @retval 请参考lfs_error枚举值。
 * @warning 非递归。
 * @note
 */
lfs_error fs_ls_dir(lfs_t * const lfs, const char * dir);

/**
 * @brief 向特定目录写入指定的图片数据。
 * @param lfs 文件系统实例。
 * @param res 图片资源数据。
 * @param name 写入后的图片文件的名称。
 * @return lfs_error
 * @retval 请参考lfs_error枚举值。
 * @warning
 * @note
 */
// lfs_error
// fs_write_res(lfs_t * const lfs, const lv_image_dsc_t * const res, const char * name);

#endif