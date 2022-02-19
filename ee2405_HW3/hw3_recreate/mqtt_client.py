import serial
import time
import paho.mqtt.client as paho

mqttc = paho.Client()

host = "172.20.10.12"
topic1 = "Gesture_Data"
topic2 = "Tilt_Data"

serdev = '/dev/ttyACM0'
s = serial.Serial(serdev, 9600)


def on_connect(self, mosq, obj, rc):
    print("Connected rc: " + str(rc))

def on_message(mosq, obj, msg):
    print("[Received] Topic: " + msg.topic + ", Message: " + str(msg.payload) + "\n")
    if msg.topic == 'Gesture_Data':
        if (chr(msg.payload[0]) == 'S'):    
            s.write("/LEDControl/run 1 0\r".encode())
            s.write("/LEDControl/run 2 1\r".encode())
        
        print("entering tilting detction mode \r\n")

    if msg.topic == 'Tilt_Data':
        if (chr(msg.payload[0]) == 'i'):
            print("hi")
            if(chr(msg.payload[5]) == '5'):
                s.write("/LEDControl/run 2 0\r".encode())
            
       
    

def on_subscribe(mosq, obj, mid, granted_qos):
    print("Subscribed OK")

def on_unsubscribe(mosq, obj, mid, granted_qos):
    print("Unsubscribed OK")

mqttc.on_message = on_message
mqttc.on_connect = on_connect
mqttc.on_subscribe = on_subscribe
mqttc.on_unsubscribe = on_unsubscribe

print("Connecting to " + host + "/" + topic1)
print("Connecting to " + host + "/" + topic2)

mqttc.connect(host, port=1883, keepalive=60)
mqttc.subscribe(topic1, 0)
mqttc.subscribe(topic2, 0)

'''num = 0

while num != 1:
    ret = mqttc.publish(topic2, "Message from pyhton!\n", qos =0)

    if(ret[0] != 0):
        
        print("Publish failed")
    
    time.sleep(1.5)

    num+=1

'''

mqttc.loop_forever()


s.close()