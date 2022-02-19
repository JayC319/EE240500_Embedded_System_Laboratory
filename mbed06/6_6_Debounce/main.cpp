#include "mbed.h"
using namespace std::chrono;

Timer debounce;
InterruptIn button(USER_BUTTON);
DigitalOut led1(LED1);

void toggle()
{
    if (duration_cast<milliseconds>(debounce.elapsed_time()).count() > 5000)
    {
        led1 = !led1;
        debounce.reset();
    }
}

int main()
{
    debounce.start();
    button.rise(&toggle);
    while(1)
        ;
}