/**
 * @file ebd_fs_if.h
 * @author proyrb (proyrb@yeah.net)
 * @brief 片上文件系统接口。
 * @version 0.1
 * @date 2025-08-14
 * @copyright Copyright (c) 2025
 * @note
 */

#ifndef EBD_FS_H
#define EBD_FS_H

/****************************** 导入头文件 ******************************/

#include <lfs.h>

/****************************** 类型定义 ******************************/

typedef struct lfs_config lfs_cfg;
typedef struct lfs        lfs;

/****************************** 函数接口 ******************************/

/**
 * @brief 初始化片上文件系统。
 * @return int
 * @retval 请参考lfs_error枚举值。
 * @warning 必须先执行一次初始化后才能进行后续操作。
 * @note
 */
int ebd_fs_init(void);

/**
 * @brief 获取片上文件系统实例。
 * @return lfs*
 * @retval 0 文件系统未初始化或初始化失败。
 * @retval !0 文件系统句柄。
 * @warning
 * @note
 */
lfs * ebd_fs_hdl(void);

/**
 * @brief 读取片上文件系统指定位置的数据。
 * @param cfg 文件系统实例。
 * @param blk 操作的起始块。
 * @param ofs 块的偏移地址。
 * @param buf 数据读取缓冲区。
 * @param size 读取的字节数。
 * @return int
 * @retval 请参考lfs_error枚举值。
 * @warning
 * @note
 */
int ebd_fs_read(const lfs_cfg * cfg,
                lfs_block_t     blk,
                lfs_off_t       ofs,
                void *          buf,
                lfs_size_t      size);

/**
 * @brief 擦除片上文件系统指定块。
 * @param cfg 文件系统实例。
 * @param blk 操作的块。
 * @return int
 * @retval 请参考lfs_error枚举值。。
 * @warning
 * @note
 */
int ebd_fs_erase(const lfs_cfg * cfg, lfs_block_t blk);

/**
 * @brief 向片上文件系统指定位置写入指定数据。
 * @param cfg 文件系统实例。
 * @param blk 操作的起始块。
 * @param ofs 块的偏移地址。
 * @param buf 数据写入缓冲区。
 * @param size 写入的字节数。
 * @return int
 * @retval 请参考lfs_error枚举值。
 * @warning
 * @note
 */
int ebd_fs_write(const lfs_cfg * cfg,
                 lfs_block_t     blk,
                 lfs_off_t       ofs,
                 const void *    buf,
                 lfs_size_t      size);

/**
 * @brief 将缓冲区的数据同步到物理内存中。
 * @param cfg 文件系统实例。
 * @return int
 * @retval 请参考lfs_error枚举值。
 * @warning
 * @note
 */
int ebd_fs_sync(const lfs_cfg * cfg);

#endif