# 概述
A wrapper which can make codes developed by uCOS-III APIs can directly run on RT-Thread

这是一个针对RT-Thread操作系统的uCOS-III兼容层，可以让基于uCOS-III的应用层程序做最小的修改，使项目快速从uCOS-III迁移到RT-Thread。

本文件内的兼容层为uCOS-III3.03版本向RT-Thread Nano-3.1.3兼容。由于uCOS-III支持<u>8、16、32位</u>CPU，而RT-Thread支持<u>32、64位CPU</u>，**因此本兼容层仅能对基于32位CPU的已有工程进行兼容**。


## 版本详细信息
uC/OS-III        3.03.00 </br>
uC/CPU          1.30.00 </br>
uC/LIB            1.37.02 </br>
RTT nano       3.1.3  </br>


## 仿真工程
本仿真工程是基于*STM32F103RB*平台。

Keil工程路径：<u>RT-Thread-wrapper-of-uCOS-III\rt_thread_3.1.3-ucosiii_3.03-wrapper\rt-thread-3.1.3\bsp\stm32f103-msh-628\Project.uvprojx</u>

需要提前安装好RT-Thread Nano-3.1.3 Keil支持包：https://www.rt-thread.org/download/mdk/RealThread.RT-Thread.3.1.3.pack

