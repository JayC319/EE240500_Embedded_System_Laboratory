#include "mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"


Ticker servo_ticker;
Thread APTthread;

Timer t;

PwmOut pin5(D5), pin6(D6);

BufferedSerial pc(USBTX,USBRX);
BufferedSerial uart(D10, D9); // tx, rx  D10:tx  D9:rx
BufferedSerial xbee(D1, D0);  // tx, rx  D1:tx   D0:rx
BBCar car(pin5, pin6, servo_ticker);



/*AprilTagTrace*/
void AprilTagToggle(Arguments *in, Reply *out);
void AprilTagFind(Arguments *in, Reply *out);
RPCFunction rpc_AprilTagTrace_call(&AprilTagToggle, "AprilTag");
void AprilTagTrace();
bool BoolAprilTagTrace = true;


/*ping*/

void pingRespond(Arguments *in, Reply *out);
DigitalInOut ping(D11);
RPCFunction ping_reponse(&pingRespond, "ping");
float val = 0.0;




int main() {
   pc.set_baud(9600);
   uart.set_baud(9600);
   APTthread.start(AprilTagTrace);
   
   char buf[256], outbuf[256];
   while (1)
   {
      if(BoolAprilTagTrace == 1) {
         
         memset(buf, 0, 256);
            for(int i = 0;; i++) {
            char *ReceivedBuffer = new char[1];
            uart.read(ReceivedBuffer, 1);
            buf[i] = *ReceivedBuffer;

            if (*ReceivedBuffer == '\n') {
               break;
            }
         }
      

            RPC::call(buf, outbuf);
      }

       else if(BoolAprilTagTrace == 0) {
         car.stop();
      }
      
   }
}

void AprilTagToggle(Arguments *in, Reply *out) {
   BoolAprilTagTrace = !BoolAprilTagTrace;
}



void pingRespond(Arguments *in, Reply *out) {
  

      ping.output();
      ping = 0; wait_us(200);
      ping = 1; wait_us(5);
      ping = 0; wait_us(5);

      ping.input();
      while(ping.read() == 0);
      t.start();
      while(ping.read() == 1);
      val = t.read();
      printf("Ping = %lf\r\n", val*17700.4f);
      t.stop();
      t.reset();

      ThisThread::sleep_for(1s);
   
}



void AprilTagTrace()
{
  
}

void AprilTagFind(Arguments *in, Reply *out){
   
}
