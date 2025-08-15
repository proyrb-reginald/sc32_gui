#include "fs_cfg.h"
#include "fs_if.h"
#include INC_LOG
#include INC_RTOS

lfs_error fs_get_boot_cnt(lfs_t * const lfs) {
    if (lfs == NULL) {
        return LFS_ERR_INVAL;
    }

    uint32_t   boot_cnt = 0;
    lfs_file_t file;
    lfs_error  err;

    err = lfs_file_open(lfs, &file, BOOT_FILE_P, LFS_O_RDONLY);
    if (err < 0) {
        PRTF_OS_LOG(ERRO_LOG, "open " BOOT_FILE_P " fail with %d!\n", err);
        return err;
    }

    lfs_ssize_t bytes_read = lfs_file_read(lfs, &file, &boot_cnt, sizeof(boot_cnt));
    if (bytes_read != sizeof(boot_cnt)) {
        err = lfs_file_close(lfs, &file);
        if (err < 0) {
            PRTF_OS_LOG(ERRO_LOG, "close " BOOT_FILE_P " fail with %d!\n", err);
            return err;
        }
        return -1;
    }

    err = lfs_file_close(lfs, &file);
    if (err < 0) {
        PRTF_OS_LOG(ERRO_LOG, "close " BOOT_FILE_P " fail with %d!\n", err);
        return err;
    }

    return (int)boot_cnt;
}

lfs_error fs_update_boot_cnt(lfs_t * const lfs) {
    if (lfs == NULL) {
        return LFS_ERR_INVAL;
    }

    uint32_t   boot_cnt = 0;
    lfs_file_t file;
    lfs_error  err;

    err = lfs_file_open(lfs, &file, BOOT_FILE_P, LFS_O_RDONLY);
    if (err >= 0) {
        lfs_ssize_t bytes_read = lfs_file_read(lfs, &file, &boot_cnt, sizeof(boot_cnt));
        if (bytes_read == sizeof(boot_cnt)) {
            boot_cnt++;
        } else {
            boot_cnt = 1;
        }
        err = lfs_file_close(lfs, &file);
        if (err < 0) {
            PRTF_OS_LOG(ERRO_LOG, "close " BOOT_FILE_P " fail with %d!\n", err);
            return err;
        }
    } else {
        struct lfs_info info;
        err = lfs_stat(lfs, ROOT_SYS_D, &info);
        if (err < 0 || info.type != LFS_TYPE_DIR) {
            err = lfs_mkdir(lfs, ROOT_SYS_D);
            if (err < 0 && err != LFS_ERR_EXIST) {
                PRTF_OS_LOG(ERRO_LOG, "mkdir " ROOT_SYS_D " fail with %d!\n", err);
                return err;
            }
        }
        boot_cnt = 1;
    }

    err =
        lfs_file_open(lfs, &file, BOOT_FILE_P, LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC);
    if (err < 0) {
        PRTF_OS_LOG(ERRO_LOG, "open " BOOT_FILE_P " fail with %d!\n", err);
        return err;
    }

    lfs_ssize_t bytes_written = lfs_file_write(lfs, &file, &boot_cnt, sizeof(boot_cnt));
    if (bytes_written != sizeof(boot_cnt)) {
        err = lfs_file_close(lfs, &file);
        if (err < 0) {
            PRTF_OS_LOG(ERRO_LOG, "close " BOOT_FILE_P " fail with %d!\n", err);
            return err;
        }
        return -1;
    }

    err = lfs_file_close(lfs, &file);
    if (err < 0) {
        PRTF_OS_LOG(ERRO_LOG, "close " BOOT_FILE_P " fail with %d!\n", err);
        return err;
    }

    return LFS_ERR_OK;
}

lfs_error fs_reset_boot_cnt(lfs_t * const lfs) {
    if (lfs == NULL) {
        return LFS_ERR_INVAL;
    }

    lfs_error err;
    err = lfs_remove(lfs, BOOT_FILE_P);
    if (err < 0) {
        PRTF_OS_LOG(ERRO_LOG, "remove " BOOT_FILE_P " fail with %d!\n", err);
        return err;
    }

    return LFS_ERR_OK;
}

