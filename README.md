# ElecCompetitionTask1_2021A
电赛真题训练 2021年A题 失真度测量  
仅远程管理工程的core文件夹部分  
3.21 DMA实现对1kHz正弦波的连续采样  
3.24 配置好了HC-05蓝牙通讯模块以及上位机中安卓软件Bluetoothelectronic的波形显示的传输协议，出现的几个问题及解决方案如下：  
1.注意看bluetoothele…这个软件示波器控件的说明，串口打印格式为："*HX...Y..."其中*为这个软件的串口通信接受标志，H为某一个示波器控件（可以有很多个示波器空间，遵从不同的“Receive Characer”），X后面跟横坐标的值，Y后面跟纵坐标的值  
2.蓝牙模块HC-05应事先设置波特率与MCU的USART波特率相同（这个项目中的波特率为38400），否则输出乱码  

3.25 HMI串口屏输出波形和频域
