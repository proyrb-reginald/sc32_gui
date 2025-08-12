#ifndef W25Q64_H
#define W25Q64_H

/**
 * @brief 这是w25q64驱动模块。
 * @details 先初始化模块。
 * @file w25q64.h
 * @author proyrb
 * @date 2025/7/28
 * @note
 */

/********** 导入需要的头文件 **********/

#include "sc32_conf.h"
#include "FreeRTOS.h"
#include "semphr.h"

/********** 选择 gpio 引脚 **********/

#ifdef W25Q64_C
#    define CHIP_GPIO_GRP GPIOB
#    define CHIP_GPIO_PIN GPIO_Pin_13
#    define GPIO_WRITE(grp, pin, val) GPIO_WriteBit(grp, pin, val)
#endif  // W25Q64_C

/********** 选择 qspi 设备 **********/

#ifdef W25Q64_C
#    define USE_QSPI QSPI0
#endif  // W25Q64_C

/********** 选择 dma 设备 **********/

#ifdef W25Q64_C
#    define USE_DMA_TX DMA1
#    define USE_DMA_RX DMA2
#endif  // W25Q64_C

/********** 配置设备信息 **********/

// 以MSB方式接收后再以小端格式存储的设备唯一ID
#define W25Q64_Unique_ID 0x353E730B973C60DF

/********** 模块行为 **********/

#ifdef W25Q64_C
// 是否整片擦除
#    define CHIP_ERASE 0
#endif  // W25Q64_C

/********** 常用指令 **********/

typedef enum {
    /********** 初始化指令 **********/

    Reset = 0x6699,

    /********** 读取指令 **********/

    ReadUniqueID = 0x4B,
    ReadSR1      = 0x05,
    ReadSR2      = 0x35,
    ReadSR3      = 0x15,
    ReadDataSPI  = 0x03,
    ReadDataQSPI = 0x6B,  // 4线读取和DMA搬运有问题

    /********** 擦写指令 **********/

    WriteDisable = 0x04,
    WriteEnable  = 0x06,
    Erase4KB     = 0x20,
    Erase32KB    = 0x52,
    Erase64KB    = 0xD8,
    EraseChip    = 0xC7,
    ProgramSPI   = 0x02,
    ProgramQSPI  = 0x32,
} w25q64_cmd;

/********** 指令附带参数 **********/

typedef struct {
    const uint8_t *    adr;   // 3字节的地址
    volatile uint8_t * data;  // 变字节数据
    uint32_t           size;  // 数据长度
} w25q64_arg;

/********** 导出的函数 **********/

/**
 * @brief 初始化w25q64模块。
 * @param
 * @retval
 * @warning 必须先初始化模块后才能进行后续操作。
 * @note
 */
BaseType_t w25q64_init(void);

/**
 * @brief dma中断处理。
 * @param
 * @retval
 * @warning 禁止在非中断中调用。
 * @note
 */
void w25q64_dma_irq(void);

/**
 * @brief 发送控制指令与附带的可选参数。
 * @param cmd 指令：可以使用不在w25q64_cmd范围的值，该值会被自动作为8位命令发送。
 * @param arg 可选参数：不追加参数时请使用NULL填充；
 * 非NULL时，如果cmd值不在给定范围内，则不会发送该字节流。
 * @retval
 * @warning 线程安全；同步的。
 * @note
 */
void w25q64_ctl(const w25q64_cmd cmd, const w25q64_arg * const arg);

#endif  // W25Q64_H
