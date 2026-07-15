# Electronic Systems and Network Development Practice

## 中文说明

本仓库用于保存《电子系统及网络开发实践》短学期综合实践课程中的项目文件，主要包含两部分内容：

- 基于 Cisco Packet Tracer 的计算机网络课程设计
- 基于 STM32F103 的嵌入式波形发生器设计

---

## 1. 计算机网络课程设计

### 1.1 项目简介

本项目是基于 Cisco Packet Tracer 实现的小型企业综合网络仿真项目。项目包含企业内网、服务器区、RIP 路由区域、OSPF 路由区域以及模拟公网环境，实现了 VLAN 划分、三层交换、动态路由、路由重分发、DHCP、NAT/PAT、DNS、Web、FTP、Email、Telnet、ACL 和无线接入等功能。

项目重点训练从网络需求分析、拓扑规划、设备配置，到故障排查和业务验收的完整工程流程。

### 1.2 网络区域划分

- 企业用户接入区
- RIP 动态路由区域
- OSPF 动态路由区域
- 企业服务器区
- 企业边界出口
- 模拟 ISP 与公网服务区
- 无线局域网接入区

### 1.3 主要设备

| 设备 | 主要作用 |
| --- | --- |
| 三层交换机 MLS | VLAN 网关、跨 VLAN 路由、RIP/OSPF 边界及路由重分发 |
| 边界路由器 R-BORDER / R-OUT | 企业内外网出口、默认路由、NAT/PAT、静态 NAT |
| OSPF 路由器 | 运行 OSPF，连接用户网络及服务器区域 |
| RIP 路由器 | 运行 RIPv2，连接分支用户网络 |
| ISP 路由器 | 模拟公网路由环境 |
| 二层交换机 | 终端接入、VLAN 划分和 Trunk 转发 |
| Server-PT | 提供 DHCP、DNS、HTTP、FTP 和 Email 服务 |
| Access Point | 提供无线终端接入 |
| PC / Laptop | 模拟企业用户和公网用户 |

### 1.4 已实现功能

- VLAN 划分、Access 端口、Trunk 链路和三层交换机 SVI 网关
- RIPv2、OSPF Area 0，以及 RIP 与 OSPF 双向路由重分发
- DHCP 地址池和 DHCP Relay，为不同 VLAN 和用户网段自动分配地址
- NAT/PAT，实现内网用户共享公网地址访问外网
- 静态 NAT，实现公网用户访问企业内部 Web 服务器
- DNS、Web、FTP 和 Email 网络服务
- Telnet 远程管理，包括 VTY、Enable 密码和本地用户认证
- 扩展 ACL 访问控制，用于限制指定终端的网络访问行为
- 无线 AP 接入，支持无线终端通过 DHCP 获取地址并跨网段通信

### 1.5 网络设计思路

项目按照以下顺序完成配置和测试：

1. 绘制拓扑并完成设备连接
2. 规划 IP 地址、VLAN 和设备接口
3. 配置设备接口 IP 并验证直连通信
4. 配置 VLAN、Access、Trunk 和三层网关
5. 配置 RIP 和 OSPF
6. 配置 RIP 与 OSPF 路由重分发
7. 验证企业内部全网互通
8. 配置默认路由及公网动态路由
9. 配置 DHCP 和 DHCP Relay
10. 配置 NAT/PAT 与静态 NAT
11. 部署 DNS、Web、FTP 和 Email 服务
12. 配置 Telnet 和 ACL
13. 完成整体连通性测试和故障排查

采用分阶段配置和测试的方法，可以避免在基础路由尚未连通时，过早排查 DNS、Web 或 NAT 等上层业务问题。

### 1.6 文件位置

```text
PTProjects/
  260706/
  260707/
  260708/
```

---

## 2. STM32 嵌入式设计

### 2.1 项目简介

STM32 部分实现了一个基于 STM32F103 的波形发生器。该项目在尽量不改变原有硬件接线的基础上，通过软件增加了波形选择、频率调节、扫频、参数保存和 LCD 显示等功能。

### 2.2 主要功能

- 输出方波、锯齿波和正弦波
- 频率范围：500 Hz 到 2000 Hz
- 支持 10 Hz、100 Hz、500 Hz 三档频率步进
- 支持输出开启和关闭
- 方波支持占空比调节
- 正弦波和锯齿波支持幅度调节
- 支持 500 Hz 到 2000 Hz 自动扫频
- LCD1602 显示当前波形、频率、输出状态、步进、占空比或幅度
- 使用 Flash 保存参数，掉电后可恢复上次设置

