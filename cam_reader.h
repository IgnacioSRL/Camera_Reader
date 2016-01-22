#ifndef CAM_READER_H
#define CAM_READER_H

#include <QTime>
#include <thread>
#include <mutex>
#include <opencv2/opencv.hpp>

using namespace std;

class Cam_Reader
{
public:
    Cam_Reader(string direction="", float fps=0);
    void initialization(string direction="", float fps=0);
    bool get_frame_rate(float &fps);
    cv::Mat get_image();
    bool get_image(cv::Mat &image);

private:
    void frame_reader();
    //Variables
    cv::VideoCapture cap;
    QTime t;
    int miliseconds_cycle;

    //Performance
    float fps_performance;
    bool underflow;

    //Thread control
    std::mutex mtx;
};

#endif // CAM_READER_H
