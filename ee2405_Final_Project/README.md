# ee2405_Final_Project

# Project Design

My design is to let the camera detect the Blob with specific color threshold so that it follows it, until it detect that the blob is close to the car, then the car circles around it, and then after circling the can I will let the camera to track for april tag and then it will send message to let the car know how to go, and then it will eventually go back near to the original position.

# Reference Video Link

https://drive.google.com/drive/folders/1YulE2YT7O4fxFdFxLqOY2ywDIxclBJZh?usp=sharing


# Code part

## main.cpp

### presetting pins


These are for car controlling and for communication


```bash
BufferedSerial pc(USBTX,USBRX);
BufferedSerial uart(D10, D9); // tx, rx  D10:tx  D9:rx
BufferedSerial xbee(D1, D0);

Ticker servo_ticker;
PwmOut pin5(D5), pin6(D6);
BBCar car(pin5, pin6, servo_ticker);
```

### threads for UART and XBEE reading

these are threads to set buffer for xbee and uart communication

```bash
Thread XbeeThread(osPriorityHigh);
EventQueue XbeeEvent(32 * EVENTS_EVENT_SIZE);
void XbeeCom();

Thread Recieved_Uart_Thread(osPriorityNormal);
void received_thread_uart();
```

#### void XbeeCom

```bash
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
```


#### void received_thread_uart

```bash
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
 ```


### April tag tracking
These are some function predefined for apriltag tracking
```bash

DigitalInOut pin11(D11);
bool BoolApriltagTrack = false;

void ApriltagTrack_Toggle(Arguments *in, Reply *out);
RPCFunction rpc_ApriltagTrack_call(&ApriltagTrack_Toggle, "APT");

void ReadApriltagTrack(Arguments *in, Reply *out);
RPCFunction rpc_ReadApriltagTrack_call(&ReadApriltagTrack, "ReadApt")
```
#### ApriltagTrack_Toggle
 
 This function toggles a boolean function to enable/disable uart encoding
 ```bash
 void ApriltagTrack_Toggle(Arguments *in, Reply *out) {
   BoolApriltagTrack = !BoolApriltagTrack;
   char APTBuffer[30] ={0};
   sprintf(APTBuffer, "starting APTbtracking \r\n");
   xbee.write(APTBuffer, sizeof(APTBuffer));
}
 ```
#### ReadApriltagTrack
This reads uart encoding from the camera, if the received theta is larger than 5 degrees, then it will move the horizontal direction and then face the apriltag
and then while ping is larger than 20 it moves straight until it approches to 20, and then it turns around to face the mug again.

```bash
void ReadApriltagTrack(Arguments *in, Reply *out)
{
   int theta = in ->getArg<int>();
   int distance = in ->getArg<int>();
   int sleeptime = abs(1000*distance/16);

   if (BoolApriltagTrack == 1) {
      
      //BoolApriltagTrack =! BoolApriltagTrack;
      char RAPTbuffer [80] = {0};
      sprintf(RAPTbuffer, "theta is :%d x_distance is %d sleeptime is %d\r\n",theta, distance, sleeptime);
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
         ThisThread::sleep_for(380ms);
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
         
      }


         parallax_ping ping(pin11);
         
         while(1)
         {
           car.goStraight(50);
         
           if(float(ping) <=20)
           {
              break;
           }
         }
         
         car.stop();
         char pingbuff[30] = {0};
         sprintf(pingbuff ,"Ping = %.1f\r\n", float(ping));
         xbee.write(pingbuff, sizeof(pingbuff));

         ThisThread::sleep_for(100ms);

         car.turn(200,1);
         ThisThread::sleep_for(740ms);
         car.stop();
    
   }
}
```

### BlobCircling
```bash
  bool BoolBlobTracking = false;

void BlobTracking_Toggle(Arguments *in, Reply *out);
RPCFunction rpc_blobTrack_call(&BlobTracking_Toggle, "BlobT");

void ReadBlobTrack(Arguments *in, Reply *out);
RPCFunction rpc_ReadblobTrack_call(&ReadBlobTrack, "ReadBlobTrack");

void DoBlobCircling(Arguments *in, Reply *out);
RPCFunction rpc_DoblobTrack_call(&DoBlobCircling, "DoBlobCircling");
```
#### BlobTracking_Toggle
This works same as the previous toggle function
```bash

void BlobTracking_Toggle(Arguments *in, Reply *out) {
   BoolBlobTracking =! BoolBlobTracking;
   char buffer[30] = {0};
   sprintf(buffer, "starting Blobtracking \r\n");
   xbee.write(buffer, sizeof(buffer));
}

```


#### ReadBlobTrack
This reads encoding from camera to move straight

