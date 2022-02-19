##  Exam1

# basic set up
using A0 as my analogIn pin.

using D7 as my analogOut pin.

using D12 D11 D10 as a serial imput.

and D1 D0 D2 as basic uLCD setting pin.

```bash
AnalogIn Ain(A0);
AnalogOut  aout(D7);
uLCD_4DGL uLCD(D1, D0, D2); 
BusIn Buttons(D12, D11, D10);
```
# setting variables
```bash
int barLevel  = 3;
int slew[4] = {8,4,2,1}; 
float ADCdata[1000];
Thread wavethread;
Thread eventThread;
EventQueue eventQueue(32 * EVENTS_EVENT_SIZE);
EventQueue wavequeue(32 * EVENTS_EVENT_SIZE);
```
barlevel as a controlling cursor variable of array

slew as int array to put different slew seelection

create to thread and two event queue to generate wave and sample

# sampling
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
sample rate as 1ms also means 1000hz.

# wave generartion
```bash
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

```
first decide the sleep time of the peak value then generate the different slew rate signal dominated by the barlevel cursor
# main function

first set up an default screen as slew rate 1, and then use the uLCD library to control the selection panel.
The switch function detect whether any pin is high(0x1,0x2,0x4)

Once the middle button(D11 = 0x2 is high) the while breaker boolean variables become true and then break out the while loop and then start the thread call.
```bash 
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
```

## FFT.py
setting sampling rate as 1000hz and then the rest are similar to the lab
```bash
import matplotlib.pyplot as plt

import numpy as np

import serial

import time


Fs = 5000.0;  # sampling rate

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
## Pyhton Plot & picoscope
## slew rate 1

![slew rate 1](https://user-images.githubusercontent.com/67352558/113844304-ebc7f100-97c6-11eb-9816-30fa93525023.png)


![1](https://user-images.githubusercontent.com/67352558/113845119-be2f7780-97c7-11eb-8755-d2d69190e993.jpg)

## slew rate1/2

![slew rate 0 5](https://user-images.githubusercontent.com/67352558/113844500-203bad00-97c7-11eb-9cc8-f78b25cb6c3f.png)

![0 5](https://user-images.githubusercontent.com/67352558/113845148-c5568580-97c7-11eb-988c-2df61485d3f3.png)


## slew rate 1/4

![slur rate 0 25](https://user-images.githubusercontent.com/67352558/113844535-2af64200-97c7-11eb-80f3-7d8e602411c5.png)


![0 25](https://user-images.githubusercontent.com/67352558/113845168-cbe4fd00-97c7-11eb-960e-ef2cf6810311.jpg)

## slew rate 1/8

![slew rate0 125](https://user-images.githubusercontent.com/67352558/113844541-2cc00580-97c7-11eb-9008-e4b022ce793b.png)

![0 125](https://user-images.githubusercontent.com/67352558/113845207-d7d0bf00-97c7-11eb-991f-4616b0473399.jpg)

## circuit and breadboard

![S__40361986](https://user-images.githubusercontent.com/67352558/113849179-ce495600-97cb-11eb-97b7-7c00e7c489d0.jpg)
![S__40361988](https://user-images.githubusercontent.com/67352558/113849185-d0131980-97cb-11eb-8604-12cbb4fc9bbd.jpg)
![S__40361989](https://user-images.githubusercontent.com/67352558/113849198-d3a6a080-97cb-11eb-9551-ab8c9943c683.jpg)
![S__40361990](https://user-images.githubusercontent.com/67352558/113849208-d608fa80-97cb-11eb-960b-a1b81881b8e7.jpg)
![S__40361991](https://user-images.githubusercontent.com/67352558/113849218-d86b5480-97cb-11eb-9a17-9815f9216212.jpg)


