#include "mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"

/*simple variable*/
Ticker servo_ticker;
Thread LTthread;


PwmOut pin5(D5), pin6(D6);
BufferedSerial pc(USBTX,USBRX);
BufferedSerial uart(D10, D9); // tx, rx  D10:tx  D9:rx
BufferedSerial xbee(D1, D0);  // tx, rx  D1:tx   D0:rx
BBCar car(pin5, pin6, servo_ticker);
//DigitalOut LineTraceLED (LED2);


/*********lineTracing RPC**********/
void lineTracingToggle(Arguments *in, Reply *out);
RPCFunction rpc_lineTracing_call(&lineTracingToggle, "linetrace");
void lineTrace();
bool BoolLineTrace = true;





int main() {

   uart.set_baud(9600);
   LTthread.start(lineTrace);

   while(1)
   {
     char buf[256], outbuf[256];
      FILE *devin = fdopen(&xbee, "r");
      FILE *devout = fdopen(&xbee, "w");

      /*xBee reading*/
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
}


/*sends RPC command to toggle lineTrace mode through Xbee*/
void lineTracingToggle(Arguments *in, Reply *out) {
   BoolLineTrace = !BoolLineTrace;
}


/*activated when BoolLineTrace is true*/
void lineTrace() {
   char buf[256], outbuf[256];
   while (1)
   {
      
      if(BoolLineTrace == 1) {
         memset(buf, 0, 256);
            for(int i = 0;; i++) {
            char *ReceivedBuffer = new char[1];
            uart.read(ReceivedBuffer, 1);
            buf[i] = *ReceivedBuffer;

            if (*ReceivedBuffer == '\n') {
               break;
            }
         }
            //printf("%s\r\n", buf);
            RPC::call(buf, outbuf);
      }

      else if(BoolLineTrace ==0) {
         car.stop();
      }
   }
}
   





