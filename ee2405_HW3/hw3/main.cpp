#include "mbed.h"
#include "mbed_rpc.h"
#include "mbed_events.h"

/*MQTT header*/
/*********************************/
#include "MQTTNetwork.h"
#include "MQTTmbed.h"
#include "MQTTClient.h"
/*********************************/


/*headers from lab8 5.5*/
/*******************************************************************/
#include "accelerometer_handler.h"
#include "config.h"
#include "magic_wand_model_data.h"
#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
/****************************************************************/

#include "uLCD_4DGL.h"
#include "stm32l475e_iot01_accelero.h"
#include <cmath>
#include <iostream>

using namespace std::chrono;
using namespace std;

/*global variable*/
/**********************************************/
DigitalOut test_LED(LED3);
InterruptIn btn2(USER_BUTTON);
constexpr int kTensorArenaSize = 60 * 1024;
uint8_t tensor_arena[kTensorArenaSize];
int angle_list[5] = {30, 35, 40, 45, 50};
int angle_index;
/*********************************************/



/*uLCD setup*/
/**************************************************/
uLCD_4DGL uLCD(D1, D0, D2);
void uLCD_print(int angle);
/**************************************************/



/*int predict gesture*/
/*************************************************/
int PredictGesture(float* output) {
  // How many times the most recent gesture has been matched in a row
  static int continuous_count = 0;
  // The result of the last prediction
  static int last_predict = -1;
  // Find whichever output has a probability > 0.8 (they sum to 1)
  int this_predict = -1;
  for (int i = 0; i < label_num; i++) {

    if (output[i] > 0.8) this_predict = i;

  }
  // No gesture was detected above the threshold

  if (this_predict == -1) {
    continuous_count = 0;
    last_predict = label_num;
    return label_num;
  }

  if (last_predict == this_predict) {
    continuous_count += 1;
  } else {
    continuous_count = 0;
  }

  last_predict = this_predict;

  // If we haven't yet had enough consecutive matches for this gesture,
  // report a negative result
  if (continuous_count < config.consecutiveInferenceThresholds[this_predict]) {
    return label_num;
  }

  // Otherwise, we've seen a positive result, so clear all our variables

  // and report it
  continuous_count = 0;
  last_predict = -1;
  return this_predict;
}
/**********************************************************************/



/*MQTT global variables*/
/***********************************/
WiFiInterface *wifi;
volatile int message_num = 0;
volatile int arrivedcount = 0;
volatile bool closed = false;
char* topic1 = "Mbed1";
char* topic2 = "Mbed2";
char* topic;
Thread mqtt_thread(osPriorityHigh);
EventQueue mqtt_queue;
/*********************************/



/*MQTT function*/
/**********************************/
void messageArrived(MQTT::MessageData& md);
void publish_message(MQTT::Client<MQTTNetwork, Countdown>* client) ;
void close_mqtt();
/***********************************************************/



/*MODE CONTROL*/
/*************************************************/
DigitalOut gestureLED(LED1);
DigitalOut tiltLED(LED2);
EventQueue Tilt_queue(32 * EVENTS_EVENT_SIZE);
EventQueue Gesture_queue(32 * EVENTS_EVENT_SIZE);
Thread Tilt_Thread;
Thread Gesture_Thread;
void Gesture_mode();
void Tilt_mode();
/**************************************************/



/* RPC*/
/***************************************************/
RpcDigitalOut myled1(LED1,"myled1");
RpcDigitalOut myled2(LED2,"myled2");
RpcDigitalOut myled3(LED3,"myled3");
BufferedSerial pc(USBTX, USBRX);
void LEDControl(Arguments *in, Reply *out);
RPCFunction rpcLED(&LEDControl, "LEDControl");
double x, y;
/**************************************************/







int main() {
    BSP_ACCELERO_Init();
    Tilt_queue.call(&Tilt_mode);
    Gesture_queue.call(&Gesture_mode);
    Tilt_Thread.start(callback(&Tilt_queue, &EventQueue::dispatch_forever));
    Gesture_Thread.start(callback(&Gesture_queue, &EventQueue::dispatch_forever));
    
   
    char buf[256], outbuf[256];

    FILE *devin = fdopen(&pc, "r");
    FILE *devout = fdopen(&pc, "w");

    while(1) {
        memset(buf, 0, 256);
        for (int i = 0; ; i++) {
            char recv = fgetc(devin);
            if (recv == '\n') {
                printf("\r\n");
                break;
            }
            buf[i] = fputc(recv, devout);
        }
        //Call the static call method on the RPC class
        RPC::call(buf, outbuf);
        printf("%s\r\n", outbuf);
    }
}



