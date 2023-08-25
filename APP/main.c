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
