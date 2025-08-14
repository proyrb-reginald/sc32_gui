#define ETN_FS_C

#include "etn_fs.h"
#include "log.h"
#include "w25q64.h"
#include "lvgl.h"
#include "fs.h"
#include "user.h"

static lfs etn_fs;

int etn_fs_init(void) {
    lfs_cfg etn_fs_cfg = {
        .read  = etn_fs_read,
        .erase = etn_fs_erase,
        .prog  = etn_fs_write,
        .sync  = etn_fs_sync,

        .read_size      = 4,
        .prog_size      = 4,
        .block_size     = ETN_BLK_SIZE,
        .block_count    = ETN_FS_BLK_CNT,
        .block_cycles   = ETN_BLK_CYCLE,
        .cache_size     = ETN_FS_CACHE_SIZE,
        .lookahead_size = (ETN_FS_BLK_CNT >> 3),
    };

    int rst = lfs_mount(&etn_fs, &etn_fs_cfg);

    if (rst) {
        OS_PRTF(ERRO_LOG, "mount etn_fs fail!");
        rst = lfs_format(&etn_fs, &etn_fs_cfg);

        if (rst) {
            OS_PRTF(ERRO_LOG, "fmt etn_fs fail!");
            return rst;
        } else {
            OS_PRTF(NEWS_LOG, "fmt etn_fs done!");
        }

        rst = lfs_mount(&etn_fs, &etn_fs_cfg);
        if (rst) {
            OS_PRTF(ERRO_LOG, "mount etn_fs fail again!");
            return rst;
        }
    }

    OS_PRTF(NEWS_LOG, "mount etn_fs done!");

    /********** 写入图片数据 **********/

#if WRITE_RES
    LV_IMG_DECLARE(sbg);
    rst = lfs_remove(&etn_fs, RES_FILE);
    rst = fs_write_res(&etn_fs, &sbg, RES_FILE_PATH);
    if (rst < 0) {
        OS_PRTF(ERRO_LOG, "write res fail with %d!", rst);
    }
#endif  // WRITE_RES

    /********** 更新启动信息 **********/

#if RST_BOOT_INFO
    rst = fs_reset_boot_cnt(&etn_fs);
    if (rst) {
        OS_PRTF(ERRO_LOG, "rst boot fail with %d!", rst);
    }
#endif
    rst = fs_update_boot_cnt(&etn_fs);
    rst = fs_get_boot_cnt(&etn_fs);
    if (rst > -1) {
        OS_PRTF(NEWS_LOG, "boot cnt: %d!", rst);
    }

    /********** 输出文件系统内容 **********/

    rst = fs_ls_dir(&etn_fs, "/");
    rst = fs_ls_dir(&etn_fs, SYS_DIR);
    rst = fs_ls_dir(&etn_fs, RES_DIR);

    return pdPASS;
}

lfs * etn_fs_hdl(void) {
    return &etn_fs;
}

int etn_fs_read(const lfs_cfg * cfg,
                lfs_block_t     blk,
                lfs_off_t       ofs,
                void *          buf,
                lfs_size_t      size) {
    uint32_t adr = ETN_FS_START_ADR + (blk * ETN_BLK_SIZE) + ofs;
    uint32_t cnt = size >> 2;

    OS_PRTF(INFO_LOG, "read: blk: %u ofs: %u size: %u adr: 0x%x", blk, ofs, size, adr);

    w25q64_arg read_arg = {
        .adr  = (uint8_t *)&adr,
        .data = buf,
        .size = size,
    };

    w25q64_ctl(ReadDataSPI, &read_arg);
    // w25q64_ctl(ReadDataQSPI, &read_arg);

    PRTF(TRAC_LOG, "read buf: ");
    for (uint32_t i = 0; i < size; i++) {
        PRTF(TRAC_LOG, "0x%02x ", ((uint8_t *)buf)[i]);
    }
    PRTF(TRAC_LOG, "\n");

    return LFS_ERR_OK;
}

int etn_fs_erase(const lfs_cfg * cfg, lfs_block_t blk) {
    OS_PRTF(INFO_LOG, "erase: blk: %u", blk);

    const uint32_t adr = blk * ETN_BLK_SIZE;

    w25q64_ctl(WriteEnable, NULL);

    uint8_t    SR1;
    w25q64_arg erase_arg = {
        .adr  = (uint8_t *)&adr,
        .data = &SR1,
        .size = 1,
    };

    w25q64_ctl(Erase4KB, &erase_arg);
    do {
        vTaskDelay(5);
        w25q64_ctl(ReadSR1, &erase_arg);
    } while (SR1 & 0x1);

    return LFS_ERR_OK;
}

int etn_fs_write(const lfs_cfg * cfg,
                 lfs_block_t     blk,
                 lfs_off_t       ofs,
                 const void *    buf,
                 lfs_size_t      size) {
    uint32_t adr = ETN_FS_START_ADR + (blk * ETN_BLK_SIZE) + ofs;
    uint32_t cnt = size >> 2;

    OS_PRTF(INFO_LOG, "write: blk: %u ofs: %u size: %u adr: 0x%x", blk, ofs, size, adr);

    PRTF(TRAC_LOG, "write buf: ");
    for (uint32_t i = 0; i < size; i++) {
        PRTF(TRAC_LOG, "0x%02x ", ((uint8_t *)buf)[i]);
    }
    PRTF(TRAC_LOG, "\n");

    while (size > 0) {
        uint32_t page_ofs = adr & (ETN_PAGE_SIZE - 1);

        w25q64_ctl(WriteEnable, NULL);

        w25q64_arg write_arg = {
            .adr  = (uint8_t *)&adr,
            .data = (uint8_t *)buf,
            .size = 256 - page_ofs,
        };

        if (write_arg.size > size) {
            write_arg.size = size;
        }
        w25q64_ctl(ProgramQSPI, &write_arg);

        uint8_t    SR1;
        w25q64_arg read_arg = {
            .data = &SR1,
            .size = 1,
        };

        do {
            vTaskDelay(1);
            w25q64_ctl(ReadSR1, &read_arg);
        } while (SR1 & 0x1);

        adr += write_arg.size;
        buf += write_arg.size;
        size -= write_arg.size;
    }

    return LFS_ERR_OK;
}

int etn_fs_sync(const lfs_cfg * cfg) {
    return LFS_ERR_OK;
}
