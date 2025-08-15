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

        .read_size      = 1,
        .prog_size      = 1,
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

    PRTF_OS_LOG(INFO_LOG, "read: blk: %u ofs: %u size: %u adr: 0x%x\n", blk, ofs, size,
                adr);

    uint16_t read_size = IAP_ReadByteArray(adr, (uint8_t *)buf, size);
    if (read_size < size) {
        PRTF_OS_LOG(ERRO_LOG, "read size: %u < %u!\n", read_size, size);
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

    PRTF_OS_LOG(INFO_LOG, "write: blk: %u ofs: %u size: %u adr: 0x%x\n", blk, ofs, size,
                adr);

    PRTF_LOG(TRAC_LOG, "write buf: ");
    for (uint32_t i = 0; i < size; i++) {
        PRTF_LOG(TRAC_LOG, "0x%02x ", ((uint8_t *)buf)[i]);
    }
    PRTF_LOG(TRAC_LOG, "\n");

    IAP_Unlock();
    IAP_WriteCmd(ENABLE);
    uint16_t wrt_size = IAP_ProgramByteArray(adr, (uint8_t *)buf, size);
    IAP_Lock();

    if (wrt_size < size) {
        PRTF_OS_LOG(ERRO_LOG, "write size: %u < %u!\n", wrt_size, size);
        return LFS_ERR_IO;
    }

    return LFS_ERR_OK;
}

int ebd_fs_sync(const lfs_cfg * cfg) {
    return LFS_ERR_OK;
}

#ifdef USE_TEST

#    define BUF_SIZE 64

static uint8_t test_read_buf[BUF_SIZE];
static uint8_t test_write_buf[BUF_SIZE];

void ebd_fs_test(void * thread_args) {
    for (uint16_t i = 0; i < BUF_SIZE; i++) {
        test_write_buf[i] = i;
    }

    while (1) {
        for (uint16_t blk = 0; blk < FS_BLK_CNT; blk++) {
            const uint32_t adr = FS_START_ADR + (blk * BLK_SIZE);

            PRTF_OS_LOG(ERRO_LOG, "\n[[%u] test]!\n", blk);

            IAP_Unlock();
            PRTF_LOG(NEWS_LOG, "erase blk: %u\n", (FS_OST_ADR / BLK_SIZE) + blk,
                     BLK_SIZE);
            IAP_EraseSector((FS_OST_ADR / BLK_SIZE) + blk);
            IAP_Lock();

            uint16_t read_size = IAP_ReadByteArray(adr, test_read_buf, BUF_SIZE);
            if (read_size < BUF_SIZE) {
                PRTF_OS_LOG(ERRO_LOG, "read size: %u < %u!\n", read_size, BUF_SIZE);
            }
            PRTF_LOG(NEWS_LOG, "read adr: 0x%x size: %u data:\n", adr, BUF_SIZE);
            for (uint16_t i = 0; i < BUF_SIZE; i++) {
                PRTF_LOG(INFO_LOG, "0x%02x ", test_read_buf[i]);
            }
            PRTF_LOG(NEWS_LOG, "\n");

            IAP_Unlock();
            PRTF_LOG(NEWS_LOG, "erase blk: %u\n", (FS_OST_ADR / BLK_SIZE) + blk,
                     BLK_SIZE);
            IAP_EraseSector((FS_OST_ADR / BLK_SIZE) + blk);
            IAP_Lock();

            IAP_Unlock();
            IAP_WriteCmd(ENABLE);
            uint16_t wrt_size = IAP_ProgramByteArray(adr, test_write_buf, BUF_SIZE);
            IAP_Lock();

            if (wrt_size < BUF_SIZE) {
                PRTF_OS_LOG(ERRO_LOG, "write size: %u < %u!\n", wrt_size, BUF_SIZE);
            }
            PRTF_LOG(NEWS_LOG, "write adr: 0x%x size: %u data:\n", adr, BUF_SIZE);
            for (uint16_t i = 0; i < BUF_SIZE; i++) {
                PRTF_LOG(INFO_LOG, "0x%02x ", test_write_buf[i]);
            }
            PRTF_LOG(NEWS_LOG, "\n");
        }
        rt_thread_delay(5000);
    }
}
#endif