```bash
void ReadBlobTrack(Arguments *in, Reply *out)
{
   if(BoolBlobTracking == 1) {
      car.goStraight(40);
   }
}
```

#### DoBlobCircling
This reads encoding from camera to circle it
```bash
void DoBlobCircling(Arguments *in, Reply *out)
{
   if(BoolBlobTracking == 1) {
      
      
      char buffer[30] = {0};
      sprintf(buffer, "starting Blobcircling \r\n");
      xbee.write(buffer, sizeof(buffer));

      car.stop();
      ThisThread::sleep_for(500ms);

      car.turn(200,1);
      ThisThread::sleep_for(370ms);

      car.goStraight(100);
      ThisThread::sleep_for(1100ms);

      car.turn(200,-1);
      ThisThread::sleep_for(500ms);

      car.goStraight(100);
      ThisThread::sleep_for(1800ms);

      car.turn(200,-1);
      ThisThread::sleep_for(500ms);

      car.goStraight(100);
      ThisThread::sleep_for(2200ms);

      car.turn(200,-1);
      ThisThread::sleep_for(540ms);

      car.goStraight(100);
      ThisThread::sleep_for(2200ms);

      car.stop();
      BoolBlobTracking = !BoolBlobTracking;

      char buffer2[30] = {0};
      sprintf(buffer2, "ending Blobcircling \r\n");
      xbee.write(buffer2, sizeof(buffer));
      ThisThread::sleep_for(500ms);
   }
}
```

### main
```bash

int main() 
{
    pc.set_baud(9600);
    xbee.set_baud(9600);
    uart.set_baud(9600);
    XbeeEvent.call(&XbeeCom);
    XbeeThread.start(callback(&XbeeEvent, &EventQueue::dispatch_forever));
    Recieved_Uart_Thread.start(received_thread_uart);
}
```


## main.py (on the camera)

```bash
import pyb, sensor, image, time, math
enable_lens_corr = False
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)
BLUEMUG_threshold = (0, 82, -128, -8, -31, -5)
BLUEMUG2_threshold = (53, 24, -26, -14, -81, 25)
REDCAN_threshold = (0, 28, 127, -85, 15, 127)
BUFF_CONSTANT = 6.2
k = 1270
clock = time.clock()
FindLine = 0;
f_x = (2.8 / 3.984) * 160 # find_apriltags defaults to this if not set
f_y = (2.8 / 2.952) * 120 # find_apriltags defaults to this if not set
c_x = 160 * 0.5 # find_apriltags defaults to this if not set (the image.w * 0.5)
c_y = 120 * 0.5 # find_apriltags defaults to this if not set (the image.h * 0.5)




def degrees(radians):
    if(radians >= math.pi):
      return ((180 * radians) / math.pi - 360)
    else :
      return (180 * radians) / math.pi


uart = pyb.UART(3,9600,timeout_char=1000)
uart.init(9600,bits=8,parity = None, stop=1, timeout_char=1000)



while(True):
    FindLine = 0
    clock.tick()
    img = sensor.snapshot()
    if enable_lens_corr: img.lens_corr(1.8)
    blobs = img.find_blobs([REDCAN_threshold])






    if len(blobs) == 1:
      b = blobs[0]
      img.draw_rectangle(b[0:4])
      img.draw_cross(b[5],b[6])
      Lm = (b[2]+b[3])/2

      length = k/Lm
      print(length)
      uart.write("/ReadBlobTrack/run \n".encode())


      if(length < 15):
        print("hit")
        uart.write("/DoBlobCircling/run \n".encode())
        time.sleep(5)


        time.sleep(5)
    elif len(blobs) == 0:
        print("no")


    for tag in img.find_apriltags(fx=f_x, fy=f_y, cx=c_x, cy=c_y): # defaults to TAG36H11
          img.draw_rectangle(tag.rect(), color = (0, 0, 255))
          img.draw_cross(tag.cx(), tag.cy(), color = (0, 255, 0))

          REAL_Z_Distance = tag.z_translation() * 3.45
          REAL_X_Distance = tag.x_translation() * 3.07
          REAL_DISTANCE = math.sqrt(REAL_Z_Distance * REAL_Z_Distance + REAL_X_Distance * REAL_X_Distance)

          THETA = ( math.atan(tag.x_translation() / tag.z_translation()) )

          CALCULATED_X = REAL_DISTANCE * math.sin(THETA)

          theta =  ( 180 * math.atan(tag.x_translation() / tag.z_translation())/math.pi)

          position_args = (theta, CALCULATED_X)


          if (theta > 5 or theta < -5):
            uart.write(("/ReadApt/run %f %f\n" %position_args).encode())
            print(tag.x_translation())
            print("X by frame is : %f" %REAL_X_Distance)
            print("X by calculated is :%f" %CALCULATED_X)


          else:

            print("end")


