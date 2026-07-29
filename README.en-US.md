

# THD Measurement Device

(National Undergraduate Electronics Design Contest 2021, Topic A)

## Requirements

1. Input signal peak-to-peak voltage range: 300mV~600mV
2. Input signal fundamental frequency: 1kHz~100kHz
3. Display the measured distortion value
4. Measure and display one cycle waveform of the input signal
5. Display the normalized amplitudes of the fundamental and harmonics of the input signal, up to the 5th harmonic
6. Display on Android host PC

## Overall Framework

![image-20230422114557818](https://raw.githubusercontent.com/aaalvinnn/THD-Measurement/main/images/image-20230422114557818.png)

## Hardware Approach

The VGA module amplifies the input signal amplitude to 3V for MCU ADC sampling. A DC bias is added simultaneously, and negative feedback gain control is implemented via peak detection.

### Module Design

(VGA Schematic)



(DC Bias Schematic)



(Peak Detection Schematic)



![image-20230422114651729](https://raw.githubusercontent.com/aaalvinnn/THD-Measurement/main/images/image-20230422114651729.png)

## Software Approach

### Signal Processing

- **Sampling**:

  Single-channel sampling using ADC+TIM+DMA, with 2048 sampling points, single buffering, and three-pass moving average smoothing;

- **FFT**:

  Ported .c/.h files;

  Note the issue of frequency resolution:

$$
频率分辨率=\frac{采样频率}{采样点数}
$$

  Simultaneously apply a Hann window function, and compensate for the spectrum after FFT (sum the 3 points around the peak point as the amplitude of that frequency);

- **Distortion**:

  According to the problem requirements, a simplified distortion calculation formula is used: 
$$
THD_o=\frac{\sqrt{U_{o2}^2+U_{o3}^2+U_{o4}^2+U_{o5}^2}}{U_{o1}}\times100\%
$$
  The indices for the fundamental and harmonics are found as follows:

1. First, traverse the post-FFT result array$*pr$ to find the point with the maximum amplitude (excluding the DC bias point$pr[0]$);
2. Multiply by \*2, \*3, \*4, \*5 respectively to get the approximate range for each harmonic index, then perform a local maximum search centered on these points to obtain the exact harmonic indices
3. Sum the 3 adjacent points, and use the sum as the amplitude for that harmonic frequency;

- **Normalized Amplitude**:

  Standard normalization of each harmonic amplitude divided by the fundamental amplitude.

### Host Computer

- HC-05 Bluetooth Communication Module

  Serial protocol based on USART, easy to program;

- Bluetooth Electronics Android Host App

  Bluetooth-based host application with drag-and-drop custom UI controls, easy to develop;

  However, wireless communication is not particularly stable, occasionally resulting in distorted waveforms;

- TJC Serial Display

  (Not implemented...)

## Result Analysis

Input fundamentals f=3, 4, 5kHz, amplitude 1000mV, 5th harmonic amplitude all 500mV, theoretical THD 100%

![image-20230422121246383](https://raw.githubusercontent.com/aaalvinnn/THD-Measurement/main/images/image-20230422121246383.png)

![image-20230422121251738](https://raw.githubusercontent.com/aaalvinnn/THD-Measurement/main/images/image-20230422121251738.png)

![image-20230422121302015](https://raw.githubusercontent.com/aaalvinnn/THD-Measurement/main/images/image-20230422121302015.png)

(Software-hardware joint debugging...)



## Issues & Reflections

Regarding software, I am still not entirely clear on configuring clock frequencies in CubeMX. Since timer-triggered ADC sampling is used, I found that after configuring the clock frequency in CubeMX, modifying the TIM prescaler and overflow period had no effect on the actual sampling frequency. Only changing the overall clock frequency could reduce the sampling frequency. (This is because the principle of FFT dictates that a too-high sampling frequency results in very low frequency resolution, leading to larger errors in calculated distortion for low-frequency inputs.)

Regarding hardware, (...)