/*LEDcontrol*/
/***********************************************************/
void LEDControl (Arguments *in, Reply *out)   {
    bool success = true;
    // In this scenario, when using RPC delimit the two arguments with a space.
    x = in->getArg<double>();
    y = in->getArg<double>();

    // Have code here to call another RPC function to wake up specific led or close it.
    char buffer[200], outbuf[256];
    char strings[20];
    int led = x;
    int on = y;
    sprintf(strings, "/myled%d/write %d", led, on);
    strcpy(buffer, strings);
    RPC::call(buffer, outbuf);
    if (success) {
        out->putData(buffer);
    } else {
        out->putData("Failed to execute LED control.");
    }
}
/*******************************************************/



/*void gesture_mode*/
/************************************************************/
void Gesture_mode()
{
    while(1){
            if(gestureLED == 1 && tiltLED == 0)
        {
  // Whether we should clear the buffer next time we fetch data

  bool should_clear_buffer = false;
  bool got_data = false;
  // The gesture index of the prediction

  int gesture_index;


  // Set up logging.

  static tflite::MicroErrorReporter micro_error_reporter;
  tflite::ErrorReporter* error_reporter = &micro_error_reporter;


  // Map the model into a usable data structure. This doesn't involve any

  // copying or parsing, it's a very lightweight operation.

  const tflite::Model* model = tflite::GetModel(g_magic_wand_model_data);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    error_reporter->Report(
        "Model provided is schema version %d not equal "
        "to supported version %d.",
        model->version(), TFLITE_SCHEMA_VERSION);
    return ;

  }


  // Pull in only the operation implementations we need.

  // This relies on a complete list of all the ops needed by this graph.

  // An easier approach is to just use the AllOpsResolver, but this will

  // incur some penalty in code space for op implementations that are not

  // needed by this graph.

  static tflite::MicroOpResolver<6> micro_op_resolver;
  micro_op_resolver.AddBuiltin(
      tflite::BuiltinOperator_DEPTHWISE_CONV_2D,
      tflite::ops::micro::Register_DEPTHWISE_CONV_2D());
  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_MAX_POOL_2D,
                               tflite::ops::micro::Register_MAX_POOL_2D());
  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_CONV_2D,
                               tflite::ops::micro::Register_CONV_2D());
  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_FULLY_CONNECTED,
                               tflite::ops::micro::Register_FULLY_CONNECTED());
  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_SOFTMAX,
                               tflite::ops::micro::Register_SOFTMAX());
  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_RESHAPE,
                               tflite::ops::micro::Register_RESHAPE(), 1);


  // Build an interpreter to run the model with

  static tflite::MicroInterpreter static_interpreter(
      model, micro_op_resolver, tensor_arena, kTensorArenaSize, error_reporter);
  tflite::MicroInterpreter* interpreter = &static_interpreter;


  // Allocate memory from the tensor_arena for the model's tensors

  interpreter->AllocateTensors();


  // Obtain pointer to the model's input tensor

  TfLiteTensor* model_input = interpreter->input(0);

  if ((model_input->dims->size != 4) || (model_input->dims->data[0] != 1) ||
      (model_input->dims->data[1] != config.seq_length) ||
      (model_input->dims->data[2] != kChannelNumber) ||
      (model_input->type != kTfLiteFloat32)) {
    error_reporter->Report("Bad input tensor parameters in model");
    return ;
  }


  int input_length = model_input->bytes / sizeof(float);
  TfLiteStatus setup_status = SetupAccelerometer(error_reporter);
  if (setup_status != kTfLiteOk) {
    error_reporter->Report("Set up failed\n");
    return ;
  }


  error_reporter->Report("Set up successful...\n");


  while (gestureLED == 1) {


    // Attempt to read new data from the accelerometer

    got_data = ReadAccelerometer(error_reporter, model_input->data.f,

                                 input_length, should_clear_buffer);
    // If there was no new data,

    // don't try to clear the buffer again and wait until next time
    if (!got_data) {
      should_clear_buffer = false;
      continue;
    }


    // Run inference, and report any error

    TfLiteStatus invoke_status = interpreter->Invoke();

    if (invoke_status != kTfLiteOk) {
      error_reporter->Report("Invoke failed on index: %d\n", begin_index);
      continue;
    }


    // Analyze the results to obtain a prediction

    gesture_index = PredictGesture(interpreter->output(0)->data.f);
    // Clear the buffer next time we read data
    should_clear_buffer = gesture_index < label_num;
    // Produce an output

    if(gesture_index == 0){
      if(angle_index < 4)
        angle_index++; 
      else angle_index = 0;
      uLCD_print(angle_list[angle_index]);
	  cout << "index :" << angle_index << endl;
    } 
    
    else if (gesture_index == 1) {
	    if(angle_index > 0)
            angle_index--; 
        else angle_index = 4;
        uLCD_print(angle_list[angle_index]);
	    cout << "index :" << angle_index << endl;
    }

    if (gesture_index < label_num) {
      error_reporter->Report(config.output_message[gesture_index]);
    }

    }
        }
    }
}
/*******************************************************************************/



