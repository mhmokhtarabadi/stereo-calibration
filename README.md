
# Stereo Calibration using OpenCV library

If you have at least two cameras, you can use this repository to calibrate your stereo setup. You Just need to build the project and all the steps will be done automatically.

## Requirements

* OpenCV

    Run `sudo sh build_opencv.sh` inside the [scripts](./scripts/) folder (not tested yet, but probably works!).

## Build

```sh
mkdir build
cd build
cmake ..
make
```

## Run

```
./build/src/camera_calibration
```