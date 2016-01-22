#include "cam_reader.h"

Cam_Reader::Cam_Reader(string direction, float fps){
    if(!direction.empty() && direction!="0.0.0.0")
        initialization(direction,fps);
}

void Cam_Reader::initialization(string direction, float fps){
    this->cap.open(direction);
    if(fps>0)
        this->miliseconds_cycle=1000./fps;
    else
        this->miliseconds_cycle=0;
    std::thread thrd(&Cam_Reader::frame_reader,this);
    thrd.detach();
    t.start();
}

void Cam_Reader::frame_reader(){
    while(true){
        if(this->cap.isOpened()){
            mtx.lock();
            this->cap.grab();
            mtx.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

cv::Mat Cam_Reader::get_image(){
    cv::Mat image;
    if(this->cap.isOpened()){
        mtx.lock();
        this->cap.retrieve(image);
        mtx.unlock();
    }
    this->underflow=true;
    while(this->miliseconds_cycle-t.elapsed()>0)
        this->underflow=false;
    this->fps_performance=1000./t.elapsed();
    t.restart();
    return image;
}

bool Cam_Reader::get_image(cv::Mat &image){
    if(this->cap.isOpened()){
        mtx.lock();
        this->cap.retrieve(image);
        mtx.unlock();
    }
    this->underflow=true;
    while(this->miliseconds_cycle-t.elapsed()>0)
        this->underflow=false;
    this->fps_performance=1000./t.elapsed();
    t.restart();
    if(image.empty())
        return false;
    return true;
}

bool Cam_Reader::get_frame_rate(float &fps){
    fps=this->fps_performance;
    return (this->underflow && (this->miliseconds_cycle>0));
}
