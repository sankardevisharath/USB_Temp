#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
//#include<stdio.h>
#include<stdint.h>
//#include<string.h>

#include "usbdrv.h"

#define F_CPU 12000000UL
#include <util/delay.h>
#define USB_LED_OFF 0
#define USB_LED_ON  1
#define USB_DATA_OUT 2
#define USB_TEMP_READ 3

void InitADC()
{
ADMUX=(1<<REFS0);                         // For Aref=AVcc;
ADCSRA=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //Rrescalar div factor =128
}
uint16_t ReadADC(uint8_t ch)
{
   //Select ADC Channel ch must be 0-7
   ch=ch&0b00000111;
   ADMUX|=ch;

   //Start Single conversion
   ADCSRA|=(1<<ADSC);

   //Wait for conversion to complete
   while(!(ADCSRA & (1<<ADIF)));

   //Clear ADIF by writing one to it
   //Note you may be wondering why we have write one to clear it
   //This is standard way of clearing bits in io as said in datasheets.
   //The code writes '1' but it result in setting bit to '0' !!!

   ADCSRA|=(1<<ADIF);

   return(ADC);
}



USB_PUBLIC uchar usbFunctionSetup(uchar data[8]) {
	usbRequest_t *rq = (void *)data; // cast data to correct type
	static uchar replyBuf[16];
	uint16_t adc_result;
	int i=0,j=0;

    switch(rq->bRequest) { // custom command is in the bRequest field
    case USB_LED_ON:
        PORTB |= 1; // turn LED on
        return 0;
    case USB_LED_OFF:
        PORTB &= ~1; // turn LED off
        return 0;
    case USB_DATA_OUT: // send data to PC
		if((PORTB & 1)==1)
			{
			replyBuf[0]='o';
			replyBuf[1]='n';
			replyBuf[2]='\0';
      		usbMsgPtr = replyBuf;
       		return 2;
			
			}
		else
			{
			replyBuf[0]='o';
			replyBuf[1]='f';
			replyBuf[2]='f';
			replyBuf[3]='\0';
			usbMsgPtr = replyBuf;
        	return 3;
			};
	case USB_TEMP_READ://Read temperature
			adc_result=ReadADC(0);
			adc_result/=2;
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
			usbMsgPtr = replyBuf;
			return i;
			/*sprintf(replyBuf, "Temp is %hd", adc_result);
			return strlen(replyBuf);*/
    }

    return 0; // should not get here

}

int main() {
        uchar i;

DDRB = 1; // PB0 as output
    wdt_enable(WDTO_1S); // enable 1s watchdog timer

    usbInit();

    usbDeviceDisconnect(); // enforce re-enumeration
    for(i = 0; i<250; i++) { // wait 500 ms
        wdt_reset(); // keep the watchdog happy
        _delay_ms(2);
    }
    usbDeviceConnect();

    sei(); // Enable interrupts after re-enumeration
	InitADC();

    while(1) {
        wdt_reset(); // keep the watchdog happy
        usbPoll();
    }

    return 0;
}
