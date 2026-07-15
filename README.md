# Electronic Systems and Network Development Practice

## 中文说明

本仓库用于保存短学期综合实践课程中的项目文件，主要包含两部分内容：

- 计算机网络课程设计
- STM32 嵌入式系统设计

### 计算机网络课程设计

计算机网络部分主要存放 Packet Tracer 网络拓扑、实验配置和课程设计相关文件。相关内容位于：

```text
PTProjects/
  260706/
  260707/
  260708/
```

### STM32 设计

STM32 部分实现了一个基于 STM32F103 的波形发生器。该项目在尽量不改变原有硬件接线的基础上，通过软件增加了波形选择、频率调节、扫频、参数保存和 LCD 显示等功能。

主要功能：

- 输出方波、锯齿波和正弦波
- 频率范围：500 Hz 到 2000 Hz
- 支持 10 Hz、100 Hz、500 Hz 三档频率步进
- 支持输出开启和关闭
- 方波支持占空比调节
- 正弦波和锯齿波支持幅度调节
- 支持 500 Hz 到 2000 Hz 自动扫频
- LCD1602 显示当前波形、频率、输出状态、步进、占空比或幅度
- 使用 Flash 保存参数，掉电后可恢复上次设置

按键功能：

| 按键 | 功能 |
| --- | --- |
| KEY1 短按 | 切换波形 |
| KEY1 长按 | 开启或关闭输出 |
| KEY2 | 降低频率 |
| KEY3 | 提高频率 |
| KEY2 + KEY3 | 切换频率步进 |
| 方波模式下 KEY4 / KEY5 | 调节占空比 |
| 正弦波或锯齿波模式下 KEY4 / KEY5 | 调节幅度 |
| KEY4 + KEY5 | 开启或关闭扫频 |

STM32 工程目录：

```text
STM32/
  example/
    User/
    Library/
    Start/
    Wave_Create/
    system/
    project1.uvprojx
```

STM32 工程使用 Keil uVision 构建，生成的下载文件为：

```text
STM32/example/Objects/project1.hex
```

## English Description

This repository stores project files for the short-semester comprehensive practice course. It contains two main parts:

- Computer network course design
- STM32 embedded system design

### Computer Network Course Design

The computer network part contains Packet Tracer network topologies, experiment configurations, and related course design files. These files are located in:

```text
PTProjects/
  260706/
  260707/
  260708/
```

### STM32 Design

The STM32 part implements an STM32F103-based signal generator. Without changing the original hardware pin connections as much as possible, the project adds waveform selection, frequency adjustment, sweep mode, parameter persistence, and LCD display through software.

Main features:

- Square wave, sawtooth wave, and sine wave output
- Frequency range from 500 Hz to 2000 Hz
- Frequency step options: 10 Hz, 100 Hz, and 500 Hz
- Output enable/disable control
- Duty-cycle adjustment for square wave
- Amplitude adjustment for sine wave and sawtooth wave
- Automatic sweep from 500 Hz to 2000 Hz
- LCD1602 display for waveform, frequency, output state, step, duty cycle, and amplitude
- Flash parameter persistence after power-off

Button controls:

| Button | Function |
| --- | --- |
| KEY1 short press | Switch waveform |
| KEY1 long press | Toggle output on/off |
| KEY2 | Decrease frequency |
| KEY3 | Increase frequency |
| KEY2 + KEY3 | Switch frequency step |
| KEY4 / KEY5 in square-wave mode | Adjust duty cycle |
| KEY4 / KEY5 in sine/sawtooth mode | Adjust amplitude |
| KEY4 + KEY5 | Toggle sweep mode |

STM32 project directory:

```text
STM32/
  example/
    User/
    Library/
    Start/
    Wave_Create/
    system/
    project1.uvprojx
```

The STM32 project is built with Keil uVision. The generated firmware file is:

```text
STM32/example/Objects/project1.hex
```
