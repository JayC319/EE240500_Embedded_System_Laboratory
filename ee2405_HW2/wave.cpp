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