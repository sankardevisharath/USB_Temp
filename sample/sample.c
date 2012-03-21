#include<stdio.h>
#include<stdint.h>
main()
	{
	int i,j=0;
	unsigned char replyBuf[16];
uint16_t adc_result=48;
	i=0;
			while(adc_result>0)
				{	

				j=i-1;
				while(j>=0)
					{
					replyBuf[j+1]=replyBuf[j];
					j--;
					}
				replyBuf[0]=(char)((adc_result%10)+48);
				i++;
				adc_result/=10;
				}
			replyBuf[i]='\0';

	printf("%s\n", replyBuf);
	}
