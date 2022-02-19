#include "mbed.h"

void Led(DigitalOut &ledpin,int cycle_num)
{
   for(int i = 0; i < cycle_num; ++i)
    {
        ledpin = !ledpin;
        ThisThread::sleep_for(500ms);
    }

    ledpin = 0;
}