lfs_error fs_ls_dir(lfs_t * const lfs, const char * dir_name) {
    if (lfs == NULL || dir_name == NULL) {
        return LFS_ERR_INVAL;
    }

    lfs_dir_t  dir;
    lfs_info   info;
    lfs_error  err;
    int        file_count = 0;
    int        dir_count  = 0;
    lfs_soff_t total_size = 0;

    err = lfs_dir_open(lfs, &dir, dir_name);
    if (err < 0) {
        PRTF_OS_LOG(ERRO_LOG, "open dir %s fail with %d!\n", dir_name, err);
        return err;
    }

    PRTF_OS_LOG(NEWS_LOG, "list of dir: %s\n", dir_name);
    PRTF_LOG(NEWS_LOG, "%-4s %-8s %s\n", "type", "size", "name");

    while (true) {
        err = lfs_dir_read(lfs, &dir, &info);
        if (err < 0) {
            PRTF_OS_LOG(ERRO_LOG, "read dir fail with %d!\n", err);
            err = lfs_dir_close(lfs, &dir);
            if (err < 0) {
                PRTF_OS_LOG(ERRO_LOG, "close dir fail with %d!\n", err);
                return err;
            }
            return err;
        }

        if (err == 0) {
            break;
        }

        if (strcmp(info.name, ".") == 0 || strcmp(info.name, "..") == 0) {
            continue;
        }

        if (info.type == LFS_TYPE_REG) {
            PRTF_LOG(NEWS_LOG, "%-4c %-8u %s\n", 'f', (int)info.size, info.name);
            file_count++;
            total_size += info.size;
        } else if (info.type == LFS_TYPE_DIR) {
            PRTF_LOG(NEWS_LOG, "%-4c %-8c %s/\n", 'd', '-', info.name);
            dir_count++;
        } else {
            PRTF_LOG(NEWS_LOG, "%-4c %-8c %s\n", 'o', '-', info.name);
        }
    }

    err = lfs_dir_close(lfs, &dir);
    if (err < 0) {
        PRTF_OS_LOG(ERRO_LOG, "close dir fail with %d!\n", err);
        return err;
    }

    return LFS_ERR_OK;
}

/* lfs_error
fs_write_res(lfs_t * const lfs, const lv_image_dsc_t * const res, const char * name) {
    if (lfs == NULL || name == NULL) {
        return LFS_ERR_INVAL;
    }

    if (res->data == NULL || res->data_size == 0) {
        return LFS_ERR_INVAL;
    }

    lfs_file_t file;
    lfs_error  err;

    err = lfs_file_open(lfs, &file, name, LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC);
    if (err != LFS_ERR_OK) {
        struct lfs_info info;
        err = lfs_stat(lfs, ROOT_RES_D, &info);
        if (err < 0 || info.type != LFS_TYPE_DIR) {
            err = lfs_mkdir(lfs, ROOT_RES_D);
            if (err < 0 && err != LFS_ERR_EXIST) {
                PRTF_OS_LOG(ERRO_LOG, "mkdir " ROOT_RES_D " fail with %d!\n", err);
                return err;
            }
        }

        err = lfs_file_open(lfs, &file, name, LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC);
        if (err < 0) {
            PRTF_OS_LOG(ERRO_LOG, "open %s fail with %d!\n", name, err);
            return err;
        }
    }

    lfs_ssize_t written = lfs_file_write(lfs, &file, res->data, res->data_size);
    if (written < 0) {
        PRTF_OS_LOG(ERRO_LOG, "write %s fail with %d!\n", name, err);
        err = lfs_file_close(lfs, &file);
        if (err < 0) {
            PRTF_OS_LOG(ERRO_LOG, "close %s fail with %d!\n", name, err);
            return err;
        }
        return (lfs_error)written;
    }

    if (written != (lfs_ssize_t)res->data_size) {
        PRTF_OS_LOG(ERRO_LOG, "write %s fail with %d!\n", name, written);
        err = lfs_file_close(lfs, &file);
        if (err < 0) {
            PRTF_OS_LOG(ERRO_LOG, "close %s fail with %d!\n", name, err);
            return err;
        }
        return LFS_ERR_IO;
    }

    err = lfs_file_close(lfs, &file);
    if (err < 0) {
        PRTF_OS_LOG(ERRO_LOG, "close %s fail with %d!\n", name, err);
        return err;
    }

    return LFS_ERR_OK;
} */