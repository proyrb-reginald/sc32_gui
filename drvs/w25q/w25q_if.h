/**
 * @file w25q_if.h
 * @author proyrb (proyrb@yeah.net)
 * @brief 外部闪存接口。
 * @version 0.1
 * @date 2025-08-18
 * @copyright Copyright (c) 2025
 * @note
 */

#ifndef __W25Q_IF_H__
#define __W25Q_IF_H__

/****************************** 导入头文件 ******************************/

#include <sc32_conf.h>

/****************************** 类型定义 ******************************/

/* 主要常用指令 */
typedef enum {
    Reset        = 0x6699,
    ReadUniqueID = 0x4B,
    ReadSR1      = 0x05,
    ReadSR2      = 0x35,
    ReadSR3      = 0x15,
    ReadDataSPI  = 0x03,
    ReadDataQSPI = 0xEB,
    WriteDisable = 0x04,
    WriteEnable  = 0x06,
    Erase4KB     = 0x20,
    Erase32KB    = 0x52,
    Erase64KB    = 0xD8,
    EraseChip    = 0xC7,
    ProgramSPI   = 0x02,
    ProgramQSPI  = 0x32,
} w25q_ins_t;

/* 地址结构 */
typedef struct __attribute__((packed)) {
    uint8_t byte3;
    uint8_t byte2;
    uint8_t byte1;
} w25q_adr_t;

/* 指令结构 */
typedef struct __attribute__((packed)) {
    w25q_ins_t         ins;
    w25q_adr_t         adr;
    volatile uint8_t * data;
    uint32_t           size;
} w25q_cmd_t;

/****************************** 函数接口 ******************************/

/**
 * @brief 初始化外部闪存。
 * @return uint8_t
 * @retval 0 初始化完成。
 * @retval !0 初始化失败。
 * @warning
 * @note
 */
uint8_t w25q_init(void);

#endif
