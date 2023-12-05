
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>

#include "cameraCalibration.hpp"

const cv::Size2i patternSize(7, 4);
const float cubeSize = 47.2f; // mm

static bool finishedAsking = false;
std::string cam1_str, cam2_str; // cam1 => left , cam2 => right
int cam1, cam2;

void askCameraIndexes()
{
    std::cout << "What is the index of left camera? ";
    std::cin >> cam1_str;
    cam1 = std::stoi(cam1_str);

    std::cout << "What is the index of right camera? ";
    std::cin >> cam2_str;
    cam2 = std::stoi(cam2_str);

    finishedAsking = true;
}

int main()
{
    printf("===== Camera Calibration =====\r\n\n");

    // Step 1: camera indexes
    printf("Step 1: camera indexes\r\n");

    std::thread askThread(askCameraIndexes);

    std::vector<int> cameraIndexes;
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);
    for (int i = 0; i < 10; i++)
    {
        cv::VideoCapture cap(i);

        if (cap.isOpened())
        {
            cameraIndexes.push_back(i);
            cap.release();
        }
    }
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_WARNING);

    std::vector<cv::VideoCapture> caps;
    for (int index : cameraIndexes)
    {
        caps.push_back(cv::VideoCapture(index));
    }

    while (true)
    {
        std::vector<cv::Mat> frames;
        for (size_t i = 0; i < cameraIndexes.size(); i++)
        {
            cv::Mat frame;
            if (!caps[i].read(frame))
            {
                char *message;
                sprintf(message, "can not read frame from camera index %i", cameraIndexes[i]);
                throw std::runtime_error(message);
            }
            char text[30];
            sprintf(text, "index = %i", cameraIndexes[i]);
            std::string text_str(text);
            cv::putText(frame, text_str, cv::Point2i(50, 50), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 0, 255), 3);
            cv::resize(frame, frame, cv::Size2i(320, 240));
            frames.push_back(frame);
        }

        cv::Mat concatedFrame;
        cv::hconcat(frames, concatedFrame);

        cv::imshow("frames", concatedFrame);
        cv::waitKey(1);
        if (finishedAsking)
            break;
    }

    cv::destroyAllWindows();
    caps.clear();

    askThread.join();
    printf("\r\n");

    // Step 2: left camera calibration
    printf("Step 2: left camera calibration\r\n");

    cv::VideoCapture cap1(cam1);
    if (!cap1.isOpened())
        throw std::runtime_error("can not open the left camera.");
    cap1.set(cv::CAP_PROP_BRIGHTNESS, 40);
    cap1.set(cv::CAP_PROP_CONTRAST, 30);
    cap1.set(cv::CAP_PROP_SATURATION, 70);

    int width = cap1.get(cv::CAP_PROP_FRAME_WIDTH);
    int height = cap1.get(cv::CAP_PROP_FRAME_HEIGHT);

    cv::Mat cameraMatrixLeft, distCoeffsLeft;
    double retLeft = intrinsicCalibration(cap1,
                                          cameraMatrixLeft,
                                          distCoeffsLeft,
                                          cv::Size2i(width, height),
                                          patternSize,
                                          cubeSize,
                                          10);

    printf("Done, RMS: %f\r\n\n", (float)retLeft);

    // Step 3: right camera calibration
    printf("Step 3: right camera calibration\r\n");

    cv::VideoCapture cap2(cam2);
    if (!cap2.isOpened())
        throw std::runtime_error("can not open the left camera.");
    cap2.set(cv::CAP_PROP_BRIGHTNESS, 40);
    cap2.set(cv::CAP_PROP_CONTRAST, 30);
    cap2.set(cv::CAP_PROP_SATURATION, 70);

    cv::Mat cameraMatrixRight, distCoeffsRight;
    double retRight = intrinsicCalibration(cap2,
                                           cameraMatrixRight,
                                           distCoeffsRight,
                                           cv::Size2i(width, height),
                                           patternSize,
                                           cubeSize,
                                           10);

    printf("Done, RMS: %f\r\n\n", (float)retRight);

    // Step 4: stereo calibration
    printf("Step 4: stereo calibration\r\n");

    cv::Mat R, T;
    double retStereo = extrinsicCalibration(cap1,
                                            cap2,
                                            cameraMatrixLeft,
                                            distCoeffsLeft,
                                            cameraMatrixRight,
                                            distCoeffsRight,
                                            R,
                                            T,
                                            cv::Size2i(width, height),
                                            patternSize,
                                            cubeSize,
                                            15);
    cap1.release();
    cap2.release();

    printf("Done, RMS: %f\r\n\n", (float)retStereo);

    return 0;
}