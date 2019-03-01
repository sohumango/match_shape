#include "my.hpp"
#include <stdio.h>
#include <unistd.h>
#include <thread>

my::my(){

}
my::~my(){
}

byte * my::matToBytes(Mat image)
{
   int size = image.total() * image.elemSize();
   byte * buf = new byte[size];  // you will have to delete[] that later
   printf("mat buf size:%d \n", size);
   std::memcpy(buf,image.data,size * sizeof(byte));
   return buf;
}

Mat my::bytesToMat(byte * bytes,int width,int height, int channel)
{
    if (channel == 1) {
        Mat mat = Mat(height,width,CV_8UC1,bytes).clone(); // make a copy
        return mat;
    } else if( channel == 3){
        Mat mat = Mat(height,width,CV_8UC3,bytes).clone(); // make a copy
        return mat;
    } else {
        Mat mat = Mat(height,width,CV_8UC4,bytes).clone(); // make a copy
        return mat;
    }
}

void task1(std::string msg)
{
    system("/Users/wang/usr/local/bin/cliclick c:1610,100 w:8500");
}

void my::visualize(cv::Mat img) {
    cv::imshow("hello", img);
    cv::moveWindow("hello", 1600, 10);
    int key = cv::waitKey(0);
    if (  key != 27 )
    {
        system("/usr/bin/open .");
    }
}


bool my::saveimg(Mat matsrc, std::string fn)
{
    //char cwd[PATH_MAX];
    //getcwd(cwd, sizeof(cwd));
    //strcat(cwd, filename);
    //bool bsave = cv::imwrite("/Users/wang/oh/test/draw/build/dd.png", matsrc);
    bool bsave = cv::imwrite(fn.c_str(), matsrc);
    printf(bsave ? "true" : "false");
    return bsave;
}