### 2.3 按键功能

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

### 2.4 工程目录

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

---

## English Description

This repository stores project files for the short-semester comprehensive practice course **Electronic Systems and Network Development Practice**. It contains two main parts:

- Computer network course design based on Cisco Packet Tracer
- STM32F103 embedded signal generator design

---

## 1. Computer Network Course Design

### 1.1 Project Overview

This project is a small enterprise network simulation built with Cisco Packet Tracer. It includes an enterprise intranet, server area, RIP routing area, OSPF routing area, and simulated public network environment. The project implements VLAN segmentation, Layer 3 switching, dynamic routing, route redistribution, DHCP, NAT/PAT, DNS, Web, FTP, Email, Telnet, ACL, and wireless access.

The project focuses on the complete engineering workflow from network requirement analysis, topology planning, and device configuration to troubleshooting and service validation.

### 1.2 Network Areas

- Enterprise user access area
- RIP dynamic routing area
- OSPF dynamic routing area
- Enterprise server area
- Enterprise border and Internet exit
- Simulated ISP and public service area
- Wireless LAN access area

### 1.3 Main Devices

| Device | Role |
| --- | --- |
| Multilayer switch MLS | VLAN gateway, inter-VLAN routing, RIP/OSPF boundary, route redistribution |
| Border router R-BORDER / R-OUT | Enterprise Internet exit, default route, NAT/PAT, static NAT |
| OSPF routers | Run OSPF and connect user networks and server areas |
| RIP routers | Run RIPv2 and connect branch user networks |
| ISP router | Simulate the public network routing environment |
| Layer 2 switches | Terminal access, VLAN segmentation, and Trunk forwarding |
| Server-PT | Provide DHCP, DNS, HTTP, FTP, and Email services |
| Access Point | Provide wireless terminal access |
| PC / Laptop | Simulate enterprise users and public users |

### 1.4 Implemented Features

- VLAN segmentation, Access ports, Trunk links, and SVI gateways on the multilayer switch
- RIPv2, OSPF Area 0, and bidirectional route redistribution between RIP and OSPF
- DHCP address pools and DHCP Relay for different VLANs and user networks
- NAT/PAT for internal users to access the public network through shared public addresses
- Static NAT for public users to access the internal enterprise Web server
- DNS, Web, FTP, and Email services
- Telnet remote management with VTY, Enable password, and local user authentication
- Extended ACL access control for restricting selected terminal traffic
- Wireless AP access with DHCP address assignment and inter-network communication

### 1.5 Design Workflow

The network configuration and verification were completed in the following order:

1. Draw the topology and connect devices
2. Plan IP addresses, VLANs, and device interfaces
3. Configure interface IP addresses and verify directly connected communication
4. Configure VLANs, Access ports, Trunk links, and Layer 3 gateways
5. Configure RIP and OSPF
6. Configure route redistribution between RIP and OSPF
7. Verify full connectivity inside the enterprise network
8. Configure default routes and public network routing
9. Configure DHCP and DHCP Relay
10. Configure NAT/PAT and static NAT
11. Deploy DNS, Web, FTP, and Email services
12. Configure Telnet and ACL
13. Complete overall connectivity testing and troubleshooting

### 1.6 File Location

```text
PTProjects/
  260706/
  260707/
  260708/
```

---

## 2. STM32 Embedded Design

### 2.1 Project Overview

The STM32 part implements an STM32F103-based signal generator. Without changing the original hardware pin connections as much as possible, the project adds waveform selection, frequency adjustment, sweep mode, parameter persistence, and LCD display through software.

### 2.2 Main Features

- Square wave, sawtooth wave, and sine wave output
- Frequency range from 500 Hz to 2000 Hz
- Frequency step options: 10 Hz, 100 Hz, and 500 Hz
- Output enable/disable control
- Duty-cycle adjustment for square wave
- Amplitude adjustment for sine wave and sawtooth wave
- Automatic sweep from 500 Hz to 2000 Hz
- LCD1602 display for waveform, frequency, output state, step, duty cycle, and amplitude
- Flash parameter persistence after power-off

### 2.3 Button Controls

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

### 2.4 Project Directory

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
