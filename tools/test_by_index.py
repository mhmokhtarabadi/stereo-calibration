import cv2

cv2.setLogLevel(1)

camNames = []
caps = []
print("\nlist of founded cameras:")
i = 0
for camName in range(10):
    cap = cv2.VideoCapture(camName)

    if cap.isOpened():
        print(f"cam{i}: {camName}")
        i += 1
        camNames.append(camName)
        caps.append(cap)

if len(camNames):
    while True:
        frames = []
        for cap, camName in zip(caps, camNames):
            ok, frame = cap.read()
            if not ok:
                RuntimeError(f"can not read image from {camName}")
            frames.append(cv2.resize(frame, (320, 240)))

        concatenatedFrames = cv2.hconcat(frames)

        cv2.imshow("frames", concatenatedFrames)

        if cv2.waitKey(1) == ord("q"):
            break

    cv2.destroyAllWindows()
    for cap in caps:
        cap.release()
