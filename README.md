# sc32_gui

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Version](https://img.shields.io/badge/version-1.0.0-green.svg)

## 一、简介

这是一个用于**快速构建**基于SC32F1系列单片机的，集成图形界面、文件系统、实时操作系统的开发框架。

## 二、特性

1. 目前支持赛元微的**SC32F1**系列。
2. 适配多种模块驱动：

    |板载外设驱动芯片|是否适配|通讯协议|主要用途|
    |-|-|-|-|
    |W25Q128|☑️|SPI/QSPI|提供灵活可替换的外部存储空间|
    |ST77916|☑️|SPI|驱动并显示图形界面|
    |...|❎|...|...|

3. 集成多种开源中间件：

    |系统中间件|是否适配|版本|主要用途|
    |-|-|-|-|
    |RT-Thread-Nano|☑️|V4.1.1|拆解任务以简化业务逻辑、提高CPU利用率|
    |FatFS|☑️|R0.16|提供更加便捷且高效的方式来操作片内与外部储存空间|
    |LVGL|☑️|V9.3/V8.4|提供以组件化的方式构建图形界面的手段|
    |lwIP|❎|||

4. 面向开发者提供灵活、便捷、统一的多任务开发框架。
5. 易于移植到所有Cortex-M系列。

## 三、如何开始

请参考本项目的[Wiki](https://github.com/proyrb-reginald/sc32_gui/wiki)页面。
