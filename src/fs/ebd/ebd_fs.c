#define EBD_FS_C

#include "ebd_fs_cfg.h"
#include "ebd_fs_if.h"
#include <sc32_conf.h>
#include INC_LOG
#include INC_RTOS

static lfs ebd_fs;

int ebd_fs_init(void) {
    lfs_cfg ebd_fs_cfg = {
        .read  = ebd_fs_read,
        .erase = ebd_fs_erase,
        .prog  = ebd_fs_write,
        .sync  = ebd_fs_sync,

        .read_size      = 4,
        .prog_size      = 4,
        .block_size     = BLK_SIZE,
        .block_count    = FS_BLK_CNT,
        .block_cycles   = BLK_CYCLE,
        .cache_size     = FS_CACHE_SIZE,
        .lookahead_size = (FS_BLK_CNT >> 3),
    };

    int rst = lfs_mount(&ebd_fs, &ebd_fs_cfg);

    if (rst) {
        PRTF_OS_LOG(ERRO_LOG, "mnt ebd_fs fail!\n");
        rst = lfs_format(&ebd_fs, &ebd_fs_cfg);

        if (rst) {
            PRTF_OS_LOG(ERRO_LOG, "fmt ebd_fs fail!\n");
            return rst;
        } else {
            PRTF_OS_LOG(NEWS_LOG, "fmt ebd_fs done!\n");
        }

        rst = lfs_mount(&ebd_fs, &ebd_fs_cfg);
        if (rst) {
            PRTF_OS_LOG(ERRO_LOG, "mnt ebd_fs fail!\n");
            return rst;
        }
    }

    PRTF_OS_LOG(NEWS_LOG, "mnt ebd_fs done!\n");

    return LFS_ERR_OK;
}

lfs * ebd_fs_hdl(void) {
    return &ebd_fs;
}

int ebd_fs_read(const lfs_cfg * cfg,
                lfs_block_t     blk,
                lfs_off_t       ofs,
                void *          buf,
                lfs_size_t      size) {
    uint32_t adr = FS_START_ADR + (blk * BLK_SIZE) + ofs;
    uint32_t cnt = size >> 2;

    PRTF_OS_LOG(INFO_LOG, "read: blk: %u ofs: %u size: %u adr: 0x%x\n", blk, ofs, size,
                adr);

    if (IAP_ReadWordArray(adr, buf, cnt) < cnt) {
        PRTF_OS_LOG(ERRO_LOG, "read err!\n");
        return LFS_ERR_IO;
    }

    PRTF_LOG(TRAC_LOG, "read buf: ");
    for (uint32_t i = 0; i < size; i++) {
        PRTF_LOG(TRAC_LOG, "0x%02x ", ((uint8_t *)buf)[i]);
    }
    PRTF_LOG(TRAC_LOG, "\n");

    return LFS_ERR_OK;
}

int ebd_fs_erase(const lfs_cfg * cfg, lfs_block_t blk) {
    PRTF_OS_LOG(INFO_LOG, "erase: blk: %u\n", blk);

    IAP_Unlock();
    IAP_EraseSector((FS_OST_ADR / BLK_SIZE) + blk);
    IAP_Lock();

    return LFS_ERR_OK;
}

int ebd_fs_write(const lfs_cfg * cfg,
                 lfs_block_t     blk,
                 lfs_off_t       ofs,
                 const void *    buf,
                 lfs_size_t      size) {
    uint32_t adr = FS_START_ADR + (blk * BLK_SIZE) + ofs;
    uint32_t cnt = size >> 2;

    PRTF_OS_LOG(INFO_LOG, "write: blk: %u ofs: %u size: %u adr: 0x%x\n", blk, ofs, size,
                adr);

    PRTF_LOG(TRAC_LOG, "write buf: ");
    for (uint32_t i = 0; i < size; i++) {
        PRTF_LOG(TRAC_LOG, "0x%02x ", ((uint8_t *)buf)[i]);
    }
    PRTF_LOG(TRAC_LOG, "\n");

    IAP_Unlock();
    IAP_WriteCmd(ENABLE);
    if (IAP_ProgramWordArray(adr, (uint32_t *)buf, cnt) < cnt) {
        PRTF_OS_LOG(ERRO_LOG, "write err!\n");
        IAP_Lock();
        return LFS_ERR_IO;
    }
    IAP_Lock();

    return LFS_ERR_OK;
}

int ebd_fs_sync(const lfs_cfg * cfg) {
    return LFS_ERR_OK;
}