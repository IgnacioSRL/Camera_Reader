# Camera_Reader

Description
-----------
Cam_Reader is a C ++ library for reading OpenCV images from a streaming video (such as an IP camera) in an easy way.
The library solves the problem caused by VideoCapture buffer. This means that the library always gives the ltest captured image.
The method can be configured to fix the fps desired. If needed it will wait to keep up with the same fps.

Usage
-----------
### Compilation
For the compilation you need to link QT and OpenCV libraries. Besides, compiler and library support for the ISO C++ 2011 standard is required. This support is currently experimental, and must be enabled with the -std=c++11 or -std=gnu++11 compiler options.

### Example
```
	#include "cam_reader.h"
	int main()
	{
		Cam_Reader cam_reader();
		cam_reader.initialization("your_camera_address",fps_desired)
		
		while(true){
			cv::Mat image=cam_reader.get_image();
			float fps;
			bool underflow=cam_reader.get_frame_rate(fps);
			cout<<"fps="<<fps<<"          underflow="<<underflow<<endl;
		}
	}
```
