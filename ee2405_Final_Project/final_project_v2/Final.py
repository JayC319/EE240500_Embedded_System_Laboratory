
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
REDCAN2_threshold = (100, 37, 15, 104, -67, 127)
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




#line_tracing

'''
def line_tracing():
    linetmp = ""
    while(True):

        clock.tick()
        img = sensor.snapshot()
        if enable_lens_corr: img.lens_corr(1.8) # for 2.8mm lens...


        # `merge_distance` controls the merging of nearby lines. At 0 (the default), no
        # merging is done. At 1, any line 1 pixel away from another is merged... and so
        # on as you increase this value. You may wish to merge lines as line segment
        # detection produces a lot of line segment results.

        # `max_theta_diff` controls the maximum amount of rotation difference between
        # any two lines about to be merged. The default setting allows for 15 degrees.

        for l in img.find_line_segments(merge_distance = 0, max_theta_diff = 5):
           img.draw_line(l.line(), color = (255, 0, 0))
           uart.write("/goStraight/run 100 \n".encode())
'''


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
        time.sleep(2)




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









