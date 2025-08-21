/**
 * @file rom_if.h
 * @author proyrb (proyrb@yeah.net)
 * @brief 片上存储空间配置。
 * @version 0.1
 * @date 2025-08-21
 * @copyright Copyright (c) 2025
 * @note
 */

#ifndef __ROM_IF_H__
#define __ROM_IF_H__

/****************************** 导入头文件 ******************************/

#include <sc32_conf.h>

/****************************** 函数接口 ******************************/

/**
 * @brief 测试驱动程序功能。
 * @param thread_args 任务参数，暂时未使用。
 * @retval
 * @warning 此函数不返回！
 * @note 请在`rom_cfg.h`中添加`USE_TEST`宏。
 */
extern void rom_test(void * thread_args);

#endif