/**
 * @file drvs_if.h
 * @author proyrb (proyrb@yeah.net)
 * @brief 驱动程序接口。
 * @version 0.1
 * @date 2025-08-14
 * @copyright Copyright (c) 2025
 * @note
 */

#ifndef __DRVS_IF_H__
#define __DRVS_IF_H__

/****************************** 导入头文件 ******************************/

#include "drvs_cfg.h"

#ifdef USE_EP15301T
#    include "ep15301t/ep15301t_if.h"
#endif

#ifdef USE_W25Q64
#    include "w25q64/w25q64_if.h"
#endif

#endif