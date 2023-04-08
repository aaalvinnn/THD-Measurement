/*
*file		WindowFunction.c
*author		Vincent Cui
*e-mail		whcui1987@163.com
*version	0.3
*data		31-Oct-2014
*brief		各种窗函数的C语言实现
*/

#include "WindowFunction.h"
#include <math.h>
#include <stdlib.h>

#if prod_Flag
/*函数名：taylorWin
*说明：计算泰勒窗。泰勒加权函数
*输入：
*输出：
*返回：
*调用：prod()连乘函数
*其它：用过以后，需要手动释放掉*w的内存空间
*        调用示例：ret = taylorWin(99, 4, 40, &w); 注意此处的40是正数 表示-40dB
*/
dspErrorStatus taylorWin(dspUint_16 N, dspUint_16 nbar, dspDouble sll, dspDouble **w)
{
	dspDouble A;
	dspDouble *retDspDouble;
	dspDouble *sf;
	dspDouble *result;
	dspDouble alpha, beta, theta;
	dspUint_16 i, j;

	/*A = R   cosh(PI, A) = R*/
	A = (dspDouble)acosh(pow((dspDouble)10.0, (dspDouble)sll / 20.0)) / PI;
	A = A * A;

	/*开出存放系数的空间*/
	retDspDouble = (dspDouble *)malloc(sizeof(dspDouble) * (nbar - 1));
	if (retDspDouble == NULL)
		return DSP_ERROR;
	sf = retDspDouble;

	/*开出存放系数的空间*/
	retDspDouble = (dspDouble *)malloc(sizeof(dspDouble) * N);
	if (retDspDouble == NULL)
		return DSP_ERROR;
	result = retDspDouble;

	alpha = prod(1, 1, (nbar - 1));
	alpha *= alpha;
	beta = (dspDouble)nbar / sqrt(A + pow((nbar - 0.5), 2));
	for (i = 1; i <= (nbar - 1); i++)
	{
		*(sf + i - 1) = prod(1, 1, (nbar - 1 + i)) * prod(1, 1, (nbar - 1 - i));
		theta = 1;
		for (j = 1; j <= (nbar - 1); j++)
		{
			theta *= 1 - (dspDouble)(i * i) / (beta * beta * (A + (j - 0.5) * (j - 0.5)));
		}
		*(sf + i - 1) = alpha * (dspDouble)theta / (*(sf + i - 1));
	}

	/*奇数阶*/
	if ((N % 2) == 1)
	{
		for (i = 0; i < N; i++)
		{
			alpha = 0;
			for (j = 1; j <= (nbar - 1); j++)
			{
				alpha += (*(sf + j - 1)) * cos(2 * PI * j * (dspDouble)(i - ((N - 1) / 2)) / N);
			}
			*(result + i) = 1 + 2 * alpha;
		}
	}
	/*偶数阶*/
	else
	{
		for (i = 0; i < N; i++)
		{
			alpha = 0;
			for (j = 1; j <= (nbar - 1); j++)
			{
				alpha += (*(sf + j - 1)) * cos(PI * j * (dspDouble)(2 * (i - (N / 2)) + 1) / N);
			}
			*(result + i) = 1 + 2 * alpha;

		}
	}
	*w = result;
	free(sf);

	return DSP_SUCESS;
}
#endif

/*
*函数名：triangularWin
*说明：计算三角窗函数
*输入：
*输出：
*返回：
*调用：
*调用示例：ret = triangularWin(99, w);
*/
dspErrorStatus triangularWin(uint16_t N, double w[])
{
	uint16_t i;

	/*阶数为奇*/
	if ((N % 2) == 1)
	{
		for (i = 0; i < ((N - 1) / 2); i++)
		{
			w[i] = 2 * (double)(i + 1) / (N + 1);
		}
		for (i = ((N - 1) / 2); i < N; i++)
		{
			w[i] = 2 * (double)(N - i) / (N + 1);
		}
	}
	/*阶数为偶*/
	else
	{
		for (i = 0; i < (N / 2); i++)
		{
			w[i] = (i + i + 1) * (double)1 / N;
		}
		for (i = (N / 2); i < N; i++)
		{
			w[i] = w[N - 1 - i];
		}
	}

	return DSP_SUCESS;
}

