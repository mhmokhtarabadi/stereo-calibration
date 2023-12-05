#pragma once

#include <vector>
#include <opencv2/opencv.hpp>

double intrinsicCalibration(cv::VideoCapture cap,
                            cv::InputOutputArray cameraMatrix,
                            cv::InputOutputArray distCoffs,
                            const cv::Size2i frameSize,
                            const cv::Size2i patternSize,
                            const float cubeSize,
                            const int numberOfFrames = 10);

double extrinsicCalibration(cv::VideoCapture cap1,
                            cv::VideoCapture cap2,
                            cv::InputOutputArray cameraMatrix1,
                            cv::InputOutputArray distCoeffs1,
                            cv::InputOutputArray cameraMatrix2,
                            cv::InputOutputArray distCoeffs2,
                            cv::InputOutputArray R,
                            cv::InputOutputArray T,
                            const cv::Size2i frameSize,
                            const cv::Size2i patternSize,
                            const float cubeSize,
                            const int numberOfFrames = 15);
