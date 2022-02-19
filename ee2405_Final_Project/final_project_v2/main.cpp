#include "mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"
#include <cmath>


/**********serial communication**********/
BufferedSerial pc(USBTX,USBRX);
BufferedSerial uart(D10, D9); // tx, rx  D10:tx  D9:rx
BufferedSerial xbee(D1, D0);  // tx, rx  D1:tx   D0:rx


/**********car_servo control**********/
Ticker servo_ticker;
PwmOut pin5(D5), pin6(D6);
BBCar car(pin5, pin6, servo_ticker);



/**********Threads_&_Eventqueues**********/
/*
Thread LineTraceThread;
Thread ParkingThread;
Thread AprilTagThread;
EventQueue LineTrace_Queue(32 * EVENTS_EVENT_SIZE);
EventQueue Parking_Queue(32 * EVENTS_EVENT_SIZE);
EventQueue AprilTag_Queue(32 * EVENTS_EVENT_SIZE);
*/


/******functions for reading from serial port*****/
Thread XbeeThread(osPriorityHigh);
EventQueue XbeeEvent(32 * EVENTS_EVENT_SIZE);
void XbeeCom();

Thread Recieved_Uart_Thread(osPriorityNormal);
void received_thread_uart();




/***************************************/
/*       for line trace                */
/***************************************/
/*
bool BoolLineTrace = false;
void lineTracingToggle(Arguments *in, Reply *out);
RPCFunction rpc_lineTracing_call(&lineTracingToggle, "linetrace");
void ReadLineTrace(Arguments *in, Reply *out);
RPCFunction rpc_ReadLineTrace_call(&ReadLineTrace, "ReadLineTrace");
*/



/***************************************/
/*       for Apriltag tracking         */
/***************************************/
DigitalInOut pin11(D11);
bool BoolApriltagTrack = false;

void ApriltagTrack_Toggle(Arguments *in, Reply *out);
RPCFunction rpc_ApriltagTrack_call(&ApriltagTrack_Toggle, "APT");

void ReadApriltagTrack(Arguments *in, Reply *out);
RPCFunction rpc_ReadApriltagTrack_call(&ReadApriltagTrack, "ReadApt");




/***************************************/
/*       for blob_circling             */
/***************************************/
bool BoolBlobTracking = false;

void BlobTracking_Toggle(Arguments *in, Reply *out);
RPCFunction rpc_blobTrack_call(&BlobTracking_Toggle, "BlobT");

void ReadBlobTrack(Arguments *in, Reply *out);
RPCFunction rpc_ReadblobTrack_call(&ReadBlobTrack, "ReadBlobTrack");

void DoBlobCircling(Arguments *in, Reply *out);
RPCFunction rpc_DoblobTrack_call(&DoBlobCircling, "DoBlobCircling");






/*main*/
///////////////////////////////////////////////////////////////////////////

int main() 
{
    pc.set_baud(9600);
    xbee.set_baud(9600);
    uart.set_baud(9600);
    XbeeEvent.call(&XbeeCom);
    XbeeThread.start(callback(&XbeeEvent, &EventQueue::dispatch_forever));
    Recieved_Uart_Thread.start(received_thread_uart);
}




/*functions definition*/
//////////////////////////////////////////////////////////////////////////




/***************************************/
/*      Xbee Comunication              */
/***************************************/

