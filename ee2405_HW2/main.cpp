#include "mbed.h"
#include "wave.h"
#include "uLCD_4DGL.h"
#include<iostream>
using namespace std::chrono;
using namespace std;

AnalogIn Ain(A0);
uLCD_4DGL uLCD(D1, D0, D2); 
BusIn Buttons(D12, D11, D10);
int frequency[9] = {5,10,30,40,50,100,150,200,250};
float ADCdata[1000];

EventQueue eventQueue;

void sampling() {
    while(1) {

        for (int i = 0; i < 1000; i++){
            ADCdata[i] = Ain;
           ThisThread::sleep_for(1ms);
        }
        for (int i = 0; i < 1000; i++){
            cout << ADCdata[i] * 3.3 << "\r\n";
        }
        ThisThread::sleep_for(5000ms);
    }
}


int main()

{
    uLCD.color(BLUE);
    
    uLCD.printf("\nHello uLCD World\n"); 

    uLCD.printf("\n  Starting Demo...");

    ThisThread::sleep_for(500ms);

    uLCD.cls();

    uLCD.text_width(2); 

    uLCD.text_height(4);

    uLCD.color(BLUE);

    uLCD.locate(1,2);

    uLCD.printf(" 150hz\n-------");

int barLevel  = 6;
bool whileBreaker = false;

while (1){
    
        switch(Buttons){
        case 0x4:
            if(barLevel >= 8)
            {
                uLCD.cls();
                uLCD.text_width(2); 

                uLCD.text_height(4);

                uLCD.color(WHITE);

                uLCD.locate(1,2);
                uLCD.printf(" N\\A");
                 ThisThread::sleep_for(50ms);
                char bar[9] = "";
                 for(int count = 0; count <= barLevel; count++)
                    {
                     bar[count] = '-';
                    }
                uLCD.cls();
                uLCD.text_width(2); 

                uLCD.text_height(4);

                uLCD.color(BLUE);

                uLCD.locate(1,2);
                uLCD.printf(" %dhz\n%s ",frequency[barLevel],bar);

                
            }
                    
            else{
                uLCD.cls();
                barLevel++;
                uLCD.text_width(2); 

                uLCD.text_height(4);

                if(barLevel >= 6)
                    uLCD.color(BLUE);
                else if(barLevel <= 2)
                    uLCD.color(RED);
                else
                    uLCD.color(GREEN);

                uLCD.locate(1,2);

                char bar[9] = "";
                for(int count = 0; count <= barLevel; count++)
                {
                    bar[count] = '-';
                }
                uLCD.printf(" %dhz\n%s",frequency[barLevel],bar);

                    ThisThread::sleep_for(200ms);
            }        
            break;

        case 0x2:
            uLCD.cls();
            uLCD.locate(1,1);
            uLCD.printf("You select %dhz",frequency[barLevel]);
            whileBreaker = true;
            break;

        case 0x1:
            if(barLevel <= 0)
            {
                uLCD.cls();
                uLCD.text_width(2); 

                uLCD.text_height(4);

                uLCD.color(WHITE);

                uLCD.locate(1,2);
                uLCD.printf(" N\\A");
                ThisThread::sleep_for(50ms);

                char bar[9] = "";
                for(int count = 0; count <= barLevel; count++)
                {
                    bar[count] = '-';
                }

                uLCD.cls();
                uLCD.text_width(2); 

                uLCD.text_height(4);

                uLCD.color(RED);

                uLCD.locate(1,2);
                uLCD.printf(" %dhz\n%s",frequency[barLevel],bar);
            }
            
            else{
                uLCD.cls();
                barLevel--;
                uLCD.text_width(2); 

                uLCD.text_height(4);

                    if(barLevel<=2)
                        uLCD.color(RED);
                    else if(barLevel >=6)
                        uLCD.color(BLUE);
                    else
                        uLCD.color(GREEN);

                uLCD.locate(1,2);
                
                char bar[9] = "";
                for(int count = 0; count <= barLevel; count++)
                {
                    bar[count] = '-';
                }
                uLCD.printf(" %dhz\n%s",frequency[barLevel],bar);
                ThisThread::sleep_for(200ms);
            }          
            break;
    }

            if (whileBreaker == true){ 
                break;
            }
    
    }
    
    
    
    eventQueue.call(&sampling);
    Thread eventThread(osPriorityNormal);
    eventThread.start(callback(&eventQueue, &EventQueue::dispatch_forever));
    wave(frequency[barLevel]);
    
   
}
