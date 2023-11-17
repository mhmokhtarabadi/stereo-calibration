import cv2

for i in range(10):
    cap = cv2.VideoCapture(i)

    if cap.isOpened():
        print(f"index = {i}")
        cap.release()
