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
    ReadUniqueID = 0x4b,
    ReadSR1      = 0x05,
    ReadSR2      = 0x35,
    ReadSR3      = 0x15,
    ReadDataSPI  = 0x03,  // 未实现
    ReadDataQSPI = 0xeb,
    WriteDisable = 0x04,
    WriteEnable  = 0x06,
    Erase4KB     = 0x20,
    Erase32KB    = 0x52,
    Erase64KB    = 0xd8,
    EraseChip    = 0xc7,
    ProgramSPI   = 0x02,  // 未实现
    ProgramQSPI  = 0x32,
} w25q_ins_t;

/* 地址结构 */
typedef struct __attribute__((packed)) {
    uint8_t byte[3];
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

/**
 * @brief 发送指令。
 * @param cmd 具体指令与参数。
 * @retval
 * @warning
 * @note
 */
void w25q_ctl(const w25q_cmd_t * const cmd);

/**
 * @brief 测试驱动程序功能。
 * @param thread_args 任务参数，暂时未使用。
 * @retval
 * @warning 此函数不返回！
 * @note 请在`w25q_cfg.h`中添加`USE_TEST`宏。
 */
void w25q_test(void * thread_args);

/**
 * @brief DMA发送通道中断处理。
 * @retval
 * @warning
 * @note
 */
void w25q_dma_tx_irq(void);

/**
 * @brief DMA接收通道中断处理。
 * @retval
 * @warning
 * @note
 */
void w25q_dma_rx_irq(void);

#endif
