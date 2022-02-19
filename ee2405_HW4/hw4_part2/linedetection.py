# Untitled - By: user - 週六 六月 12 2021

enable_lens_corr = False

import sensor, image, time, math

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)

clock = time.clock()

while(True):
    clock.tick()
    img = sensor.snapshot()
    if enable_lens_corr: img.lens_corr(1.8)

    for l in img.find_line_segments(threshold = 1000 ,theta_margin = 0, rho_margin  = 15, segment_threshold = 10):
        if(l.y1() < 5):
            img.draw_line(l.line(), color = (255,0,0))
            print("found")

    print(clock.fps())
