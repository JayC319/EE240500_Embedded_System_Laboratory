#include "mbed.h"


PwmOut PWM1(D6);

AnalogIn Ain(A0);

float ADCdata;

int main(){

  while(1){

    PWM1.period_ms(500);

    PWM1 = Ain;

    ADCdata = Ain; //send an opening text message

    printf("%f\r\n", ADCdata);

    ThisThread::sleep_for(100ms);

  }

}