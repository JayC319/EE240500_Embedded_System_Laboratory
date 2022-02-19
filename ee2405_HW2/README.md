# ee2405_HW2

## setup

This project requires 3 buttons as analog input to control the frequency, select D12,D11,D10 as analog input port, using BusIn to put the imput as a series.
```bash
BusIn Buttons(D12, D11, D10);
```
Create array of frequency selections size of 9
```bash
int frequency[9] = {5,10,30,40,50,100,150,200,250};
```
Create Eventqueue for later use
```bash
EventQueue eventQueue;
```

## main.cpp
Set up the default selection on the uLCD
```bash
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

```
set the cursor at the seventh element of the selection array and a while loop breaker boolean function
```bash
int barLevel  = 6;
bool whileBreaker = false;
```
Using while loop to detect whether the analog input is high or not
case 0x1 (001) pin D12 high (lower)
case 0x2 (010) pin D11 high (select)
case 0x4 (100) pin D10 high (higher)
and set up the selection interface through functions like locate or printf, cls,...etc
if the boolean variable is set to true, then break the while loop.
```bash
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
```
After breaking the loop use eventQueue and thread to call the sampling function periodically and then start the wave function.
```bash
eventQueue.call(&sampling);
    Thread eventThread(osPriorityNormal);
    eventThread.start(callback(&eventQueue, &EventQueue::dispatch_forever));
    wave(frequency[barLevel]);
```
## wave.h & wave.cpp
wave function to generate wave that hit the peak voltage in it's 1/10 th period and then gradually decrease. 
```bash
#include "wave.h"

AnalogOut  aout(PA_4);

void wave(int freq)
{

      float increment = (0.001f * freq);
      
      while (1){
            for(float i = 0.0f ; i < 0.9f; i += 9*increment) {
                  aout = i ;
                  ThisThread::sleep_for(1ms);            
      }

            for (float i = 0.9f ; i > 0.0f; i -= increment) {    
                  aout = i;
                  ThisThread::sleep_for(1ms);          
            } 
       } 
}
```
## void sampling()
The sampling function puts analog input value in to the array every 1ms (1000hz sample rate), and then print out the result after storing the input.
```bash
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
```
## FFT.py
Adjusting the sample rate as 1000, and let python convert string on screen to float and then make the graph.
```bash
import matplotlib.pyplot as plt

import numpy as np

import serial

import time


Fs = 1000.0;  # sampling rate

Ts = 1/Fs; # sampling interval

t = np.arange(0,1,Ts) # time vector; create Fs samples between 0 and 1.0 sec.

y = np.arange(0,1,Ts) # signal vector; create Fs samples


n = len(y) # length of the signal

k = np.arange(n)

T = n/Fs

frq = k/T # a vector of frequencies; two sides frequency range

frq = frq[range(int(n/2))] # one side frequency range


serdev = '/dev/ttyACM0'

s = serial.Serial(serdev)

for x in range(0, int(Fs)):

    line=s.readline() # Read an echo string from B_L4S5I_IOT01A terminated with '\n'

    # print line

    y[x] = float(line)


Y = np.fft.fft(y)/n*2 # fft computing and normalization

Y = Y[range(int(n/2))] # remove the conjugate frequency parts


fig, ax = plt.subplots(2, 1)

ax[0].plot(t,y)

ax[0].set_xlabel('Time')

ax[0].set_ylabel('Amplitude')

ax[1].plot(frq,abs(Y),'r') # plotting the spectrum

ax[1].set_xlabel('Freq (Hz)')

ax[1].set_ylabel('|Y(freq)|')

plt.show()

s.close()
```
## Result (150 hz)
Wave before filtering
![150hz tue before_RC](https://user-images.githubusercontent.com/67352558/113005987-8a40ca80-91a7-11eb-8aa6-f61bd2358182.png)
Wave after filtering
![150h tue](https://user-images.githubusercontent.com/67352558/113006065-99277d00-91a7-11eb-92ab-4788dee37543.png)



