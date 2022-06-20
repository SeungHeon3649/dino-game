#ifndef SEUNGHEON_H  
#define SEUNGHEON_H
#include <opencv2/opencv.hpp>
#include <iostream>
#include <Windows.h>
#pragma comment(lib,"winmm.lib")
using namespace cv;
using namespace cv::dnn;
using namespace std;
#define triangle_num 10                 //triangle_num를 10으로 정의
#define rect_num 10                     //rect_num을 10로 정의
void triangle(Mat& src, Point point);   //triangle 함수 선언
void rect(Mat& src, Point point2);      //rect 함수 선언
#endif
