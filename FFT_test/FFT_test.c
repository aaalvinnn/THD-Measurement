#include "stdio.h"
#include "math.h"
#include "kfft.c"


#define PI 3.1415926535
#define Length (1<<m)	//DMA转移数组长度
#define Fs 50000	//采样率
#define T 0.00002	//采样周期
#define m 12		//DMA转移数组长度取对数
#define F 1000 //输入信号的基频

/* 窗函数 */
void hannWin(int N, double w[])
{
	int n;

	for (n = 0; n < N; n++)
	{
		w[n] = 0.5 * (1 - cos(2 * PI * (double)n / (N - 1)));
	}
}
void hammingWin(int N, double w[])
{
	int n;

	for (n = 0; n < N; n++)
	{
		w[n] = 0.54 - 0.46 * cos(2 * PI *  (double)n / (N - 1));
	}
}

int main()
{ 
	int i,j;
    double pr[Length],pi[Length],fr[Length],fi[Length],t[Length];
	double window[Length];
	int flag[5];

	/* 窗函数 */
	hannWin(Length, window);	
	// hammingWin(Length,window);
    for (i=0; i<Length; i++)  //生成输入信号
    { 
		t[i] = i*T;
		pr[i]=1.5+1.5*cos(2*PI*F*t[i])+1.2*cos(2*PI*2*F*t[i])+1.0*cos(2*PI*3*F*t[i])+0.8*cos(2*PI*4*F*t[i])+0.5*cos(2*PI*5*F*t[i]);
		// printf("%d\t%lf\n",i,pr[i]);
		pr[i] *= window[i];
		pi[i]=0.0;
	}

	/* 频率分辨率 */
	double Fstep=(double)Fs/Length;
	printf("delta Fs: %.2f\n",Fstep);
	/* 调用FFT函数 */
    kfft(pr,pi,Length,m,fr,fi);  
	/* 输出结果 */
	// printf("the result of FFT:\n");
	// for(i=0; i<Length; i++)
    // { 
    //     printf("%.2f\t%lf\n",(double)i*Fstep,pr[i]); 
    // }

	/* 寻找极大值下标 */
	/* 先找出基波（一次谐波）频率最大值点 */
	flag[0]=2;
	/* 只查找前半部分，从i=2开始是因为排除直流分量（i=0）以及因加窗导致的直流分量频域附近的影响（i=2）*/
	for(i=2;i<(Length)>>1;i++)		
	{
		if(pr[i+1]>=pr[flag[0]])
		{
			flag[0]=i+1;
		}
	}
	printf("max No. is %d \n",flag[0]);
	/* 然后开始找五次谐波内的极大值点 */
	for(j=0;j<4;j++)
	{
		/* 因谐波频率都是基波频率的整数倍，故先确定小范围寻找极大值的中心点 */
		flag[j+1]=flag[j]+(int)F/Fstep;
		/* 一次搜寻包括中心频率点在内的附近5个点 */
		for(i=0;i<5;i++)		
		{
			if(pr[flag[j+1]+i-2]>=pr[flag[j+1]])
			{
				flag[j+1]=flag[j+1]+i-2;
			}
		}
	}
	/* 输出对应极大值的下标和频率 */
	for(j=0;j<5;j++)
	{
		printf("No.%d is %d \t which Frequency is %.4f \n",j+1,flag[j],flag[j]*Fstep);	
	}
	
    // /* 幅值恢复（因为加了窗函数） */
	// // 为了节约资源，只处理一、二、三、四、五次谐波点
	// for(j=0;j<5;j++)
	// {
	// 	(double)pr[flag[j]]/=window[flag[j]];		//错啦！时域乘积，频域卷积！！！
	// 	printf("\nfre:%.2f\tamp*%d:%.2f",flag[j]*Fstep,Length>>1,pr[flag[j]]);
	// }

	/* 计算失真度
	*  误差范围在3%之内
	 */
	double Distortion=0;
    Distortion = sqrt((pr[flag[1]])*(pr[flag[1]]) //二次谐波
    +(pr[flag[2]])*(pr[flag[2]])  //三次谐波
    +(pr[flag[3]])*(pr[flag[3]])  //四次谐波
    +(pr[flag[4]])*(pr[flag[4]])) //五次谐波
    /(pr[flag[0]]); //一次谐波频率分量幅值
    printf("\nDistortion:%.2f\n",Distortion*100);	

	/* 计算归一化幅值
	*  加窗之后得到的结果似乎更准确，虽然和真实值的误差还是有点小大
	*/
	printf("***********************\n");
	for(j=0;j<5;j++)
	{
		printf("xiebo-%d : %.3f\n",j+1,pr[flag[j]]/pr[flag[0]]);
	}
}
