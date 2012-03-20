#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "usbdrv.h"

#define F_CPU 12000000UL
#include <util/delay.h>
#define USB_LED_OFF 0
#define USB_LED_ON  1
#define USB_DATA_OUT 2




USB_PUBLIC uchar usbFunctionSetup(uchar data[8]) {
usbRequest_t *rq = (void *)data; // cast data to correct type
static uchar replyBuf[16];

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
			}

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

    while(1) {
        wdt_reset(); // keep the watchdog happy
        usbPoll();
    }

    return 0;
}
