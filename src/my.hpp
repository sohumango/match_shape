#ifndef __cv_my_h
#define __cv_my_h

#include <opencv2/opencv.hpp>
#include <string>

using namespace cv;
typedef unsigned char byte;

class my
{
private:    
public:
    my();    
    ~my();
public:
    static byte * matToBytes(Mat image);
    static Mat bytesToMat(byte * bytes,int width,int height, int channel);
    static void visualize(cv::Mat img);
    static bool saveimg(Mat matsrc, std::string filename);
};




#endif

