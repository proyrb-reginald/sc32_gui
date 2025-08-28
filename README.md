# SC32 GUI Development Framework

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Version](https://img.shields.io/badge/version-1.0.0-green.svg)

## 一、简介

SC32 GUI 是一个专为 SC32F1 系列单片机设计的集成开发框架，集成了图形界面、文件系统和实时操作系统，旨在帮助开发者快速构建嵌入式图形界面应用程序。

该框架基于以下技术栈：
- **RT-Thread Nano** v4.1.1 - 实时操作系统内核
- **LVGL** v8.4 - 轻量级图形界面库
- **FatFS** R0.16 - 嵌入式文件系统

## 二、特性

### 2.1 硬件支持

目前主要支持赛元微电子的 SC32F1 系列 Cortex-M0+ 内核单片机。

### 2.2 驱动支持

| 外设 | 状态 | 通讯接口 | 描述 |
|------|------|----------|------|
| W25Q128 | ☑️ | SPI/QSPI | 外部 Flash 存储 |
| GC9A01 | ☑️ | SPI | 1.28寸圆形 LCD 显示屏 |
| ROM | ☑️ | 内部 | 片上 ROM 文件系统 |
| SD Card | ☑️ | SPI | SD 卡存储支持 |
| EP15301T | ☑️ | I2C | 电容式触摸屏控制器 |

### 2.3 中间件集成

| 中间件 | 版本 | 状态 | 描述 |
|--------|------|------|------|
| RT-Thread Nano | V4.1.1 | ☑️ | 实时操作系统内核 |
| FatFS | R0.16 | ☑️ | 嵌入式文件系统 |
| LVGL | V8.4 | ☑️ | 轻量级嵌入式图形库 |
| LittleFS | latest | ☑️ | 日志结构文件系统 |

### 2.4 开发特性

- 统一的驱动接口设计
- 模块化架构，易于扩展
- 支持多种文件系统（内部 ROM、外部 Flash、SD 卡）
- 图形界面支持（基于 SquareLine Studio 设计）
- 多任务实时操作系统支持
- 易于移植到其他 Cortex-M 系列单片机

## 三、系统架构

```
+-----------------------------------------------------+
|                    Application Layer                |
|  GUI Applications  |  File System  |  User Tasks    |
+-----------------------------------------------------+
|                 RT-Thread Nano OS                   |
+-----------------------------------------------------+
|        Driver Interface Abstraction Layer           |
|  Display  |  Storage  |  Touch  |  Communication   |
+-----------------------------------------------------+
|           SC32F1 Hardware Abstraction Layer         |
|     GPIO    |    SPI    |    I2C    |    UART       |
+-----------------------------------------------------+
|                 SC32F1 MCU Hardware                 |
+-----------------------------------------------------+
```

## 四、快速开始

### 4.1 环境准备

1. 安装 IDE（支持 Keil MDK、EIDE 等）
2. 安装编译工具链（ARM GCC 或 ARMCC）
3. 克隆项目源码

### 4.2 硬件连接

根据所使用的开发板和外设模块，正确连接以下设备：
- 显示屏（如 GC9A01）
- 外部存储（如 W25Q Flash）
- 触摸屏（如 EP15301T）

### 4.3 编译和下载

1. 打开项目工程文件
2. 根据硬件配置修改 [drvs_cfg.h](file://c:\Users\Administrator\Documents\SOC\sc32_gui\drvs\drvs_cfg.h) 文件
3. 编译项目
4. 下载固件到目标硬件

### 4.4 示例代码

项目默认创建了一个 GUI 测试任务：

```c
__attribute__((noreturn)) void gui_test(void * thread_args) {
    lv_init();
    lv_port_disp_init();
    ui_init();

    PRTF_OS_LOG(NEWS_LOG, "gui start!\n");

    while (1) {
        lv_timer_handler();
        rt_thread_delay(3);
    }
}
```

## 五、目录结构

```
sc32_gui/
├── drvs/              # 设备驱动实现
├── hal/               # 硬件抽象层
├── inc/               # 公共头文件
├── mid/               # 中间件组件
├── src/               # 应用源码
│   ├── gui/           # 图形界面相关
│   ├── fs/            # 文件系统接口
│   └── main.c         # 主程序入口
└── script/            # 构建脚本
```

## 六、配置说明

主要配置文件：
- `inc/main.h` - 系统核心配置
- `drvs/drvs_cfg.h` - 驱动使能配置
- `mid/rtos/port/rtconfig.h` - RT-Thread 配置
- `mid/lvgl/lv_conf.h` - LVGL 图形库配置

## 七、开发工具

- **SquareLine Studio** - LVGL 官方 UI 设计工具
- **VS Code** - 推荐的代码编辑器
- **串口调试工具** - 日志输出查看

## 八、如何贡献

欢迎提交 Issue 和 Pull Request 来改进这个项目。

## 九、许可证

本项目采用 MIT 许可证，详情请查看 [LICENSE](LICENSE) 文件。

## 十、联系方式

如有问题，请联系：proyrb@yeah.net
