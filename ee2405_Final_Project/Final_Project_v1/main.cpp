#include "mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"



/**********serial communication**********/
BufferedSerial pc(USBTX,USBRX);
BufferedSerial uart(D10, D9); // tx, rx  D10:tx  D9:rx
BufferedSerial xbee(D1, D0);  // tx, rx  D1:tx   D0:rx


/**********car_servo control**********/
Ticker servo_ticker;
PwmOut pin5(D5), pin6(D6);
BBCar car(pin5, pin6, servo_ticker);

/**********Threads_&_Eventqueues**********/
Thread LineTraceThread;
Thread ParkingThread;
Thread AprilTagThread;
EventQueue LineTrace_Queue(32 * EVENTS_EVENT_SIZE);
EventQueue Parking_Queue(32 * EVENTS_EVENT_SIZE);
EventQueue AprilTag_Queue(32 * EVENTS_EVENT_SIZE);


/******functions for reading from serial port*****/
Thread XbeeThread;
EventQueue XbeeEvent(32 * EVENTS_EVENT_SIZE);
void XbeeCom();

Thread Recieved_Uart_Thread;
void received_thread_uart();




/***************************************/
/*       for line trace             */
/***************************************/

bool BoolLineTrace = false;
void lineTracingToggle(Arguments *in, Reply *out);
RPCFunction rpc_lineTracing_call(&lineTracingToggle, "linetrace");



/***************************************/
/*       for Apriltag tracking         */
/***************************************/

bool BoolApriltagTrack = false;
void ApriltagTrack_Toggle(Arguments *in, Reply *out);
RPCFunction rpc_ApriltagTrack_call(&ApriltagTrack_Toggle, "APT");



/***************************************/
/*       for blob_circling             */
/***************************************/
bool BoolBlobTracking = false;

// The command will break by it self in py, no need to terminate it by ownself
void do_BlobTracking(Arguments *in, Reply *out);
RPCFunction rpc_blobTrack_call(&do_BlobTracking, "BlobT");


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


void lineTracingToggle(Arguments *in, Reply *out) {
   BoolLineTrace = !BoolLineTrace;
   if(BoolLineTrace == 1)
   {
      char buffer[20] = {};
      sprintf(buffer, "Linetrace: %d\r\n", BoolLineTrace);
      pc.write(buffer, sizeof(buffer));
      ThisThread::sleep_for(10ms);
   }

   else if (BoolLineTrace == 0)
   {
      char buffer[20] = {};
      sprintf(buffer, "Linetrace: %d\r\n", BoolLineTrace);
      pc.write(buffer, sizeof(buffer));
      ThisThread::sleep_for(10ms);
   }
}

void ApriltagTrack_Toggle(Arguments *in, Reply *out) {
////
}

void do_BlobTracking(Arguments *in, Reply *out) {

}
