
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <fstream>
#include <filesystem>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <toml++/toml.hpp>

#include "cameraCalibration.hpp"

static auto config = toml::parse_file(std::string(DIRECTORY_PATH) + "/conf.toml");

const cv::Size2i patternSize(config["checkerboard"]["pattern_size"][0].value_or(7), config["checkerboard"]["pattern_size"][1].value_or(4));
const float cubeSize = config["checkerboard"]["cube_size"].value_or(1.0f);

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

    std::vector<std::string> cameraNames;
    for (const auto &entity : std::filesystem::directory_iterator("/dev/v4l/by-id/"))
    {
        cv::VideoCapture cap(entity.path());

        if (cap.isOpened())
        {
            cameraNames.push_back(entity.path());
            cap.release();
        }
    }

    std::vector<cv::VideoCapture> caps;
    for (std::string name : cameraNames)
    {
        caps.push_back(cv::VideoCapture(name));
    }

    while (true)
    {
        std::vector<cv::Mat> frames;
        for (size_t i = 0; i < cameraNames.size(); i++)
        {
            cv::Mat frame;
            if (!caps[i].read(frame))
            {
                std::string message = "can not read frame from camera path:\n" + cameraNames[i];
                throw std::runtime_error(message);
            }
            std::string text = "index = " + std::to_string(i);
            cv::putText(frame, text, cv::Point2i(50, 50), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 0, 255), 3);
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

    cv::VideoCapture cap1(cameraNames[cam1]);
    if (!cap1.isOpened())
        throw std::runtime_error("can not open the left camera.");
    cap1.set(cv::CAP_PROP_BRIGHTNESS, config["camera"]["brightness"].value_or(40));
    cap1.set(cv::CAP_PROP_CONTRAST, config["camera"]["contrast"].value_or(30));
    cap1.set(cv::CAP_PROP_SATURATION, config["camera"]["saturation"].value_or(70));

    int width = cap1.get(cv::CAP_PROP_FRAME_WIDTH);
    int height = cap1.get(cv::CAP_PROP_FRAME_HEIGHT);

    cv::Mat cameraMatrixLeft, distCoeffsLeft;
    double retLeft = intrinsicCalibration(cap1,
                                          cameraMatrixLeft,
                                          distCoeffsLeft,
                                          cv::Size2i(width, height),
                                          patternSize,
                                          cubeSize,
                                          config["frames"]["number_of_frames_intrinsic"].value_or(10));

    printf("Done, RMS: %f\r\n\n", (float)retLeft);

    // Step 3: right camera calibration
    printf("Step 3: right camera calibration\r\n");

    cv::VideoCapture cap2(cameraNames[cam2]);
    if (!cap2.isOpened())
        throw std::runtime_error("can not open the left camera.");
    cap2.set(cv::CAP_PROP_BRIGHTNESS, config["camera"]["brightness"].value_or(40));
    cap2.set(cv::CAP_PROP_CONTRAST, config["camera"]["contrast"].value_or(30));
    cap2.set(cv::CAP_PROP_SATURATION, config["camera"]["saturation"].value_or(70));

    cv::Mat cameraMatrixRight, distCoeffsRight;
    double retRight = intrinsicCalibration(cap2,
                                           cameraMatrixRight,
                                           distCoeffsRight,
                                           cv::Size2i(width, height),
                                           patternSize,
                                           cubeSize,
                                           config["frames"]["number_of_frames_intrinsic"].value_or(10));

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
                                            config["frames"]["number_of_frames_extrinsic"].value_or(15));
    cap1.release();
    cap2.release();

    printf("Done, RMS: %f\r\n\n", (float)retStereo);

    // step 5: writing calibration data to file
    std::fstream configFile;
    configFile.open(std::string(DIRECTORY_PATH) + config["outputs"]["stereo_parameters_path"].value_or("/stereoParameters.txt"), std::ios::out);

    // Mint1
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
            configFile << std::setprecision(10) << cameraMatrixLeft.at<double>(i, j) << " ";
    }
    configFile << "\n";

    // Mint2
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
            configFile << cameraMatrixRight.at<double>(i, j) << " ";
    }
    configFile << "\n";

    // R
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
            configFile << R.at<double>(i, j) << " ";
    }
    configFile << "\n";

    // T
    for (int i = 0; i < 3; i++)
        configFile << T.at<double>(i, 0) << " ";
    configFile << "\n";

    // camera names
    configFile << cameraNames[cam1] << "\n"
               << cameraNames[cam2];

    configFile.close();

    std::cout << "Stereo parameters are saved to "
              << std::string(DIRECTORY_PATH) + config["outputs"]["stereo_parameters_path"].value_or("/stereoParameters.txt")
              << std::endl;

    return 0;
}