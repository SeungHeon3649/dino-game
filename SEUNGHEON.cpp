#include "seungheon.h"                                                      //"seungheon.h"헤더파일 include
void triangle(Mat& src, Point point)                                        //삼각형을 그려주는 triangle 함수 정의(src배경, 삼각형의 중심좌표)
{
	vector<Point> pts;                                                      //1차원 vector 선언
	int size = 35;                                                          //삼각형의 중심좌표로부터 이동하는 크기 변수 선언

	pts.push_back(Point(point.x, point.y - size));                          //삼각형의 위쪽 꼭짓점
	pts.push_back(Point(point.x - (size / 2), point.y + (size / 2)));       //삼각형의 왼쪽 아래 꼭짓점
	pts.push_back(Point(point.x + (size / 2), point.y + (size / 2)));       //삼각형의 오른쪽 아래 꼭짓점

	fillPoly(src, pts, Scalar(0, 0, 0));                                    //삼각형을 그려주는 함수(src배경, pts좌표, 색)
}
void rect(Mat& src, Point point2)                                           //사각형을 그려주는 rect 함수 정의(src배경, 사각형의 좌상 좌표)
{
	rectangle(src, Rect(point2.x, point2.y, 40, 40), Scalar(0, 0, 0), -1);  //사각형 그려주는 함수(좌상 x,y좌표, x,y좌표부터 width height, 색, 굵기)
}
