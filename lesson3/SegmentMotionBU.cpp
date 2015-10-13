///@File: SegmentMotionBU.cpp
///@Brief: Contains implementation of SegementMotionBU class
///@Author: Vitaliy Baldeev
///@Date: 03 October 2015

#include "SegmentMotionBU.h"

#include <iostream>

#include "opencv2\videoio.hpp"
#include "opencv2\imgproc.hpp"
#include "opencv2\highgui.hpp"

///////////////////////////////////////////////////////////////////////////////
cv::Mat SegmentMotionBU::process(cv::VideoCapture& capture)
{
    cv::Mat currentFrame;
    capture >> currentFrame;

    if (!m_prevBackgroundUpdated)
    {
        cv::cvtColor(currentFrame, m_prevBackground, CV_RGB2GRAY);
        m_prevBackgroundUpdated = true;
    }

    cvtColor(currentFrame, currentFrame, CV_RGB2GRAY);
    updateBackground(currentFrame);

    cv::Mat result = abs(m_prevBackground - currentFrame.clone());
    cv::threshold(result, result, m_params.threshold, 255, CV_THRESH_BINARY);

    return result;
}

///////////////////////////////////////////////////////////////////////////////
void SegmentMotionBU::createGUI()
{
    const std::string windowName = GetName();
    cv::namedWindow(windowName);

    int initSliderValue = 10;

    setAlphaFromSlider(initSliderValue, &m_params);
    setThresholdFromSlider(initSliderValue, &m_params);

    cv::createTrackbar("Threshold", windowName, &initSliderValue, 255,
        setThresholdFromSlider, &m_params);

    cv::createTrackbar("Alpha", windowName, &initSliderValue, 100,
        setAlphaFromSlider, &m_params);
}

///////////////////////////////////////////////////////////////////////////////
void SegmentMotionBU::updateBackground(cv::Mat& currentFrame)
{
    float scaled_alpha = m_params.alpha * 0.01f;

    cv::Mat_<float> floatPrevBackground(m_prevBackground);
    cv::Mat_<float> floatCurrentFrame(currentFrame);
    cv::Mat_<float> floatBackground = (1 - scaled_alpha) * floatPrevBackground +
        scaled_alpha * floatCurrentFrame;

    floatBackground.convertTo(m_prevBackground, CV_8U);

}

///////////////////////////////////////////////////////////////////////////////
void SegmentMotionBU::setThresholdFromSlider(int thresholdSlider, void* changeableDataPtr)
{
    Params* data = static_cast<Params*>(changeableDataPtr);
    data->threshold = thresholdSlider;
}

///////////////////////////////////////////////////////////////////////////////
void SegmentMotionBU::setAlphaFromSlider(int alphaSlider, void* changeableDataPtr)
{
    Params* data = static_cast<Params*>(changeableDataPtr);
    data->alpha = alphaSlider;
}
