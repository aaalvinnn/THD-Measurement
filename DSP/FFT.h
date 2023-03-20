#ifndef FFT_H_
#define FFT_H_

#include "stdlib.h"
#include "math.h"

typedef struct
{
	double real;
	double imag;
} Complex;

void Sin_table(int N);//正弦表生成
unsigned char FFT(Complex *x,int m);	//进行原地FFT，结果储存在输入的时序数组中
void AmpSpectrum(Complex *pData,int m,float* pDCAmp,float* pDistortion);	//pData为FFT变换后的数组，m=log2N，pDCAmp为直流量幅值，pDistortion为失真率

#endif /* FFT_H_ */
