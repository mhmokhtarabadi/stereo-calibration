
# Stereo Calibration

To do the camera calibration, first you need to go inside the `./data/inputs` folder and take images from checkerboards with your camera connected. It will create two **cam1** and **cam2** folder inside of `./data/inputs` folder. 
```sh
cd ./data/inputs
python ./takeImageTwoCameras.py
```

---

Then from here just run:
```sh
python ./CamCalibration.py
```
and it will calibrate your stereo setup. The results will be saved in this file:
```sh
./data/outputs/CalibrationOutputs.pkl
# and
./data/outputs/CalibrationOutputs.txt
```

You can change stereo calibration configs in this file:
```sh
./data/config.toml
```

---

Later if you want to load stereo parameters into your project (for example python code):
```python
import pickle

with open("./data/outputs/CalibrationOutputs.pkl", "rb") as file:
    Mint1, Mint2, R, T = pickle.load(file)
```

## Requirements

```sh
virtualenv calib_env
source calib_env/bin/activate
pip install --upgrade pip
pip install -r requirements.txt
```

---
thank to https://github.com/perfanalytics/pose2sim 