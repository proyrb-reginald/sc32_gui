#ifndef __LOG_H__
#define __LOG_H__

/**
 * @brief 这是日志模块。
 * @details
 * @file log.h
 * @author proyrb
 * @date 2025/8/13
 * @note
 */

/********** 导入需要的头文件 **********/

#include <rtthread.h>

/********** 配置模块行为 **********/

// 日志等级
#define TRAC_LOG 0
#define INFO_LOG 1
#define NEWS_LOG 2
#define WARN_LOG 3
#define ERRO_LOG 4
#define NONE_LOG 5

// 日志输出阈值：输出大于等于该级别的日志
#define LOG_LEV INFO_LOG

// 格式化打印函数接口
#ifndef LOG_IF_PRTF
#    error "Please provide a interface to print!"
#endif

// 获取系统时刻函数接口
#ifndef LOG_IF_GET_TICK
#    error "Please provide a interface to get system tick!"
#endif

// 打印无附加信息的日志
#define PRTF_LOG(lev, fmt, ...)                                                          \
    if (lev >= LOG_LEV) {                                                                \
        LOG_IF_PRTF(fmt, ##__VA_ARGS__);                                                 \
    }

// 打印附带系统信息的日志
#define PRTF_OS_LOG(lev, fmt, ...)                                                       \
    if (lev >= LOG_LEV) {                                                                \
        LOG_IF_PRTF("[%u:%s:%s:%d] " fmt, LOG_IF_GET_TICK(), rt_thread_self()->name,     \
                    __FUNCTION__, __LINE__, ##__VA_ARGS__);                              \
    }

#endif
