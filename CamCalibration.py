import logging
import os
import toml
import pickle

from lib.CamCalibrationFun import IntrinsicCalibration, ExtrinsicCalibration

with open(os.path.join("data", "outputs/logs.txt"), "a+") as logFile: pass
logging.basicConfig(filename=os.path.join("data", "outputs/logs.txt"), format="%(message)s", 
                    level=logging.INFO)

configPath = os.path.join("data", "config.toml")
configDict = toml.load(configPath)

projectDir = configDict.get("project").get("project_dir")
if projectDir == '': projectDir = os.getcwd()

logging.info("\n\n---------------------------------------------------------------------")
logging.info("Camera calibration")
logging.info("---------------------------------------------------------------------")
logging.info(f"\nProject directory: {projectDir}")

inputFolder = configDict.get("project").get("inputs_folder_name")
outputFolder = configDict.get("project").get("outputs_folder_name")
cornersShape = configDict.get("calibration").get("corners_nb")
squareSize = configDict.get("calibration").get("square_size")
showFlag = configDict.get("calibration").get("show_corner_detection")

# intrinsics parameters
## cam1
imagesFolder1 = os.path.join(inputFolder, "cam1")
ret1, Mint1, dist1 = IntrinsicCalibration(imagesFolder1, cornersShape, squareSize, showFlag)

## cam2
imagesFolder2 = os.path.join(inputFolder, "cam2")
ret2, Mint2, dist2 = IntrinsicCalibration(imagesFolder2, cornersShape, squareSize, showFlag)

# extrinsic parameters
ret, R, T = ExtrinsicCalibration(imagesFolder1, imagesFolder2, cornersShape, squareSize, 
                                 Mint1, dist1, Mint2, dist2, showFlag)

# writing to a pickle file
with open(os.path.join(outputFolder, "CalibrationOutputs.pkl"), "wb") as file:
    pickle.dump([Mint1, Mint2, R, T], file)

# writing to a txt file
with open(os.path.join(outputFolder, "CalibrationOutputs.txt"), "w") as file:
    for row in Mint1:
        for element in row:
            file.write(str(element) + " ")

    file.write("\n")
    for row in Mint2:
        for element in row:
            file.write(str(element) + " ")

    file.write("\n")
    for row in R:
        for element in row:
            file.write(str(element) + " ")

    file.write("\n")
    for row in T:
        for element in row:
            file.write(str(element) + " ")

logging.info("\nCalibration outputs are stored at data/Outputs/CalibrationOutputs.pkl and data/Outputs/CalibrationOutputs.txt.\n")
logging.info(f"Camera #1, Intrinsics calibration return value: {ret1}")
logging.info(f"Camera #2, Intrinsics calibration return value: {ret2}")
logging.info(f"Stereo calibration return value: {ret}")

print("\nMint1 = \n")
print(Mint1)

print("\nMint2 = \n")
print(Mint2)

print("\nR = \n")
print(R)

print("\nT = \n")
print(T)
