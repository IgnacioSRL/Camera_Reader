#include "cam_reader.h"

Cam_Reader::Cam_Reader(string address, float fps, cv::Size image_size){
    this->initialized=false;
    if(!address.empty() && address!="0.0.0.0")
        initialization(address,fps);
}

bool Cam_Reader::initialization(string address, float fps, cv::Size image_size){
    this->mtx.lock();
    this->cap.open(address);
    if(fps>0)
        this->miliseconds_cycle=1000./fps;
    else
        this->miliseconds_cycle=0;
    this->image_size=image_size;
    this->cap.grab();
    std::thread thrd(&Cam_Reader::frame_reader,this);
    thrd.detach();
    this->t.start();
    this->initialized=this->cap.isOpened();
    this->mtx.unlock();
    return this->initialized;
}

bool Cam_Reader::is_initialized(){
    return this->initialized;
}

void Cam_Reader::frame_reader(){
    while(true){
        if(this->cap.isOpened()){
            this->mtx.lock();
            this->cap.grab();
            this->mtx.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

cv::Mat Cam_Reader::get_image(){
    cv::Mat image;
    if(this->cap.isOpened()){
        this->mtx.lock();
        this->cap.retrieve(image);
        this->mtx.unlock();
        if(this->image_size!=cv::Size())
            cv::resize(image,image,this->image_size);
    }
    this->underflow=true;
    while(this->miliseconds_cycle-t.elapsed()>0)
        this->underflow=false;
    this->fps_performance=1000./t.elapsed();
    this->t.restart();
    return image;
}

bool Cam_Reader::get_image(cv::Mat &image){
    if(this->cap.isOpened()){
        this->mtx.lock();
        this->cap.retrieve(image);
        this->mtx.unlock();
        if(this->image_size!=cv::Size())
            cv::resize(image,image,this->image_size);
    }
    this->underflow=true;
    while(this->miliseconds_cycle-t.elapsed()>0)
        this->underflow=false;
    this->fps_performance=1000./t.elapsed();
    this->t.restart();
    if(image.empty())
        return false;
    return true;
}

bool Cam_Reader::get_frame_rate(float &fps){
    fps=this->fps_performance;
    return (this->underflow && (this->miliseconds_cycle>0));
}
