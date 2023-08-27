
//#define PERIODIC
//#define PWM
#define EDGETIME

#ifdef PERIODIC
#include "GPTM.h"
#include "DIO.h"

volatile uint32 flag = 0;
volatile uint32 state = 0;

void timerISR(){
    flag = 1;
    TimerIntClear(TIMER_0, TATOIM);
}

uint32 f = 0;

void main()
{
    DIO_Init();
    TimerStartUse(TIMER_0);
    TimerStartUse(TIMER_1);
    TimerConfigure(TIMER_0, TIMERB, TIMER_MODE16, (TMR_PERIODIC | TAMS | 0xA));
    TimerConfigure(TIMER_1, TIMERA, TIMER_MODE16, (TMR_PERIODIC | TAMS | 0xA));
    TimerConfigure(TIMER_1, TIMERB, TIMER_MODE16, (TMR_PERIODIC | TAMS | 0xA));

    TimerConfigure(TIMER_0, TIMERA, TIMER_MODE16, (TMR_ONE_SHOT));
    TimerControlStall(TIMER_0, TIMERB, true);
    TimerControlStall(TIMER_1, TIMERA, true);
    TimerControlStall(TIMER_1, TIMERB, true);
    TimerControlStall(TIMER_0, TIMERA, true);

    TimerPrescaleSet(TIMER_0, TIMERB, 250);
    TimerPrescaleSet(TIMER_1, TIMERA, 250);
    TimerPrescaleSet(TIMER_0, TIMERB, 250);

    TimerPrescaleSet(TIMER_0, TIMERA, 30);

    //TimerIntNVICEnable();
    //TimerIntEnable(TIMER_0, TATOIM);

    TimerLoadSet(TIMER_0, TIMERB, 0xFFFF);
    TimerLoadSet(TIMER_1, TIMERA, 0xFFFF);
    TimerLoadSet(TIMER_1, TIMERB, 0xFFFF);
    TimerLoadSet(TIMER_0, TIMERA, 0xFFFF);
    //TimerIntRegister(TIMER_0, TIMERA, timerISR);

    GPIO_PORTF_AFSEL_R |= (1<<1);
    GPIO_PORTF_PCTL_R  |= (7<<4);

    GPIO_PORTF_AFSEL_R |= (1<<2);
    GPIO_PORTF_PCTL_R  |= (7<<8);

    GPIO_PORTF_AFSEL_R |= (1<<3);
    GPIO_PORTF_PCTL_R  |= (7<<12);

    TimerEnable(TIMER_0, TIMERB);
    TimerEnable(TIMER_0, TIMERA);
    TimerEnable(TIMER_1, TIMERB);
    //TimerEnable(TIMER_1, TIMERA);

    uint32 rate = 0;

    while(1){
        TimerMatchSet(TIMER_0, TIMERB, rate);
        TimerMatchSet(TIMER_1, TIMERA, rate);
        TimerMatchSet(TIMER_1, TIMERB, rate);

        while(GetEnBit(TIMER_0, TIMERA));
        TimerEnable(TIMER_0, TIMERA);
        rate += 0x1000;
        if(rate > 0xFF00){
            rate = 0;
        }
    }
}
#endif
#ifdef PWM

#include "GPTM.h"
#include "DIO.h"

volatile uint32 flag = 0;
volatile uint32 state = 0;

void timerISR(){
    flag = 1;
    TimerIntClear(TIMER_0, TATOIM);
}

void main()
{
    DIO_Init();
    TimerStartUse(TIMER_0);
    TimerConfigure(TIMER_0, TIMERA, TIMER_MODE16, (TMR_ONE_SHOT));
    TimerControlStall(TIMER_0, TIMERA, true);
    TimerPrescaleSet(TIMER_0, TIMERA, 61);
    TimerIntNVICEnable();
    TimerIntEnable(TIMER_0, TATOIM);
    TimerLoadSet(TIMER_0, TIMERA, 0xFFFF);
    TimerIntRegister(TIMER_0, TIMERA, timerISR);
    TimerEnable(TIMER_0, TIMERA);

    while(1){
        if(flag == 1){
            flag = 0;
            if(state == 1){
                DIO_WritePin(DIO_PortF, 1, LOW);
                DIO_WritePin(DIO_PortF, 2, LOW);
                DIO_WritePin(DIO_PortF, 3, LOW);
                state = 0;
            }else{
                DIO_WritePin(DIO_PortF, 1, HIGH);
                DIO_WritePin(DIO_PortF, 2, HIGH);
                DIO_WritePin(DIO_PortF, 3, HIGH);
                state = 1;
            }
            TimerEnable(TIMER_0, TIMERA);
        }
    }
}
#endif

#ifdef EDGETIME

#include "GPTM.h"
#include "DIO.h"
#include "LCD.h"

void updateLCD(void)
{
    // Update time variables
    ++time;
    sec = time % 60;

    // Update hours if necessary
    if (min == 0 && sec == 0)
    {
        hours = time / 3600;
        LcdSendCommand(0x80);
        LcdSendData((hours / 10) + '0');
        LcdSendData((hours % 10) + '0');
        LcdSendData(':');
        LcdSendData('0');
        LcdSendData('0');
        LcdSendData(':');
        LcdSendData('0');
        LcdSendData('0');
    }
    
    // Update minutes if necessary
    if (sec == 0)
    {
        min = (time / 60) % 60;
        LcdSendCommand(0x83);
        LcdSendData((min / 10) + '0');
        LcdSendData((min % 10) + '0');
        LcdSendData(':');
        LcdSendData('0');
        LcdSendData('0');
    }

    // Update seconds (milliseconds) part on LCD display
    LcdSendCommand(0x86);
    LcdSendData((sec / 10) + '0');
    LcdSendData((sec % 10) + '0');
}

void main(void){

    DIO_Init();
    LCD_Init();
    LcdSendString("00:00:00");
    TimerStartUse(WTIMER_0);
    TimerConfigure(WTIMER_0, TIMERA, WTIMER_MODE32, (TMR_CAPTURE | TCDIR | TCMR ));
    TimerControlStall(WTIMER_0, TIMERA, true);
    TimerEnable(WTIMER_0, TIMERA);

    GPIO_PORTD_AFSEL_R |= 0x01;
    GPIO_PORTD_PCTL_R &= ~(0x0000000F);
    GPIO_PORTD_PCTL_R |= 0x00000007;

    while(1){
        
    }

}