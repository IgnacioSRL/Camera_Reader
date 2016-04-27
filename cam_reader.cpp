#include "cam_reader.h"

Cam_Reader::Cam_Reader(std::string address, float fps, cv::Size image_size){
    this->initialized=false;
    initialization(address, fps, image_size);
}

Cam_Reader::~Cam_Reader(){
    stop_running();
}

void Cam_Reader::stop_running(){
    mtx.lock();
    this->running=false;
    this->initialized=false;
    this->cap.release();
    this->fps_performance=0;
    this->image_size=cv::Size();
    mtx.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

bool Cam_Reader::initialization(std::string address, float fps, cv::Size image_size){
    if(!address.empty() && address!="0.0.0.0" && !is_initialized()){
        this->mtx.lock();
        std::stringstream ss;
        ss<<address;
        int num = 0;
        ss >> num;
        if(ss.good() || (num==0 && ss.str()!="0"))
            this->cap.open(address);
        else
            this->cap.open(num);
        if(fps>0)
            this->miliseconds_cycle=1000./fps;
        else
            this->miliseconds_cycle=0;
        this->image_size=image_size;
        this->running=true;
        this->cap.grab();
        this->thrd=std::thread(&Cam_Reader::frame_reader,this);
        this->thrd.detach();
        this->t.start();
        this->initialized=this->cap.isOpened();
        this->mtx.unlock();
    }
    return this->initialized;
}

bool Cam_Reader::is_initialized(){
    this->mtx.lock();
    bool init=this->initialized;
    this->mtx.unlock();
    return init;
}

void Cam_Reader::frame_reader(){
    while(this->running){
        if(is_initialized()){
            this->mtx.lock();
            this->cap.grab();
            this->mtx.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

cv::Mat Cam_Reader::get_image(){
    cv::Mat image;
    if(is_initialized()){
        this->mtx.lock();
        this->cap.retrieve(image);
        this->mtx.unlock();
        if(this->image_size!=cv::Size())
            cv::resize(image,image,this->image_size);
    }
    this->underflow=true;
    while(this->miliseconds_cycle-this->t.elapsed()>0)
        this->underflow=false;
    this->fps_performance=1000./this->t.elapsed();
    this->t.restart();
    return image;
}

bool Cam_Reader::get_image(cv::Mat &image){
    if(is_initialized()){
        this->mtx.lock();
        this->cap.retrieve(image);
        this->mtx.unlock();
        if(this->image_size!=cv::Size())
            cv::resize(image,image,this->image_size);
    }
    this->underflow=true;
    while(this->miliseconds_cycle-this->t.elapsed()>0)
        this->underflow=false;
    this->fps_performance=1000./this->t.elapsed();
    this->t.restart();
    if(image.empty())
        return false;
    return true;
}

bool Cam_Reader::get_frame_rate(float &fps){
    this->mtx.lock();
    fps=this->fps_performance;
    bool ret=this->underflow && (this->miliseconds_cycle>0);
    this->mtx.unlock();
    return ret;
}
