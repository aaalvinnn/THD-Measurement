# 失真度测量仪

（电赛2021年A题）

## 要求

1. 输入信号峰峰值电压范围：300mV~600mV
2. 输入信号基频：1kHz~100kHz
3. 显示失真度测量值
4. 测量并显示输入信号的一个周期波形
5. 显示输入信号基波与谐波的归一化幅值，只显示到5次谐波
6. 安卓上位机显示

## 总体框架

![image-20230422114557818](https://raw.githubusercontent.com/aaalvinnn/THD-Measurement/main/images/image-20230422114557818.png)

## 硬件思路

通过VGA模块将输入信号幅值放大到3V供单片机AD采样，同时添加直流偏置，并进行峰值检波的负反馈增益控制。

### 模块设计

（VGA原理图）



（直流偏置原理图）



（峰值检波原理图）



![image-20230422114651729](https://raw.githubusercontent.com/aaalvinnn/THD-Measurement/main/images/image-20230422114651729.png)

## 软件思路

### 信号处理

- **采样**：

  ADC+TIM+DMA单通道采样，采样点数2048，单次缓冲，并进行三次移动平滑；

- **FFT**：

  移植的.c/.h；

  需注意频率分辨率的问题：

$$
频率分辨率=\frac{采样频率}{采样点数}
$$

​	同时添加Hann窗函数，并对FFT后的数组进行频谱弥补（累加峰值点周围的3个点作为该频率的幅值）；

- **失真度**：

​	根据题目要求，采用简化的失真度计算公式：
$$
THD_o=\frac{\sqrt{U_{o2}^2+U_{o3}^2+U_{o4}^2+U_{o5}^2}}{U_{o1}}\times100\%
$$
​	基波与谐波下标通过以下方式寻找：

1. 先遍历FFT后的结果数组$*pr$找到最大幅值点（去除直流偏置点$pr[0]$）;
2. 分别\*2、\*3、\*4、\*5得到各谐波下标的大致范围，再以此为中心进行临近极大值搜索得到各谐波下标
3. 累加附近的3个点，将和作为该次谐波频率的幅值；

- **归一化幅值**：

​	各次谐波幅值/基波幅值 的标准归一化。

### 上位机

- HC-05 蓝牙通信模块

​	基于USART的串口协议，易于编程；

- Bluetooth Electronics 安卓上位机

​	基于蓝牙的上位机，控件拖动型的自定义屏幕，易于编程；

​	但无线通信不是特别稳定，偶尔有波形畸形；

- 陶晶驰串口屏

​	（未做...）

## 结果分析

输入基波f=3、4、5kHz，幅值1000mV，5次谐波幅值都为500mV，理论失真度100%

![image-20230422121246383](https://raw.githubusercontent.com/aaalvinnn/THD-Measurement/main/images/image-20230422121246383.png)

![image-20230422121251738](https://raw.githubusercontent.com/aaalvinnn/THD-Measurement/main/images/image-20230422121251738.png)

![image-20230422121302015](https://raw.githubusercontent.com/aaalvinnn/THD-Measurement/main/images/image-20230422121302015.png)

（软硬联调……）



## 问题与反思

软件方面，对于Cubemx配置时钟频率仍不是特别清晰，因为采用定时器触发AD采样，发现Cubemx配置好时钟频率后，怎么修改TIM的分频和溢出周期都没有影响作用到实际的采样频率上，只有整个的修改时钟频率才能降低下来采样频率（因为FFT的原理使得采样频率太高时频率分辨率很低，导致低频输入计算得到的失真度误差较大）。

硬件方面，（……）