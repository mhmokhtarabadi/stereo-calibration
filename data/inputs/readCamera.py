import cv2
import numpy as np

cam1 = 2
cam2 = 4

cap1 = cv2.VideoCapture(cam1)
cap1.set(cv2.CAP_PROP_BRIGHTNESS, 40)
cap1.set(cv2.CAP_PROP_CONTRAST, 30)
cap1.set(cv2.CAP_PROP_SATURATION, 70)

cap2 = cv2.VideoCapture(cam2)
cap2.set(cv2.CAP_PROP_BRIGHTNESS, 40)
cap2.set(cv2.CAP_PROP_CONTRAST, 30)
cap2.set(cv2.CAP_PROP_SATURATION, 70)

while True:

    ok1, img1 = cap1.read()
    ok2, img2 = cap2.read()

    cv2.putText(img1, f"frame {cam1}", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 0, 0), 2)
    cv2.putText(img2, f"frame {cam2}", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 0, 0), 2)

    frames = np.concatenate((img1, img2), axis=1)

    cv2.imshow('frames', frames)

    if cv2.waitKey(1) & 0xFF==ord('q'):
        break

cap1.release()
cap2.release()
