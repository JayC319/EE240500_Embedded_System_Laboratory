#include "mbed.h"

DigitalOut led1(LED1);
InterruptIn sw2(USER_BUTTON);
EventQueue queue(32 * EVENTS_EVENT_SIZE);

Thread t;

void led1_info() {
    printf("led1 is triggered! \r\n");
}

void Trig_led1() {
    led1 = !led1;

    queue.call(led1_info);
}

int main() {
    t.start(callback(&queue, &EventQueue::dispatch_forever));

    sw2.rise(Trig_led1);
    }
