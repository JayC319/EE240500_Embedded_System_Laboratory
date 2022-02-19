#include "mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"


Ticker servo_ticker;

PwmOut pin5(D5), pin6(D6);
BufferedSerial xbee(D1, D0);
BBCar car(pin5, pin6, servo_ticker);


/*********parking RPC**********/
void reverse_parking(Arguments *in, Reply *out);
RPCFunction rpc_ReversePark_call(&reverse_parking, "park");






int main() {

   char buf[256], outbuf[256];
   FILE *devin = fdopen(&xbee, "r");
   FILE *devout = fdopen(&xbee, "w");
   while (1) {
      memset(buf, 0, 256);
      for( int i = 0; ; i++ ) {
         char recv = fgetc(devin);
         if(recv == '\n') {
            printf("\r\n");
            break;
         }
         buf[i] = fputc(recv, devout);
      }
   RPC::call(buf, outbuf);

   }

}


void reverse_parking(Arguments *in, Reply *out) {
   char position = in->getArg<char>();
   int vertical_length = in->getArg<int>();
   int horizontal_length = in->getArg<int>();

   char buffer[200], outbuf[256], strings[200];

   if(position == 'W') {
      /*move back*/

      car.goStraight(100);
      int sleeptime = 58.8 * horizontal_length;
      ThisThread::sleep_for(sleeptime);
      car.stop();
      
      ThisThread::sleep_for(10ms);

      /*turn*/
      car.turn(95,0.2);
      ThisThread::sleep_for(1750ms);
      car.stop();
      
      /*move back */
      car.goStraight(100);
      int sleeptime2 = 62.5 * (vertical_length +8);
      ThisThread::sleep_for(sleeptime2);
      car.stop();
      ThisThread::sleep_for(10ms);



   }

   else if(position == 'E') {
      /*move back*/
      car.goStraight(100);
      int sleeptime = 62.5 * horizontal_length;
      ThisThread::sleep_for(sleeptime);
      car.stop();
      ThisThread::sleep_for(50ms);

      /*turn*/
      car.turn(90,-0.2);
      ThisThread::sleep_for(1500ms);
      car.stop();
      
      /*move back */
      car.goStraight(100);
      int sleeptime2 = 62.5 * (vertical_length + 8);
      ThisThread::sleep_for(sleeptime2);
      car.stop();
      ThisThread::sleep_for(10ms);

   


   }
    else if(position == 'F') {
       car.goStraight(100);
       ThisThread::sleep_for(4000ms);
       car.stop();
    }

    else if(position == 'B') {
        car.goStraight(-100);
        ThisThread::sleep_for(1000ms);
        car.stop();
    }

    else if (position == 'L') {

       car.turn90(200, -1);
      
       ThisThread::sleep_for(375);
       car.stop();
       
    }

    else if(position == 'R') {
       car.turn90(200, 1);
       ThisThread::sleep_for(375);
       car.stop();
    }

    

}