#if linSpace_Flag
/*
*函数名：tukeyWin
*说明：计算tukey窗函数
*输入：
*输出：
*返回：linSpace()
*调用：
*其它：用过以后，需要手动释放掉*w的内存空间
*        调用示例：ret = tukeyWin(99, 0.5, &w);
*/
dspErrorStatus tukeyWin(dspUint_16 N, dspDouble r, dspDouble **w)
{
	dspErrorStatus retErrorStatus;
	dspUint_16        index;
	dspDouble        *x, *result, *retPtr;
	dspDouble        alpha;

	retErrorStatus = linSpace(0, 1, N, &x);
	if (retErrorStatus == DSP_ERROR)
		return DSP_ERROR;

	result = (dspDouble *)malloc(N * sizeof(dspDouble));
	if (result == NULL)
		return DSP_ERROR;

	/*r <= 0 就是矩形窗*/
	if (r <= 0)
	{
		retErrorStatus = rectangularWin(N, &retPtr);
		if (retErrorStatus == DSP_ERROR)
			return DSP_ERROR;
		/*将数据拷出来以后，释放调用的窗函数的空间*/
		memcpy(result, retPtr, (N * sizeof(dspDouble)));
		free(retPtr);
	}
	/*r >= 1 就是汉宁窗*/
	else if (r >= 1)
	{
		retErrorStatus = hannWin(N, &retPtr);
		if (retErrorStatus == DSP_ERROR)
			return DSP_ERROR;
		/*将数据拷出来以后，释放调用的窗函数的空间*/
		memcpy(result, retPtr, (N * sizeof(dspDouble)));
		free(retPtr);
	}
	else
	{
		for (index = 0; index < N; index++)
		{
			alpha = *(x + index);
			if (alpha < (r / 2))
			{
				*(result + index) = (dspDouble)(1 + cos(2 * PI * (dspDouble)(alpha - (dspDouble)r / 2) / r)) / 2;
			}
			else if ((alpha >= (r / 2)) && (alpha <(1 - r / 2)))
			{
				*(result + index) = 1;
			}
			else
			{
				*(result + index) = (dspDouble)(1 + cos(2 * PI * (dspDouble)(alpha - 1 + (dspDouble)r / 2) / r)) / 2;
			}

		}
	}

	free(x);

	*w = result;

	return DSP_SUCESS;
}
#endif

/*
*函数名：bartlettWin
*说明：计算bartlettWin窗函数
*输入：
*输出：
*返回：
*调用：
*调用示例：ret = bartlettWin(99, w);
*/
dspErrorStatus bartlettWin(uint16_t N, double w[])
{
	
	uint16_t n;

	for (n = 0; n < (N - 1) / 2; n++)
	{
		w[n] = 2 * (double)n / (N - 1);
	}

	for (n = (N - 1) / 2; n < N; n++)
	{
		w[n] = 2 - 2 * (double)n / ((N - 1));
	}

	return DSP_SUCESS;
}

/*
*函数名：bartLettHannWin
*说明：计算bartLettHannWin窗函数
*输入：
*输出：
*返回：
*调用：
*调用示例：ret = bartLettHannWin(99, w);
*/
dspErrorStatus bartLettHannWin(uint16_t N, double w[])
{
	uint16_t n;

	/*奇*/
	if ((N % 2) == 1)
	{
		for (n = 0; n < N; n++)
		{
			w[n] = 0.62 - 0.48 * fabs(((double)n / (N - 1)) - 0.5) + 0.38 * cos(2 * PI * (((double)n / (N - 1)) - 0.5));
		}
		for (n = 0; n < (N - 1) / 2; n++)
		{
			w[n] = w[N - 1 - n];
		}
	}
	/*偶*/
	else
	{
		for (n = 0; n < N; n++)
		{
			w[n] = 0.62 - 0.48 * fabs(((double)n / (N - 1)) - 0.5) + 0.38 * cos(2 * PI * (((double)n / (N - 1)) - 0.5));
		}
		for (n = 0; n < N / 2; n++)
		{
			w[n] = w[N - 1 - n];
		}
	}

	return DSP_SUCESS;
}

/*
*函数名：blackManWin
*说明：计算blackManWin窗函数
*输入：
*输出：
*返回：
*调用：
*调用示例：ret = blackManWin(99, w);
*/
dspErrorStatus blackManWin(uint16_t N, double w[])
{
	uint16_t n;

	for (n = 0; n < N; n++)
	{
		w[n] = 0.42 - 0.5 * cos(2 * PI * (double)n / (N - 1)) + 0.08 * cos(4 * PI * (double)n / (N - 1));
	}

	return DSP_SUCESS;
}

