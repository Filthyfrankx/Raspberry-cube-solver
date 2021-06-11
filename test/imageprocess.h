#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <math.h>
#include <iostream>
#include <string>
#include <map>
#define my_threshod1 30*CV_PI/180.0//阈值1
#define my_threshod2 60*CV_PI/180.0//阈值2
#define err_sum 15
const char color_temp[6] = {'U','D', 'L', 'R', 'F', 'B'};

using namespace cv;
class ImageProcess
{
public:
    ImageProcess();
    std::string get_side_color(void);
    std::string init_color_threshold(void);
    void hsv_thrshold_put(int num,int HL,int HH,int SL,int SH,int VL,int VH);
    std::string string_to_hsv_thres(std::string str_threshold);
private:
    int photo_err_count = 0;
    VideoCapture*  cap;
    std::string  color_side;
    int color_count[6];  //'U','D', 'L', 'R', 'F', 'B'//计数
    int hsv_threshold[6][2];//  L U B D F R

    //hl hh                          //sl sh vl vh
};

#endif // IMAGEPROCESS_H
