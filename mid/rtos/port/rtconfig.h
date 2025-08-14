/**
 * @file rtconfig.h
 * @author proyrb (proyrb@yeah.net)
 * @brief RTThread配置头文件
 * @version 0.1
 * @date 2025-08-14
 * @copyright Copyright (c) 2025
 * @note
 */

#ifndef __RTTHREAD_CFG_H__
#define __RTTHREAD_CFG_H__

/**
 * @brief 配置线程优先级取值范围。
 * @warning
 * @note 取值范围：8-256
 */
#define RT_THREAD_PRIORITY_MAX 16

/**
 * @brief 配置系统在1s内的节拍数。
 * @warning 只是用来告诉RT-Thread节拍的周期。
 * @note 这个数值请根据实际的系统滴答参数来设置。
 */
#define RT_TICK_PER_SECOND 1000

/**
 * @brief 配置系统的内存对齐地址。
 * @warning 默认不修改，除非打算牺牲性能换取更多空余内存。
 * @note 默认采用4字节对齐。
 */
#define RT_ALIGN_SIZE 4

/**
 * @brief 定义了各种内核对象（如线程、信号量、消息队列等）名称的最大字符数。
 * @warning 包括终止符。
 * @note 默认最大为8个字节。
 */
#define RT_NAME_MAX 8

/**
 * @brief 启用组件初始化操作。
 * @warning 一般不变。
 * @note
 */
#define RT_USING_COMPONENTS_INIT

/**
 * @brief 启用main线程。
 * @warning 一般不变。
 * @note
 */
#define RT_USING_USER_MAIN

/**
 * @brief 配置main线程的栈大小。
 * @warning 以字节为单位，不能太小。
 * @note 取值范围为：1-4086
 */
#define RT_MAIN_THREAD_STACK_SIZE (36 * 64)

/**
 * @brief 配置main线程的优先级。
 * @warning
 * @note
 */
#define RT_MAIN_THREAD_PRIORITY 1

/**
 * @brief 启用内核调试。
 * @warning
 * @note 对于发行版可注释掉。
 */
#define RT_DEBUG

/**
 * @brief 启用组件初始化过程的调试。
 * @warning
 * @note 对于发行版可注释掉。
 */
// #define RT_DEBUG_INIT 1

/**
 * @brief 启用线程栈溢出检查。
 * @warning
 * @note
 */
#define RT_USING_OVERFLOW_CHECK

/**
 * @brief 启用钩子函数。
 * @warning
 * @note
 */
// #define RT_USING_HOOK

/**
 * @brief 启用空闲任务的钩子函数。
 * @warning
 * @note
 */
// #define RT_USING_IDLE_HOOK

/**
 * @brief 是否启用用户软件定时器。
 * @warning
 * @note
 */
#define RT_USING_TIMER_SOFT 0
#if RT_USING_TIMER_SOFT == 0
#    undef RT_USING_TIMER_SOFT
#endif

/**
 * @brief 配置用户定时器的线程优先级。
 * @warning 请根据定时精确性与实际线程个数进行调整。
 * @note 默认为4。
 */
#define RT_TIMER_THREAD_PRIO (RT_THREAD_PRIORITY_MAX - 3)

/**
 * @brief 配置用户定时器的线程栈大小。
 * @warning 一般不变。
 * @note 默认为256。
 */
#define RT_TIMER_THREAD_STACK_SIZE (2 * 64)

/**
 * @brief 启用信号量。
 * @warning
 * @note 解决"谁先谁后"的同步问题。
 */
#define RT_USING_SEMAPHORE

/**
 * @brief 启用互斥量。
 * @warning
 * @note
 */
#define RT_USING_MUTEX

/**
 * @brief 启用事件。
 * @warning
 * @note
 */
// #define RT_USING_EVENT

/**
 * @brief 启用信号。
 * @warning
 * @note 解决"发生了什么"的异步通知问题。
 */
// #define RT_USING_SIGNALS

/**
 * @brief 启用邮箱。
 * @warning
 * @note 可发送大小可变的数据。
 */
// #define RT_USING_MAILBOX

/**
 * @brief 启用消息队列。
 * @warning
 * @note 只能发送固定大小的数据。
 */
// #define RT_USING_MESSAGEQUEUE

/**
 * @brief 启用内存池管理。
 * @warning
 * @note 提供固定大小块的高效内存池。
 */
#define RT_USING_MEMPOOL

/**
 * @brief 启用动态内存分配。
 * @warning
 * @note 基础内存管理。
 */
#define RT_USING_HEAP

/**
 * @brief 启用小块内存分配优化。
 * @warning
 * @note
 */
#define RT_USING_SMALL_MEM

/**
 * @brief 启用小内存作为堆。
 * @warning
 * @note 提供高性能的内存分配。
 */
#define RT_USING_SMALL_MEM_AS_HEAP

/**
 * @brief 启用极简内存配置。
 * @warning
 * @note 移除不必要的内存管理功能，适用于资源极度受限的嵌入式系统。
 */
// #define RT_USING_TINY_SIZE

/**
 * @brief 启用控制台输出功能。
 * @warning
 * @note 提供系统调试和信息显示的能力。
 */
#define RT_USING_CONSOLE

/**
 * @brief 配置控制台线程栈大小。
 * @warning 一般不变
 * @note 默认256字节。
 */
#define RT_CONSOLEBUF_SIZE (4 * 64)

/**
 * @brief 启用设备驱动框架。
 * @warning
 * @note 提供统一的设备抽象层和设备管理机制。
 */
// #define RT_USING_DEVICE

/**
 * @brief 启用FinSH。
 * @warning
 * @note
 */
// #define RT_USING_FINSH

/**
 * @brief 启用MSH模式。
 * @warning
 * @note
 */
#define FINSH_USING_MSH

/**
 * @brief 只启用MSH模式。
 * @warning
 * @note
 */
#define FINSH_USING_MSH_ONLY

/**
 * @brief 配置FinSH的优先级。
 * @warning
 * @note
 */
#define FINSH_THREAD_PRIORITY (RT_THREAD_PRIORITY_MAX - 2)

/**
 * @brief 配置FinSH的栈大小。
 * @warning
 * @note
 */
#define FINSH_THREAD_STACK_SIZE 1024

/**
 * @brief 把符号表编译进固件。
 * @warning 一般不变。
 * @note
 */
#define FINSH_USING_SYMTAB

/**
 * @brief 把每个命令的“帮助说明”一起编译进固件。
 * @warning
 * @note
 */
#define FINSH_USING_DESCRIPTION

#endif
