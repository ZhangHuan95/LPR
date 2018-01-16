#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#define template_len 13
#define license_len 7

using namespace cv;
using namespace std;

class LPR
{
public:
	LPR();
	LPR(String path);
	void load(String path);//加载图像
	void showSrc();//显示原图
	void gaussFilter();//高斯滤波
	void sobel();//sobel运算
	void thresholding();//二值化
	void closeOperation();//闭运算
	void removeLittle();//形态处理
	void getMaxArea();//找最大轮廓
	void affine();//仿射变换
	void reThreshold();//再次二值化
	void recognition();//寻找字符
	void processing();//处理
	~LPR();

private:
	Mat srcImage;//原图
	Mat gaussImage;//高斯滤波后的图像
	Mat sobelImage;//sobel计算后的图像
	Mat binImage;//二值化图像
	Mat reBinImage;//再次二值化的图像
	Mat closeImage;//闭运算后图像
	Mat markedImage;//标记
	Mat plateImage;//车牌区域
	Mat charMarkedImage;//字符标记
	String license;//车牌
	vector<Point> maxArea;//最大区域
	RotatedRect minRect;//最小外接矩形
	Point2f P[4];//矩形的顶点
	Mat NUM[template_len];//字符匹配模板
	Mat charMat[license_len];//提取的字符
};
