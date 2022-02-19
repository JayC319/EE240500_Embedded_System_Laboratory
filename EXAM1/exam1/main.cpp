#include "mbed.h"

#include "uLCD_4DGL.h"
#include<iostream>
using namespace std::chrono;
using namespace std;

AnalogIn Ain(A0);
AnalogOut  aout(D7);
DigitalOut led1 (LED1);
uLCD_4DGL uLCD(D1, D0, D2); 
BusIn Buttons(D12, D11, D10);
//int frequency[9] = {5,10,30,40,50,100,150,200,250};

int barLevel  = 3;
int slew[4] = {8,4,2,1}; 
float ADCdata[1000];
Thread wavethread;
Thread eventThread;
EventQueue eventQueue(32 * EVENTS_EVENT_SIZE);
EventQueue wavequeue(32 * EVENTS_EVENT_SIZE);

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

void wave()
{

    int sleep_time = 0;

    if(barLevel == 3)
    {
        sleep_time = 80;
    }

    else if(barLevel == 2)
    {
        sleep_time = 160;
    }

    else if (barLevel == 1)
    {
        sleep_time = 200;
    }

    else{
        sleep_time = 220;
    }
      float increment = (0.01125f * slew[barLevel]);
      
      while (1){
        for(float i = 0.0f ; i < 0.9f; i += increment) {
                  aout = i ;
                  ThisThread::sleep_for(1ms);            
      }


        aout = 0.9f;
      ThisThread::sleep_for(sleep_time*1ms);

            for (float i = 0.9f ; i > 0.0f; i -= increment) {    
                  aout = i;
                  ThisThread::sleep_for(1ms);          
            } 
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

    uLCD.printf(" %d\n----",slew[3]);


bool whileBreaker = false;

while (1){
    
        switch(Buttons){
        case 0x4:
            if(barLevel >= 3)
            {
                uLCD.cls();
                uLCD.text_width(2); 

                uLCD.text_height(4);

                uLCD.color(WHITE);

                uLCD.locate(1,2);
                uLCD.printf(" N\\A");
                 ThisThread::sleep_for(50ms);
                char bar[4] = "";
                 for(int count = 0; count <= barLevel; count++)
                    {
                     bar[count] = '-';
                    }
                uLCD.cls();
                uLCD.text_width(2); 

                uLCD.text_height(4);

                uLCD.color(BLUE);

                uLCD.locate(1,2);
                
                uLCD.printf("  %d\n%s ",slew[barLevel],bar);

                
            }
                    
            else{
                uLCD.cls();
                barLevel++;
                uLCD.text_width(2); 

                uLCD.text_height(4);

                /*if(barLevel >= 6)
                    uLCD.color(BLUE);
                else if(barLevel <= 2)
                    uLCD.color(RED);
                else
                    uLCD.color(GREEN);*/

                uLCD.locate(1,2);

                char bar[4] = "";
                for(int count = 0; count <= barLevel; count++)
                {
                    bar[count] = '-';
                }
                if(barLevel != 3)
                    uLCD.printf(" 1/%d\n%s",slew[barLevel],bar);
                else
                    uLCD.printf(" %d\n%s",slew[barLevel],bar);
                    
                    ThisThread::sleep_for(200ms);
            }        
            break;

        case 0x2:
            uLCD.cls();
            uLCD.locate(1,1);
            if(barLevel != 3 )
                uLCD.printf("You select \n\nslew rate\n\n1/%d",slew[barLevel]);
            else
                uLCD.printf("You select \n\nslew rate\n\n%d",slew[barLevel]);
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

                char bar[4] = "";
                for(int count = 0; count <= barLevel; count++)
                {
                    bar[count] = '-';
                }

                uLCD.cls();
                uLCD.text_width(2); 

                uLCD.text_height(4);

                uLCD.color(RED);

                uLCD.locate(1,2);
                uLCD.printf(" 1/%d\n%s",slew[barLevel],bar);
            }
            
            else{
                uLCD.cls();
                barLevel--;
                uLCD.text_width(2); 

                uLCD.text_height(4);

                    /*if(barLevel<=2)
                        uLCD.color(RED);
                    else if(barLevel >=6)
                        uLCD.color(BLUE);
                    else
                        uLCD.color(GREEN);*/

                uLCD.locate(1,2);
                
                char bar[4] = "";
                for(int count = 0; count <= barLevel; count++)
                {
                    bar[count] = '-';
                }
                uLCD.printf(" 1/%d\n%s",slew[barLevel],bar);
                ThisThread::sleep_for(200ms);
            }          
            break;
    }

            if (whileBreaker == true){ 
                break;
            }
    
    }
    
    
    
    
    wavethread.start(callback(&wavequeue, &EventQueue::dispatch_forever));
    eventThread.start(callback(&eventQueue, &EventQueue::dispatch_forever));
    eventQueue.call(&sampling);
    wavequeue.call(&wave);
    
   
}