void XbeeCom() 
{
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



/***************************************/
/*       UART communication            */
/***************************************/



void received_thread_uart()
{    
   char buf[256], outbuf[256];
   while (1)
   {
        memset(buf, 0, 256);
        
        for(int i = 0;; i++){
        char *ReceivedBuffer = new char[1];
        uart.read(ReceivedBuffer, 1);
        buf[i] = *ReceivedBuffer;

        if (*ReceivedBuffer == '\n') {
                printf("\r\n");
                break;
            }
         }        
            RPC::call(buf, outbuf);
    }
 }


/*******************************************/
/*       LINE TRACING                      */
/* This part is removed due to camera issue*/
/*******************************************/
/*
void lineTracingToggle(Arguments *in, Reply *out) {
   BoolLineTrace = !BoolLineTrace;
  
}


void ReadLineTrace(Arguments *in, Reply *out)
{
   int condition = in->getArg<int>();
   
   if(BoolLineTrace == 1) {
      if(condition == 1)
      {
         car.goStraight(60);
      }
      else if(condition == 0)
      {
         car.stop();
      }   
   } 
   
}*/



/***************************************/
/*       APRILTAG TRACK                */
/***************************************/

void ApriltagTrack_Toggle(Arguments *in, Reply *out) {
   BoolApriltagTrack = !BoolApriltagTrack;
   char APTBuffer[30] ={0};
   sprintf(APTBuffer, "starting APTbtracking \r\n");
   xbee.write(APTBuffer, sizeof(APTBuffer));
}

void ReadApriltagTrack(Arguments *in, Reply *out)
{
   int theta = in ->getArg<int>();
   int distance = in ->getArg<int>();
   int sleeptime = abs(1000*distance/16);

   if (BoolApriltagTrack == 1) {
      
      //BoolApriltagTrack =! BoolApriltagTrack;
      char RAPTbuffer [80] = {0};
      sprintf(RAPTbuffer, "theta is :%d x_distance is %d \r\n",theta, distance);
      xbee.write(RAPTbuffer, sizeof(RAPTbuffer));

      BoolApriltagTrack =! BoolApriltagTrack;
      if (theta > 5)
      {
         car.turn(200,1);
         ThisThread::sleep_for(360ms);
         car.stop();
         ThisThread::sleep_for(200ms);
         car.goStraight(100);
         ThisThread::sleep_for(sleeptime);
         car.stop();
         ThisThread::sleep_for(500ms);
         car.turn(200,-1);
         ThisThread::sleep_for(430ms);
         car.stop();
         
         
      }

      else if (theta <-5)
      {
         car.turn(200,-1);
         ThisThread::sleep_for(340ms);
         car.stop();
         ThisThread::sleep_for(200ms);
         car.goStraight(100);
         ThisThread::sleep_for(sleeptime);
         car.stop();
         ThisThread::sleep_for(500ms);
         car.turn(200,1);
         ThisThread::sleep_for(420ms);
         car.stop();
         ThisThread::sleep_for(2000ms);
         
      }


         parallax_ping ping(pin11);
         
         while(float(ping) > 29)
         {
           char bufferP[20] = {0};
           sprintf(bufferP, "Ping = %3.1f\r\n", float(ping));
           xbee.write(bufferP, sizeof(bufferP));
           car.goStraight(50);
           ThisThread::sleep_for(100ms);
         }
         
         car.stop();
         char pingbuff[40] = {0};
         sprintf(pingbuff ,"Start to rotate back.\r\n");
         xbee.write(pingbuff, sizeof(pingbuff));

         ThisThread::sleep_for(100ms);

         car.turn(200,1);
         ThisThread::sleep_for(690ms);
         car.stop();

         char endingbuff[40] = {0};
         sprintf(endingbuff ,"End of all task, back to origin");
         xbee.write(endingbuff, sizeof(endingbuff));
      
   }
}

/***************************************/
/*       BLOB TRACKING                 */
/* This part is customized for blue mug*/
/***************************************/

void BlobTracking_Toggle(Arguments *in, Reply *out) {
   BoolBlobTracking =! BoolBlobTracking;
   char buffer[30] = {0};
   sprintf(buffer, "starting Blobtracking \r\n");
   xbee.write(buffer, sizeof(buffer));
}


void ReadBlobTrack(Arguments *in, Reply *out)
{
   if(BoolBlobTracking == 1) {
      car.goStraight(40);
   }
}


void DoBlobCircling(Arguments *in, Reply *out)
{
   if(BoolBlobTracking == 1) {
      
      BoolBlobTracking = !BoolBlobTracking;
      char buffer[30] = {0};
      sprintf(buffer, "starting Blobcircling \r\n");
      xbee.write(buffer, sizeof(buffer));

      car.stop();
      ThisThread::sleep_for(500ms);

      car.turn(200,1);
      ThisThread::sleep_for(380ms);

      car.goStraight(100);
      ThisThread::sleep_for(1200ms);

      car.turn(200,-1);
      ThisThread::sleep_for(500ms);

      car.goStraight(100);
      ThisThread::sleep_for(1800ms);

      car.turn(200,-1);
      ThisThread::sleep_for(500ms);

      car.goStraight(100);
      ThisThread::sleep_for(2100ms);

      car.turn(200,-1);
      ThisThread::sleep_for(540ms);

      car.goStraight(100);
      ThisThread::sleep_for(2000ms);

      car.stop();
      

      char buffer2[30] = {0};
      sprintf(buffer2, "ending Blobcircling \r\n");
      xbee.write(buffer2, sizeof(buffer));
      ThisThread::sleep_for(500ms);

      char APTBuff[30] ={0};
      sprintf(APTBuff, "starting APTbtracking \r\n");
      xbee.write(APTBuff, sizeof(APTBuff));
      BoolApriltagTrack = !BoolApriltagTrack;

   }
}