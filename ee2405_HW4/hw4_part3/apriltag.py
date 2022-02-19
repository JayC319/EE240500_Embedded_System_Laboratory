import pyb, sensor, image, time, math

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA) # we run out of memory if the resolution is much bigger...
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False)  # must turn this off to prevent image washout...
sensor.set_auto_whitebal(False)  # must turn this off to prevent image washout...
clock = time.clock()

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
   clock.tick()
   img = sensor.snapshot()
   for tag in img.find_apriltags(fx=f_x, fy=f_y, cx=c_x, cy=c_y): # defaults to TAG36H11
      img.draw_rectangle(tag.rect(), color = (255, 0, 0))
      img.draw_cross(tag.cx(), tag.cy(), color = (0, 255, 0))
      # The conversion is nearly 6.2cm to 1 -> translation
      print_args = (tag.x_translation(), tag.y_translation(), tag.z_translation(), \
            degrees(tag.x_rotation()), degrees(tag.y_rotation()), degrees(tag.z_rotation()))
      # Translation units are unknown. Rotation units are in degrees.
      THETA = (180 * math.atan(tag.x_translation() / tag.z_translation()) / math.pi)
      BUFF_CONSTANT = 2.5
      REAL_DISTANCE = BUFF_CONSTANT * math.sqrt(tag.x_translation()*tag.x_translation()+tag.y_translation()*tag.y_translation()+tag.z_translation()*tag.z_translation())
      #sleeptime1 = (0.38 * degrees(tag.y_rotation())/90)
      REAL_x_DISTANCE = (REAL_DISTANCE * math.sin(math.pi*THETA/180))
      sleeptime2 = abs(REAL_x_DISTANCE/17)

      if (THETA > 5 or THETA < -5):
             if(THETA > 5 and tag.x_translation() <-1.5):
                uart.write("/turn/run 200 1 \n".encode())
                time.sleep(0.36)
                uart.write("/stop/run \n".encode())
                time.sleep(0.2)
                uart.write("/goStraight/run 100 \n".encode())
                time.sleep(sleeptime2)
                uart.write("/stop/run \n".encode())
                time.sleep(0.5)
                uart.write("/turn/run 200 -1 \n".encode())
                time.sleep(0.39)
                uart.write("/stop/run \n".encode())
                time.sleep(2)


             elif(THETA < -5 and tag.x_translation() > 1.5):
                uart.write("/turn/run 200 -1 \n".encode())
                time.sleep(0.34)
                uart.write("/stop/run \n".encode())
                time.sleep(0.2)
                uart.write("/goStraight/run 100 \n".encode())
                time.sleep(sleeptime2)
                uart.write("/stop/run \n".encode())
                time.sleep(0.5)
                uart.write("/turn/run 200 1 \n".encode())
                time.sleep(0.39)
                uart.write("/stop/run \n".encode())
                time.sleep(2)


      else :
        uart.write(("/stop/run \n").encode())
        time.sleep(1)
        ###
        uart.write("/ping/run \n".encode())
        ###
        print("facing")
