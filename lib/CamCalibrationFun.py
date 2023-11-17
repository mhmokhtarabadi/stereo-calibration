
import cv2
import numpy as np
import os

def IntrinsicCalibration(imagesFolder, cornersShape, squareSize, showFlag=False):
    '''
    Calculates intrinsics parameters of a camera using 
    checkerboard images.
    
    INPUTs:
        - imagesFolder: folder path containing checkerboard images
        - cornersShape: a tuple (h, w) that represents checkerboard size
        - squareSize:   the size of checkerboard squares
        - showFlag:     to show each detected images or not
    
    OUTPUTs:
        - ret:          return value that shows the accuracy of calibration
        - mtx:          intrinsic matrix
        - dist:         distortion matrix
    '''

    imagesList = sorted(os.listdir(imagesFolder))
    images = []
    for name in imagesList:
        img = cv2.imread(os.path.join(imagesFolder, name))
        images.append(img)

    rows = cornersShape[0]
    columns = cornersShape[1]

    #coordinates of squares in the checkerboard world space
    objp = np.zeros((rows*columns,3), np.float32) 
    objp[:,:2] = np.mgrid[0:rows,0:columns].T.reshape(-1,2)
    objp[:,0] = objp[:,0]*squareSize
    objp[:,1] = objp[:,1]*squareSize

    objpoints = [] # 3d points in world space
    imgpoints = [] # 2d points in image plane
    nbDetected = 0 # number of detected checkerboards

    criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001) # stop refining after 30 iterations or if error less than 0.001px

    for i, img in enumerate(images):
        # Find corners
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        ret, corners = cv2.findChessboardCorners(gray, cornersShape)
        # Refine corners
        if ret == True:             
            imgp = cv2.cornerSubPix(gray, corners, (11,11), (-1,-1), criteria)
            # Draw and display the corners
            if showFlag:
                cv2.drawChessboardCorners(img, cornersShape, imgp, ret)
                cv2.putText(img, "Please press y to see the next frame.", (10, 20), cv2.FONT_HERSHEY_SIMPLEX, 0.75, (0, 0, 255), 1)
                cv2.putText(img, "if corners are detected wrongly, press n", (10, 40), cv2.FONT_HERSHEY_SIMPLEX, 0.75, (0, 0, 255), 1)
                cv2.imshow("img", img)
                key = cv2.waitKey(0)
            else:
                key = ord("y")

            if key == ord("n"):
                print(f"img{i}: Corners found, but refused.")
            else:
                nbDetected += 1
                objpoints.append(objp)
                imgpoints.append(imgp)
                print(f"imgPair{i}: Corners found.")
        else:
            if showFlag:
                print(f"img{i}: Corners not found.")
    
    if showFlag:
        cv2.destroyAllWindows()

    print(f"{nbDetected}/{len(images)} images have been detected.")

    # Calibration
    ret, Mint, dist, rvecs, tvecs = cv2.calibrateCamera(objpoints, imgpoints, img.shape[1::-1], 
                                        None, None, flags=(cv2.CALIB_FIX_K3 + cv2.CALIB_FIX_PRINCIPAL_POINT))
    
    return ret, Mint, dist


def ExtrinsicCalibration(imagesFolder1, imagesFolder2, cornersShape, squareSize, Mint1, dist1, Mint2, dist2, showFlag=False):
    '''
    Calculates extrinsic (stereo) parameters of a camera using 
    checkerboard images.
    
    INPUTs:
        - imagesFolder1: folder path containing checkerboard images (camera #1)
        - imagesFolder2: folder path containing checkerboard images (camera #2)
        - cornersShape:  a tuple (h, w) that represents checkerboard size
        - squareSize:    the size of checkerboard squares
        # - Mint1:         intrinsic matrices for camera #1
        - dist1:         distortions for camera #1
        - Mint2:         intrinsic matrices for camera #2
        - dist2:         distortions for camera #2
        - showFlag:      to show each detected images or not
    
    OUTPUTs:
        - ret:          return value that shows the accuracy of calibration
        - R:            rotation matrix
        - t:            translation matrix
    '''
    imagesList1 = sorted(os.listdir(imagesFolder1))
    imagesList2 = sorted(os.listdir(imagesFolder2))
    images1 = []
    images2 = []
    for name1, name2 in zip(imagesList1, imagesList2):
        img1 = cv2.imread(os.path.join(imagesFolder1, name1))
        img2 = cv2.imread(os.path.join(imagesFolder2, name2))
        images1.append(img1)
        images2.append(img2)

    rows = cornersShape[0]
    columns = cornersShape[1]

    #coordinates of squares in the checkerboard world space
    objp = np.zeros((rows*columns,3), np.float32) 
    objp[:,:2] = np.mgrid[0:rows,0:columns].T.reshape(-1,2)
    objp[:,0] = objp[:,0]*squareSize
    objp[:,1] = objp[:,1]*squareSize

    objpoints = [] # 3d points in world space
    imgpoints1 = [] # 2d points in image plane (cam #1)
    imgpoints2 = [] # 2d points in image plane (cam #2)
    nbDetected = 0 # number of detected checkerboard pairs

    criteriaSubPix = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001) # stop refining after 30 iterations or if error less than 0.001px
    criteriaStereo = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 100, 0.0001)

    for i, (img1, img2) in enumerate(zip(images1, images2)):
        # Find corners
        gray1 = cv2.cvtColor(img1, cv2.COLOR_BGR2GRAY)
        ret1, corners1 = cv2.findChessboardCorners(gray1, cornersShape)
        gray2 = cv2.cvtColor(img2, cv2.COLOR_BGR2GRAY)
        ret2, corners2 = cv2.findChessboardCorners(gray2, cornersShape)
        # Refine corners
        if ret1 and ret2:            
            imgp1 = cv2.cornerSubPix(gray1, corners1, (11,11), (-1,-1), criteriaSubPix)
            imgp2 = cv2.cornerSubPix(gray2, corners2, (11,11), (-1,-1), criteriaSubPix)
            # Draw and display the corners
            if showFlag:
                cv2.drawChessboardCorners(img1, cornersShape, imgp1, ret1)
                cv2.drawChessboardCorners(img2, cornersShape, imgp2, ret2)
                img = np.hstack((img1, img2))
                cv2.putText(img, "Please press ENTER to see the next frame.", (10, 20), cv2.FONT_HERSHEY_SIMPLEX, 0.75, (0, 0, 255), 1)
                cv2.putText(img, "if corners are detected wrongly, press n", (10, 40), cv2.FONT_HERSHEY_SIMPLEX, 0.75, (0, 0, 255), 1)
                cv2.imshow("img", img)
                key = cv2.waitKey(0)
            else:
                key = ord("y")
            
            if key == ord("n"):
                print(f"img{i}: Corners found, but refused.")
            else:
                nbDetected += 1
                objpoints.append(objp)
                imgpoints1.append(imgp1)
                imgpoints2.append(imgp2)
                print(f"imgPair{i}: Corners found.")
        else:
            if showFlag:
                print(f"imgPair{i}: Corners not found.")
    
    if showFlag:
        cv2.destroyAllWindows()

    print(f"{nbDetected}/{len(images1)} image pairs have been detected.")

    # Calibration
    ret, CM1, dist1, CM2, dist2, R, T, E, F = cv2.stereoCalibrate(objpoints, imgpoints1, imgpoints2, 
                                                                  Mint1, dist1, Mint2, dist2, img1.shape[1::-1], 
                                                                  criteria=criteriaStereo, flags=cv2.CALIB_FIX_INTRINSIC)

    return ret, R, T