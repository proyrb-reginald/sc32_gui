#ifndef ETN_FS_H
#define ETN_FS_H

/**
 * @brief 这是片内文件系统模块。
 * @details 先初始化模块。
 * @file ebd_fs.h
 * @author proyrb
 * @date 2025/7/28
 * @note
 */

/********** 导入需要的头文件 **********/

#include "lfs.h"

/********** 片外空间配置 **********/

#ifdef ETN_FS_C
#    define ETN_BASE_ADR 0x0
#    define ETN_SIZE 0x800000
#    define ETN_PAGE_SIZE 256
#    define ETN_BLK_SIZE 4096
#    define ETN_BLK_CYCLE 10000
#    define ETN_FS_OST_ADR 0x0
#    define ETN_FS_START_ADR (ETN_BASE_ADR + ETN_FS_OST_ADR)
#    define ETN_FS_END_ADR (ETN_BASE_ADR + ETN_SIZE)
#    define ETN_FS_BLK_CNT ((ETN_SIZE - ETN_FS_OST_ADR) / ETN_BLK_SIZE)
#    define ETN_FS_CACHE_SIZE 256
#endif  // ETN_FS_C

/********** 模块行为 **********/

#ifdef ETN_FS_C
// 是否写入图片数据
#    define WRITE_RES 0
#endif  // ETN_FS_C

/********** 类型定义 **********/

typedef struct lfs_config lfs_cfg;
typedef struct lfs        lfs;

/********** 导出的函数 **********/

/**
 * @brief 初始化片外文件系统。
 * @param
 * @retval
 * @warning 必须先初始化模块后才能进行后续操作。
 * @note
 */
int etn_fs_init(void);

/**
 * @brief 获取片外文件系统实例。
 * @param
 * @retval
 * @warning
 * @note
 */
lfs * etn_fs_hdl(void);

/**
 * @brief 片外读取。
 * @param cfg 文件系统实例。
 * @param blk 操作的起始块。
 * @param ofs 块的偏移地址。
 * @param buf 数据读取缓冲区。
 * @param size 读取的字节数。
 * @retval 请参考lfs_error枚举值。
 * @warning
 * @note
 */
int etn_fs_read(const lfs_cfg * cfg,
                lfs_block_t     blk,
                lfs_off_t       ofs,
                void *          buf,
                lfs_size_t      size);

/**
 * @brief 片外擦除。
 * @param cfg 文件系统实例。
 * @param blk 操作的块。
 * @retval 请参考lfs_error枚举值。。
 * @warning
 * @note
 */
int etn_fs_erase(const lfs_cfg * cfg, lfs_block_t blk);

/**
 * @brief 片外写入。
 * @param cfg 文件系统实例。
 * @param blk 操作的起始块。
 * @param ofs 块的偏移地址。
 * @param buf 数据写入缓冲区。
 * @param size 写入的字节数。
 * @retval 请参考lfs_error枚举值。
 * @warning
 * @note
 */
int etn_fs_write(const lfs_cfg * cfg,
                 lfs_block_t     blk,
                 lfs_off_t       ofs,
                 const void *    buf,
                 lfs_size_t      size);

/**
 * @brief 将缓冲的数据同步写入到片外。
 * @param cfg 文件系统实例。
 * @retval 请参考lfs_error枚举值。
 * @warning
 * @note
 */
int etn_fs_sync(const lfs_cfg * cfg);

#endif  // ETN_FS_H
