/*
*file		WindowFunction.h
*author		Vincent Cui
*e-mail		whcui1987@163.com
*version	0.3
*data		31-Oct-2014
*brief		各种窗函数的C语言实现
*/

#ifndef _WINDOWFUNCTION_H_
#define _WINDOWFUNCTION_H_

#include <stdint.h>

#define besseli_Flag	0	//缺少besseli函数
#define prod_Flag		0	//缺少prod函数
#define linSpace_Flag	0	//缺少linSpace函数

#define BESSELI_K_LENGTH 10

#define FLATTOPWIN_A0  0.215578995
#define FLATTOPWIN_A1  0.41663158
#define FLATTOPWIN_A2  0.277263158
#define FLATTOPWIN_A3  0.083578947
#define FLATTOPWIN_A4  0.006947368

#define NUTTALL_A0   0.3635819
#define NUTTALL_A1   0.4891775
#define NUTTALL_A2   0.1365995
#define NUTTALL_A3   0.0106411

#define BLACKMANHARRIS_A0 0.35875
#define BLACKMANHARRIS_A1 0.48829
#define BLACKMANHARRIS_A2 0.14128
#define BLACKMANHARRIS_A3 0.01168

#define PI 3.14159265358979323846264338327950288419717  //定义圆周率值

typedef enum
{
	DSP_ERROR = 0,
	DSP_SUCESS,
}dspErrorStatus;

dspErrorStatus triangularWin(uint16_t N, double w[]);
dspErrorStatus bartlettWin(uint16_t N, double w[]);
dspErrorStatus bartLettHannWin(uint16_t N, double w[]);
dspErrorStatus blackManWin(uint16_t N, double w[]);
dspErrorStatus blackManHarrisWin(uint16_t N, double w[]);
dspErrorStatus bohmanWin(uint16_t N, double w[]);
dspErrorStatus chebyshevWin(uint16_t N, double r, double w[]);
dspErrorStatus flatTopWin(uint16_t N, double w[]);
dspErrorStatus gaussianWin(uint16_t N, double alpha, double w[]);
dspErrorStatus hammingWin(uint16_t N, double w[]);
dspErrorStatus hannWin(uint16_t N, double w[]);
dspErrorStatus nuttalWin(uint16_t N, double w[]);
dspErrorStatus parzenWin(uint16_t N, double w[]);
dspErrorStatus rectangularWin(uint16_t N, double w[]);

#if besseli_Flag
dspErrorStatus kaiserWin(uint16_t N, double beta, double w[]);
#endif

#if prod_Flag
dspErrorStatus taylorWin(uint16_t N, uint16_t nbar, double sll, double w[]);
#endif

#if linSpace_Flag
dspErrorStatus tukeyWin(uint16_t N, double r, double w[]);
#endif

#endif
