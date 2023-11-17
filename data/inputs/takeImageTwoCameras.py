import cv2
import numpy as np
import os
import time

# =========== Settings ===========
cam1 = 2
cam2 = 4
savePath1 = './cam1'
savePath2 = './cam2'
takeTime = 5
# ================================

if os.path.exists(savePath1):
    os.system("rm -r " + savePath1)
if os.path.exists(savePath2):
    os.system("rm -r " + savePath2)

os.mkdir(savePath1)
os.mkdir(savePath2)

cap1 = cv2.VideoCapture(cam1)
cap1.set(cv2.CAP_PROP_BRIGHTNESS, 40)
cap1.set(cv2.CAP_PROP_CONTRAST, 30)
cap1.set(cv2.CAP_PROP_SATURATION, 70)

cap2 = cv2.VideoCapture(cam2)
cap2.set(cv2.CAP_PROP_BRIGHTNESS, 40)
cap2.set(cv2.CAP_PROP_CONTRAST, 30)
cap2.set(cv2.CAP_PROP_SATURATION, 70)

t = time.time()
i = 0

while 1:

    ret1, frame1 = cap1.read()
    ret2, frame2 = cap2.read()

    if ret1 and ret2:
        frameCopy1 = frame1.copy()
        frameCopy2 = frame2.copy()

        twoFrames = np.concatenate((frame1, frame2), axis=1)

        timeCounter = float(time.time() - t)
        if timeCounter < takeTime:
            cv2.putText(twoFrames, f'Image will be taken in {int(takeTime - timeCounter)} s', (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)
            cv2.putText(twoFrames, f'Images taken: {i}', (10, 60), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)
            cv2.putText(twoFrames, 'Press q to exit', (10, 90), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 0, 0), 1)
            cv2.imshow('frame', twoFrames)
        else:
            path1 = os.path.join(savePath1, f'img{i:02d}_1.png')
            cv2.imwrite(path1, frameCopy1)
            path2 = os.path.join(savePath2, f'img{i:02d}_2.png')
            cv2.imwrite(path2, frameCopy2)
            i += 1

            cv2.putText(twoFrames, f'Your Image:', (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)
            cv2.imshow('frame', twoFrames)
            cv2.waitKey(1000)
            t = time.time()
    else:
        print('No frames captured!')

    if cv2.waitKey(1) & 0xFF==ord('q'):
        break

cap1.release()
cap2.release()
