#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include <opencv2/opencv.hpp>
#include "my.hpp"
#include "w1.hpp"
#include "lodepng.h"
#include <iostream>
#include <cvaux.h>
#include <highgui.h>

using namespace cv;
using namespace std;
// 定数
const int tempThres = 48;           // テンプレート二値化閾値
const int srcThres  = 36;           // 入力画像二値化閾値
const double huMomentThres = 0.01;  // 形状マッチングの閾値

int sample();
int goodtrack();


int main() 
{
    cv::Mat img = cv::imread("res/src.png",  IMREAD_UNCHANGED);
    CV_Assert( img.cols > 0);
    cv::Rect myROI(100, 0, 100, 100);
    Mat crop = Mat(img, myROI);
    
    goodtrack();
    
    return 0;
}

int sample()
{
    cv::Mat temp = cv::imread("./res/temp.jpg", 0); // ニッカのおじさんの画像
    if (temp.empty()){
        std::cout << "テンプレート読み込みエラー" << std::endl;
        return -1;
    }
    
    cv::Mat src = cv::imread("./res/src.jpg"); // 撮影された画像
    if (src.empty()){
        std::cout << "入力画像読み込みエラー" << std::endl;
        return -1;
    }
    cv::Mat src_gray;
    cv::cvtColor(src, src_gray, CV_BGR2GRAY);
    
    // 二値化
    cv::Mat temp_bin, src_bin;
    cv::threshold(temp, temp_bin, tempThres, 255, cv::THRESH_BINARY_INV);
    cv::threshold(src_gray, src_bin, srcThres, 255, cv::THRESH_BINARY_INV);
    
    
    // ノイズ除去
    cv::morphologyEx(temp_bin, temp_bin, cv::MORPH_OPEN, cv::Mat(), cv::Point(-1, -1), 2);
    cv::morphologyEx(src_bin,  src_bin,  cv::MORPH_OPEN, cv::Mat(), cv::Point(-1, -1), 2);
    
    // ラベリング
    cv::Mat labelsImg;
    cv::Mat stats;
    cv::Mat centroids;
    int nLabels = cv::connectedComponentsWithStats(src_bin, labelsImg, stats, centroids);
    
    // ラベルごとのROIを得る(0番目は背景なので無視)
    cv::Mat roiImg;
    cv::cvtColor(src_bin, roiImg, CV_GRAY2BGR);
    std::vector<cv::Rect> roiRects;
    for (int i = 1; i < nLabels; i++) {
        int *param = stats.ptr<int>(i);
        
        int x = param[cv::ConnectedComponentsTypes::CC_STAT_LEFT];
        int y = param[cv::ConnectedComponentsTypes::CC_STAT_TOP];
        int height = param[cv::ConnectedComponentsTypes::CC_STAT_HEIGHT];
        int width = param[cv::ConnectedComponentsTypes::CC_STAT_WIDTH];
        roiRects.push_back(cv::Rect(x, y, width, height));
        
        cv::rectangle(roiImg, roiRects.at(i-1), cv::Scalar(0, 255, 0), 2);
    }
    
    // huモーメントによる形状マッチングを行う
    cv::Mat dst = src.clone();
    for (int i = 1; i < nLabels; i++){
        cv::Mat roi = src_bin(roiRects.at(i-1));    // 対象とするブロブの領域取り出し
        double similarity = cv::matchShapes(temp_bin, roi, CV_CONTOURS_MATCH_I1, 0);    // huモーメントによるマッチング
        
        if (similarity < huMomentThres){
            cv::rectangle(dst, roiRects.at(i - 1), cv::Scalar(0, 255, 0), 2);
        }
    }
    
//    cv::imshow("template", temp);
//    cv::imshow("src", src);
//    cv::imshow("dst", dst);
//    cv::imshow("temp", src_bin);
//    cv::waitKey();
//    cv::imwrite("dst.jpg", dst);
    my b;
    byte * buf = b.matToBytes(src);
    //imshow(buf, src.cols, src.rows, 3);
    delete [] buf;
    
    Mat a= cv::imread("res/a.png");
    //cv::imshow("a.png", a);
    //cv::waitKey();
    byte * a2 = b.matToBytes(a);
    //::imshow(a2, a.cols, a.rows, 3);
    delete [] a2;
    
    std::vector<unsigned char> png;
    std::vector<unsigned char> image; //the raw pixels
    unsigned w2, h2;
    unsigned error = lodepng::load_file(png, "res/ys.png");
    if(!error) error = lodepng::decode(image, w2, h2, png, LCT_RGBA);
    if(!error) {
        ::imshow(image.data(), w2, h2, 4, false);
    }
    
    return 0;
}

int goodtrack()
{
    // (1)load a specified file as a 3-channel color image
    //const char *imagename = "./res/library.png";
    const char *imagename = "./res/bar2_1.jpeg";
    Mat eigen_img = imread(imagename);
    if(!eigen_img.data)
        return -1;
    Mat harris_img = eigen_img.clone();
    Mat fast_img = eigen_img.clone();
    
    // (2)convert to a grayscale image and normalize it
    Mat gray_img;
    cvtColor(eigen_img, gray_img, CV_BGR2GRAY);
    
    Mat temp_bin;
    threshold(gray_img, temp_bin, 150, 255, cv::THRESH_BINARY_INV);
    threshold(temp_bin, gray_img, 100, 255, cv::THRESH_BINARY_INV);
    
    normalize(gray_img, gray_img, 0, 255, NORM_MINMAX);
    
    // (3)detect and draw strong corners on the image based on Eigen Value
    vector<Point2f> corners;
    goodFeaturesToTrack(gray_img, corners, 10, 0.01, 5);
    vector<Point2f>::iterator it_corner = corners.begin();
    for(; it_corner!=corners.end(); ++it_corner) {
        circle(eigen_img, Point(it_corner->x, it_corner->y), 1, Scalar(0,200,255), -1);
        circle(eigen_img, Point(it_corner->x, it_corner->y), 8, Scalar(0,200,255));
    }
    
    // (4)detect and draw strong corners on the image using Harris detector
    goodFeaturesToTrack(gray_img, corners, 10, 0.01, 3, Mat(), 3, true);
    it_corner = corners.begin();
    for(; it_corner!=corners.end(); ++it_corner) {
        circle(harris_img, Point(it_corner->x, it_corner->y), 1, Scalar(0,255,0), -1);
        circle(harris_img, Point(it_corner->x, it_corner->y), 8, Scalar(0,255,0 ));
    }
    
    // (5)detect corners using high-speed corner detection; FAST
    int threshold = 100;
    bool nonmax = true;
    vector<KeyPoint> keypoints;
    FAST(gray_img, keypoints, threshold, nonmax);
    vector<KeyPoint>::iterator it_kp = keypoints.begin();
    for(; it_kp!=keypoints.end(); ++it_kp) {
        circle(fast_img, Point(it_kp->pt.x, it_kp->pt.y), 1, Scalar(50,0,255), -1);
        circle(fast_img, Point(it_kp->pt.x, it_kp->pt.y), 8, Scalar(50,0,255));
    }
    
    // (6)show destination images, and quit when any key pressed
    namedWindow("EigenValue",CV_WINDOW_AUTOSIZE);
    namedWindow("Harris",CV_WINDOW_AUTOSIZE);
    namedWindow("Fast",CV_WINDOW_AUTOSIZE);
    imshow("EigenValue", eigen_img);
    imshow("Harris", harris_img);
    imshow("Fast", temp_bin);
    waitKey(0);
    
    return 0;
}