/*
*函数名：blackManHarrisWin
*说明：计算blackManHarrisWin窗函数
*输入：
*输出：
*返回：
*调用：
*调用示例：ret = blackManHarrisWin(99, w);
*  minimum 4-term Blackman-harris window -- From Matlab
*/
dspErrorStatus blackManHarrisWin(uint16_t N, double w[])
{
	uint16_t n;

	for (n = 0; n < N; n++)
	{
		w[n] = BLACKMANHARRIS_A0 - BLACKMANHARRIS_A1 * cos(2 * PI * (double)n / (N)) + \
			BLACKMANHARRIS_A2 * cos(4 * PI * (double)n / (N)) - \
			BLACKMANHARRIS_A3 * cos(6 * PI * (double)n / (N));
	}

	return DSP_SUCESS;
}

/*
*函数名：bohmanWin
*说明：计算bohmanWin窗函数
*输入：
*输出：
*返回：
*调用：
*调用示例：ret = bohmanWin(99, w);
*/
dspErrorStatus bohmanWin(uint16_t N, double w[])
{
	uint16_t n;
	
	double x;

	for (n = 0; n < N; n++)
	{
		x = -1 + n *  (double)2 / (N - 1);
		/*取绝对值*/
		x = x >= 0 ? x : (x * (-1));
		w[n] = (1 - x) * cos(PI * x) + (double)(1 / PI) * sin(PI * x);
	}

	return DSP_SUCESS;
}

/*
*函数名：chebyshevWin
*说明：计算chebyshevWin窗函数
*输入：
*输出：
*返回：
*调用：
*调用示例：ret = chebyshevWin(99,100, w);
*/
dspErrorStatus chebyshevWin(uint16_t N, double r, double w[])
{
	uint16_t n, index;
	
	double x, alpha, beta, theta, gama;

	/*10^(r/20)*/
	theta = pow((double)10, (double)(fabs(r) / 20));
	beta = pow(cosh(acosh(theta) / (N - 1)), 2);
	alpha = 1 - (double)1 / beta;

	if ((N % 2) == 1)
	{
		/*计算一半的区间*/
		for (n = 1; n < (N + 1) / 2; n++)
		{
			gama = 1;
			for (index = 1; index < n; index++)
			{
				x = index * (double)(N - 1 - 2 * n + index) / ((n - index) * (n + 1 - index));
				gama = gama * alpha * x + 1;
			}
			w[n] = (N - 1) * alpha * gama;
		}

		theta = w[(N - 1) / 2];
		w[0] = 1;

		for (n = 0; n < (N + 1) / 2; n++)
		{
			w[n] = (double)(w[n]) / theta;
		}

		/*填充另一半*/
		for (; n < N; n++)
		{
			w[n] = w[N - n - 1];
		}
	}
	else
	{
		/*计算一半的区间*/
		for (n = 1; n < (N + 1) / 2; n++)
		{
			gama = 1;
			for (index = 1; index < n; index++)
			{
				x = index * (double)(N - 1 - 2 * n + index) / ((n - index) * (n + 1 - index));
				gama = gama * alpha * x + 1;
			}
			w[n] = (N - 1) * alpha * gama;
		}

		theta = w[(N / 2) - 1];
		w[0] = 1;

		for (n = 0; n < (N + 1) / 2; n++)
		{
			w[n] = (double)(w[n]) / theta;
		}

		/*填充另一半*/
		for (; n < N; n++)
		{
			w[n] = w[N - n - 1];
		}
	}

	return DSP_SUCESS;
}

/*
*函数名：flatTopWin
*说明：计算flatTopWin窗函数
*输入：
*输出：
*返回：
*调用：
*调用示例：ret = flatTopWin(99, w);
*/
dspErrorStatus flatTopWin(uint16_t N, double w[])
{
	uint16_t n;

	for (n = 0; n < N; n++)
	{
		w[n] = FLATTOPWIN_A0 - FLATTOPWIN_A1 * cos(2 * PI * (double)n / (N - 1)) + \
			FLATTOPWIN_A2 * cos(4 * PI * (double)n / (N - 1)) - \
			FLATTOPWIN_A3 * cos(6 * PI * (double)n / (N - 1)) + \
			FLATTOPWIN_A4 * cos(8 * PI * (double)n / (N - 1));
	}

	return DSP_SUCESS;
}


/*
*函数名：gaussianWin
*说明：计算gaussianWin窗函数
*输入：
*输出：
*返回：
*调用：
*调用示例：ret = gaussianWin(99,2.5, w);
*/
dspErrorStatus gaussianWin(uint16_t N, double alpha, double w[])
{
	uint16_t n;
	double k, beta, theta;

	for (n = 0; n < N; n++)
	{
		if ((N % 2) == 1)
		{
			k = n - (N - 1) / 2;
			beta = 2 * alpha * (double)k / (N - 1);
		}
		else
		{
			k = n - (N) / 2;
			beta = 2 * alpha * (double)k / (N - 1);
		}

		theta = pow(beta, 2);
		w[n] = exp((-1) * (double)theta / 2);
	}

	return DSP_SUCESS;
}

