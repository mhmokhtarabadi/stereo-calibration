
#include "cameraCalibration.hpp"
#include "my_time.hpp"

#include <toml++/toml.h>

static auto config = toml::parse_file(std::string(DIRECTORY_PATH) + "/conf.toml");

double intrinsicCalibration(cv::VideoCapture cap,
                            cv::InputOutputArray cameraMatrix,
                            cv::InputOutputArray distCoeffs,
                            const cv::Size2i frameSize,
                            const cv::Size2i patternSize,
                            const float cubeSize,
                            const int numberOfFrames)
{
    std::vector<cv::Point3f> objectPoint;
    for (int i = 0; i < patternSize.height; i++)
    {
        for (int j = 0; j < patternSize.width; j++)
            objectPoint.push_back(cv::Point3f(cubeSize * j, cubeSize * i, 0.0));
    }

    std::vector<std::vector<cv::Point3f>> objectPoints;
    std::vector<std::vector<cv::Point2f>> imagePoints;
    int numOfDetectedFrames = 0;
    auto now = getTimeNow();
    while (numOfDetectedFrames < numberOfFrames)
    {
        cv::Mat frame;
        if (!cap.read(frame))
            throw std::runtime_error("can not read frame from the left camera.");

        if (msPassedSince(now) >= config["frames"]["capture_time"].value_or(3000))
        {
            std::vector<cv::Point2f> corners;
            bool ret = cv::findChessboardCornersSB(frame, patternSize, corners);
            if (ret)
            {
                imagePoints.push_back(corners);
                objectPoints.push_back(objectPoint);
                numOfDetectedFrames++;
                cv::drawChessboardCorners(frame, patternSize, corners, ret);
                cv::putText(frame, "corners detected", cv::Point2i(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);
                std::string text = "detected corners = " + std::to_string(numOfDetectedFrames) + "/" + std::to_string(numberOfFrames);
                cv::putText(frame, text, cv::Point2i(50, 100), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 0), 2);
                cv::imshow("left frame", frame);
                cv::waitKey(1000);
            }
            else
            {
                cv::putText(frame, "corners not detected", cv::Point2i(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 2);
                std::string text = "detected corners = " + std::to_string(numOfDetectedFrames) + "/" + std::to_string(numberOfFrames);
                cv::putText(frame, text, cv::Point2i(50, 100), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 0), 2);
                cv::imshow("left frame", frame);
                cv::waitKey(1000);
            }
            now = getTimeNow();
        }
        else
        {
            std::string text = "detected corners = " + std::to_string(numOfDetectedFrames) + "/" + std::to_string(numberOfFrames);
            cv::putText(frame, text, cv::Point2i(50, 100), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 0), 2);
            cv::imshow("left frame", frame);
            cv::waitKey(1);
        }
    }

    cv::destroyAllWindows();

    cv::Mat rvec, tvec;
    double ret = cv::calibrateCamera(objectPoints, imagePoints, frameSize, cameraMatrix, distCoeffs, rvec, tvec);

    return ret;
}

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
                            const int numberOfFrames)
{
    std::vector<cv::Point3f> objectPoint;
    for (int i = 0; i < patternSize.height; i++)
    {
        for (int j = 0; j < patternSize.width; j++)
            objectPoint.push_back(cv::Point3f(cubeSize * j, cubeSize * i, 0.0));
    }

    std::vector<std::vector<cv::Point3f>> objectPoints;
    std::vector<std::vector<cv::Point2f>> imagePoints1, imagePoints2;
    int numOfDetectedFrames = 0;
    auto now = getTimeNow();
    while (numOfDetectedFrames < numberOfFrames)
    {
        cv::Mat frame1, frame2, concatenatedFrame;
        if (!cap1.read(frame1) || !cap2.read(frame2))
            throw std::runtime_error("can not read frames.");

        if (msPassedSince(now) >= config["frames"]["capture_time"].value_or(3000))
        {
            std::vector<cv::Point2f> corners1, corners2;
            bool ret1 = cv::findChessboardCornersSB(frame1, patternSize, corners1);
            bool ret2 = cv::findChessboardCornersSB(frame2, patternSize, corners2);
            if (ret1 && ret2)
            {
                imagePoints1.push_back(corners1);
                imagePoints2.push_back(corners2);
                objectPoints.push_back(objectPoint);
                numOfDetectedFrames++;
                cv::drawChessboardCorners(frame1, patternSize, corners1, ret1);
                cv::drawChessboardCorners(frame2, patternSize, corners2, ret2);
                cv::hconcat(std::vector<cv::Mat>{frame1, frame2}, concatenatedFrame);
                cv::putText(concatenatedFrame, "corners detected", cv::Point2i(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);
                std::string text = "detected corners = " + std::to_string(numOfDetectedFrames) + "/" + std::to_string(numberOfFrames);
                cv::putText(concatenatedFrame, text, cv::Point2i(50, 100), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 0), 2);
                cv::imshow("frames", concatenatedFrame);
                cv::waitKey(1000);
            }
            else
            {
                cv::hconcat(std::vector<cv::Mat>{frame1, frame2}, concatenatedFrame);
                cv::putText(concatenatedFrame, "corners not detected", cv::Point2i(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 2);
                std::string text = "detected corners = " + std::to_string(numOfDetectedFrames) + "/" + std::to_string(numberOfFrames);
                cv::putText(concatenatedFrame, text, cv::Point2i(50, 100), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 0), 2);
                cv::imshow("frames", concatenatedFrame);
                cv::waitKey(1000);
            }
            now = getTimeNow();
        }
        else
        {
            cv::hconcat(std::vector<cv::Mat>{frame1, frame2}, concatenatedFrame);
            std::string text = "detected corners = " + std::to_string(numOfDetectedFrames) + "/" + std::to_string(numberOfFrames);
            cv::putText(concatenatedFrame, text, cv::Point2i(50, 100), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 0), 2);
            cv::imshow("frames", concatenatedFrame);
            cv::waitKey(1);
        }
    }

    cv::destroyAllWindows();

    cv::Mat E, F;
    double ret = cv::stereoCalibrate(objectPoints, imagePoints1, imagePoints2, cameraMatrix1, distCoeffs1, cameraMatrix2, distCoeffs2, frameSize, R, T, E, F, cv::CALIB_FIX_INTRINSIC);

    return ret;
}