/*void Tilt mode*/
/****************************************/
void Tilt_mode()
{
    while(1){
        if(tiltLED == 1 && gestureLED == 0)
        {
          /*initialize variables*/
          /*************************/
          int16_t pDataXYZ_init[3] = {0};
          int16_t pDataXYZ[3] = {0};
          double mag_A;
          double mag_B;
          double cos;
          double rad_det;
          double angle_det;
          /************************/


















        /*initialization*/
        /*********************************/
        printf("enter angle detection mode\r\n");
        printf("Place the mbed on table after LEDs\r\n");
        ThisThread::sleep_for(2000ms);
        for (int i=0; i<5; i++) {
        test_LED = 1;                            
        ThisThread::sleep_for(100ms);
        test_LED = 0;
        ThisThread::sleep_for(100ms);
        }
    
        BSP_ACCELERO_AccGetXYZ(pDataXYZ_init);
        printf("reference acceleration vector: %d, %d, %d\r\n", pDataXYZ_init[0], pDataXYZ_init[1], pDataXYZ_init[2]);

        printf("Tilt the mbed after LEDs\r\n");
        ThisThread::sleep_for(2000ms);
        for (int i=0; i<5; i++) {
        test_LED = 1;                            
        ThisThread::sleep_for(100ms);
        test_LED = 0;
        ThisThread::sleep_for(100ms);
        }
        /********************************/


        /*start tilting*/
        /********************************/
        while (tiltLED == 1) {
        BSP_ACCELERO_AccGetXYZ(pDataXYZ);
        printf("Angle Detection: %d %d %d\r\n",pDataXYZ[0], pDataXYZ[1], pDataXYZ[2]);
        mag_A = sqrt(pDataXYZ_init[0]*pDataXYZ_init[0] + pDataXYZ_init[1]*pDataXYZ_init[1] + pDataXYZ_init[2]*pDataXYZ_init[2]);
        mag_B = sqrt(pDataXYZ[0]*pDataXYZ[0] + pDataXYZ[1]*pDataXYZ[1] + pDataXYZ[2]*pDataXYZ[2]);
        cos = ((pDataXYZ_init[0]*pDataXYZ[0] + pDataXYZ_init[1]*pDataXYZ[1] + pDataXYZ_init[2]*pDataXYZ[2])/(mag_A)/(mag_B));
        rad_det = acos(cos);
        angle_det = 180.0 * rad_det/3.1415926;
        printf("angle_det = %f\r\n", angle_det);
        uLCD_print(angle_det);
        
        /*if (angle_det > angle_list[angle_index]) {
            
            mqtt_queue.call(&publish_message, &client2);
            printf("over tilting\n");
        }*/
        ThisThread::sleep_for(1000ms);
        /**********************************/
    }
        }
    }
}
/************************************************************************/



/*void uLCD_print*/
/******************************************/
void uLCD_print(int angle)
{
    uLCD.background_color(WHITE);
    uLCD.textbackground_color(WHITE);
    uLCD.cls();
    uLCD.color(GREEN);
    uLCD.cls();
    uLCD.text_width(4); 
    uLCD.text_height(4);
    
    uLCD.locate(1,2);
    uLCD.printf("%2d",angle);
}
/*******************************************/


/*MQTT*/
/**********************************************************************/

void messageArrived(MQTT::MessageData& md) {
    MQTT::Message &message = md.message;
    char msg[300];
    sprintf(msg, "Message arrived: QoS%d, retained %d, dup %d, packetID %d\r\n", message.qos, message.retained, message.dup, message.id);
    printf(msg);
    ThisThread::sleep_for(1000ms);
    char payload[300];
    sprintf(payload, "Payload %.*s\r\n", message.payloadlen, (char*)message.payload);
    printf(payload);
    ++arrivedcount;
}


void publish_message(MQTT::Client<MQTTNetwork, Countdown>* client) {

    message_num++;
    MQTT::Message message;
    char buff[100];

    if(gestureLED == 1) {
      topic = topic1;
      printf("topic1\n");
      sprintf(buff, "%d\r\n", angle_list[angle_index]);
    } 
    else if (tiltLED == 1) {
      topic = topic2;
      printf("topic2\n");
      sprintf(buff, "%d\r\n", angle_list[angle_index]);
    }  
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;
    message.payload = (void*) buff;
    message.payloadlen = strlen(buff) + 1;
    
    int rc = client->publish(topic, message);
    ThisThread::sleep_for(100ms);
}

void close_mqtt() {
    closed = true;
}

/*************************************************/