/*
*函数名：hammingWin
*说明：计算hammingWin窗函数
*输入：
*输出：
*返回：
*调用：
*调用示例：ret = hammingWin(99, w);
*/
dspErrorStatus hammingWin(uint16_t N, double w[])
{
	uint16_t n;

	for (n = 0; n < N; n++)
	{
		w[n] = 0.54 - 0.46 * cos(2 * PI *  (double)n / (N - 1));
	}

	return DSP_SUCESS;
}

/*
*函数名：hannWin
*说明：计算hannWin窗函数
*输入：
*输出：
*返回：
*调用：
*调用示例：ret = hannWin(99, w);
*/
dspErrorStatus hannWin(uint16_t N, double w[])
{
	uint16_t n;

	for (n = 0; n < N; n++)
	{
		w[n] = 0.5 * (1 - cos(2 * PI * (double)n / (N - 1)));
	}

	return DSP_SUCESS;
}

#if besseli_Flag
/*
*函数名：kaiserWin
*说明：计算kaiserWin窗函数
*输入：
*输出：
*返回：
*调用：besseli()第一类修正贝塞尔函数
*其它：用过以后，需要手动释放掉*w的内存空间
*        调用示例：ret = kaiserWin(99, 5, &w);
*/
dspErrorStatus kaiserWin(dspUint_16 N, dspDouble beta, dspDouble **w)
{
	dspUint_16 n;
	dspDouble *ret;
	dspDouble theta;

	ret = (dspDouble *)malloc(N * sizeof(dspDouble));
	if (ret == NULL)
		return DSP_ERROR;

	for (n = 0; n < N; n++)
	{
		theta = beta * sqrt(1 - pow(((2 * (dspDouble)n / (N - 1)) - 1), 2));
		*(ret + n) = (dspDouble)besseli(0, theta, BESSELI_K_LENGTH) / besseli(0, beta, BESSELI_K_LENGTH);
	}

	*w = ret;

	return DSP_SUCESS;
}
#endif

/*
*函数名：nuttalWin
*说明：计算nuttalWin窗函数
*输入：
*输出：
*返回：
*调用：
*调用示例：ret = nuttalWin(99, w);
*/
dspErrorStatus nuttalWin(uint16_t N, double w[])
{
	uint16_t n;

	for (n = 0; n < N; n++)
	{
		w[n] = NUTTALL_A0 - NUTTALL_A1 * cos(2 * PI * (double)n / (N - 1)) + \
			NUTTALL_A2 * cos(4 * PI * (double)n / (N - 1)) - \
			NUTTALL_A3 * cos(6 * PI * (double)n / (N - 1));
	}

	return DSP_SUCESS;
}

/*
*函数名：parzenWin
*说明：计算parzenWin窗函数
*输入：
*输出：
*返回：
*调用：
*调用示例：ret = parzenWin(99, w);
*/
dspErrorStatus parzenWin(uint16_t N, double w[])
{
	uint16_t n;
	
	double alpha, k;

	if ((N % 2) == 1)
	{
		for (n = 0; n < N; n++)
		{
			k = n - (N - 1) / 2;
			alpha = 2 * (double)fabs(k) / N;
			if (fabs(k) <= (N - 1) / 4)
			{
				w[n] = 1 - 6 * pow(alpha, 2) + 6 * pow(alpha, 3);
			}
			else
			{
				w[n] = 2 * pow((1 - alpha), 3);
			}
		}
	}
	else
	{
		for (n = 0; n < N; n++)
		{
			k = n - (N - 1) / 2;
			alpha = 2 * (double)fabs(k) / N;
			if (fabs(k) <= (double)(N - 1) / 4)
			{
				w[n] = 1 - 6 * pow(alpha, 2) + 6 * pow(alpha, 3);
			}
			else
			{
				w[n] = 2 * pow((1 - alpha), 3);
			}

		}
	}

	return DSP_SUCESS;
}

/*
*函数名：rectangularWin
*说明：计算rectangularWin窗函数
*输入：
*输出：
*返回：
*调用：
*调用示例：ret = rectangularWin(99, w);
*/
dspErrorStatus rectangularWin(uint16_t N, double w[])
{
	uint16_t n;

	for (n = 0; n < N; n++)
	{
		w[n] = 1;
	}

	return DSP_SUCESS;
}
