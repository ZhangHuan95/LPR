#include "stdafx.h"
#include "LPR.h"

bool comp(RotatedRect a, RotatedRect b)
{
	return a.center.x < b.center.x;
}

float PR(Mat &src, Mat &dst, int num)
{
	int temp = 0;
	float result = 0;
// 	imshow("src", src);
// 	imshow("dst", dst);
// 	waitKey(0);

	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			if (src.ptr(i)[j] == dst.ptr(i)[j])
			{
				temp++;
			}
		}
	}
	result = (float)temp / (src.rows*src.cols);
	return result;
}

void Output(int num)
{
	if (num < 10)
	{
		cout << num << " ";
	}
	else
	{
		if (num == 10)
		{
			cout << "D" << " ";
		}
		else if (num == 11)
		{
			cout << "津" << " ";
		}
		else if (num == 12)
		{
			cout << "H" << " ";
		}
		else
		{
			cout << "*" << " ";
		}
	}
}
LPR::LPR()
{

}

LPR::LPR(String path)
{
	this->load(path);
}

void LPR::load(String path)
{
	//读取车牌图片
	srcImage = imread(path);
	if (srcImage.empty())
	{
		cout << "错误的路径!" << endl;
/*		exit(-1);*/
	}
	cvtColor(srcImage, markedImage, CV_16S);
	//载入数字图像
	for (int i = 0; i < template_len; i++)
	{
		stringstream stream;
		stream << "pictures/num_";
		stream << i;
		stream << ".bmp";
		String name = stream.str();
		NUM[i] = imread(name);
		if (NUM[i].empty())
		{
			cout << "未能读取" << name << endl;
		}
		threshold(NUM[i], NUM[i], 0, 255, CV_THRESH_BINARY);
	}
}

void LPR::showSrc()
{
	//显示原图
	imshow("原图",srcImage);
}

void LPR::gaussFilter()
{
	//高斯滤波
	GaussianBlur(srcImage, gaussImage, Size(3, 3), 0);
/*	imshow("【效果图】高斯滤波", gaussImage);*/
}

void LPR::sobel()
{
	//sobel运算
	//创建 grad_x 和 grad_y 矩阵  
	Mat grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y;

	//求 X方向梯度  
	Sobel(gaussImage, grad_x, CV_16S, 1, 0, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(grad_x, abs_grad_x);
/*	imshow("【效果图】 X方向Sobel", abs_grad_x);*/

	//求Y方向梯度  
	Sobel(gaussImage, grad_y, CV_16S, 0, 1, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(grad_y, abs_grad_y);
/*	imshow("【效果图】Y方向Sobel", abs_grad_y);*/

	//合并梯度(近似)  
	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, sobelImage);
/*	imshow("【效果图】整体方向Sobel", sobelImage);*/
}

void LPR::thresholding()
{
	//颜色空间转换函数
	cvtColor(sobelImage, binImage, CV_RGB2GRAY);

	//threshold 方法是通过遍历灰度图中点，将图像信息二值化，处理过后的图片只有二种色值。
	threshold(binImage, binImage, 180, 255, CV_THRESH_BINARY);
/*	imshow("【效果图】二值化处理", binImage);*/
}

void LPR::closeOperation()
{
	//定义核
	Mat element = getStructuringElement(MORPH_RECT, Size(17, 17));
	//进行形态学操作
	morphologyEx(binImage, closeImage, MORPH_CLOSE, element);
/*	imshow("【效果图】闭运算", closeImage);*/
}

void LPR::removeLittle()
{
	//腐蚀操作
	Mat ele = getStructuringElement(MORPH_RECT, Size(5, 5));//getStructuringElement返回值定义内核矩阵
	erode(closeImage, closeImage, ele);//erode函数直接进行腐蚀操作
/*	imshow("【效果图】去除小区域", closeImage);*/

	//膨胀操作
	ele = getStructuringElement(MORPH_RECT, Size(17, 5));//getStructuringElement返回值定义内核矩阵
	dilate(closeImage, closeImage, ele);//dilate函数直接进行膨胀操作
/*	imshow("【效果图】再膨胀", closeImage);*/
}

void LPR::getMaxArea()
{
	//取轮廓
	vector< vector< Point> > contours;
	findContours(closeImage,
		contours, // a vector of contours
		CV_RETR_EXTERNAL, // 提取外部轮廓
		CV_CHAIN_APPROX_NONE); // all pixels of each contours
	int max = 0;
	for (size_t i = 0; i < contours.size(); i++)
	{
		vector<Point> p;
		p = contours[i];
		if (p.size() > max)
		{
			max = p.size();
			maxArea = p;
		}
	}
	//绘制最大区域
	for (size_t i = 0; i < maxArea.size(); i++)
	{
		circle(markedImage, maxArea[i], 1, Scalar(240, 255, 25));
	}

	//绘制最小外接矩形
	minRect = minAreaRect(maxArea);
	minRect.points(P);
	for (int j = 0; j <= 3; j++)
	{
		line(markedImage, P[j], P[(j + 1) % 4], Scalar(255), 2);
	}
/*	imshow("【效果图】标记", markedImage);*/
}

void LPR::affine()
{
	//仿射变换
	Point2f srcTri[3];
	Point2f dstTri[3];
	Mat rot_mat(2, 3, CV_32FC1);
	Mat warp_mat(2, 3, CV_32FC1);

	//设置三个点来计算仿射变换
	//左上
	int tep = markedImage.cols;
	for (size_t i = 0; i < maxArea.size(); i++)
	{
		if (maxArea[i].x < tep)
		{
			tep = maxArea[i].x;
			srcTri[0] = maxArea[i];
		}
	}
	//左下
	tep = 0;
	for (size_t i = 0; i < maxArea.size(); i++)
	{
		if (maxArea[i].y > tep)
		{
			tep = maxArea[i].y;
			srcTri[1] = maxArea[i];
		}
	}
	//右下
	tep = 0;
	for (size_t i = 0; i < maxArea.size(); i++)
	{
		if (maxArea[i].x > tep)
		{
			tep = maxArea[i].x;
			srcTri[2] = maxArea[i];
		}
	}

	//Scalar(blue,green,red)
	circle(markedImage, srcTri[0], 5, Scalar(255));
	circle(markedImage, srcTri[1], 5, Scalar(0, 255));
	circle(markedImage, srcTri[2], 5, Scalar(0, 0, 255));

/*	imshow("【标记】车牌区域", markedImage);*/


	dstTri[0] = Point2f(0, 0);
	dstTri[1] = Point2f(0, minRect.size.height);
	dstTri[2] = Point2f(minRect.size.width, minRect.size.height);

	//计算仿射变换矩阵
	warp_mat = getAffineTransform(srcTri, dstTri);
	//对加载图形进行仿射变换操作
	warpAffine(gaussImage, plateImage, warp_mat, minRect.size);
/*	imshow("【效果图】仿射变换", plateImage);*/

}

void LPR::reThreshold()
{
	//再次二值化
	threshold(plateImage, reBinImage, 180, 255, CV_THRESH_BINARY_INV);
/*	imshow("再次二值化", reBinImage);*/
}

void LPR::recognition()
{

	cvtColor(reBinImage, reBinImage, COLOR_BGR2GRAY);
	cvtColor(plateImage, charMarkedImage, CV_16S);

	threshold(reBinImage, reBinImage, 48, 255, CV_THRESH_BINARY_INV);

	//运算操作
	Mat ele = getStructuringElement(MORPH_RECT, Size(3, 5));//getStructuringElement返回值定义内核矩阵
	dilate(reBinImage, reBinImage, ele);
/*	imshow("膨胀", reBinImage);*/

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(reBinImage, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	vector<RotatedRect> RectArr(license_len);
	int a = 0;
	//画出可包围数字的最小矩形
	for (vector<vector<Point>>::iterator It = contours.begin(); It < contours.end(); It++)
	{                        
		Point2f vertex[4];
		RotatedRect rect = minAreaRect(*It);
		if (rect.size.area() > 200)
		{
			RectArr[a] = rect;
			RectArr[a].points(vertex);
			a++;
			for (int j = 0; j < 4; j++)
				line(charMarkedImage, vertex[j], vertex[(j + 1) % 4], Scalar(0, 0, 255), 1);
		}
	}

	imshow("轮廓", charMarkedImage);

	Mat warp_mat(2, 3, CV_32FC1);
	sort(RectArr.begin(), RectArr.end(), comp);//对轮廓从左到右排序

	cout << "车牌：";
	for (int i = 0; i < RectArr.size(); i++)
	{
		Point2f vertex[4];
		RectArr[i].points(vertex);
		//仿射变换
		Point2f srcTri[3];
		Point2f dstTri[3];
		srcTri[0] = vertex[0];
		srcTri[1] = vertex[1];
		srcTri[2] = vertex[2];
		dstTri[0] = Point2f(0, 400);
		dstTri[1] = Point2f(0, 0);
		dstTri[2] = Point2f(400, 0);
		Mat warp_mat = getAffineTransform(srcTri, dstTri);
		//对加载图形进行仿射变换操作
		warpAffine(plateImage, charMat[i], warp_mat, Size(400, 400));

// 		stringstream stream;//		字符流
// 		stream << i + 1;
// 		String name = stream.str();

		//二值化
		threshold(charMat[i], charMat[i], 150, 255, CV_THRESH_BINARY_INV);

		float maxRate = 0, tempRate = 0;
		int mostLike;
		for (int j = 0; j < template_len; j++)
		{
			tempRate = PR(charMat[i], NUM[j], j);
/*			cout << setprecision(2) << tempRate<<" ";//输出匹配率*/
			if (tempRate > maxRate)
			{
				maxRate = tempRate;
				mostLike = j;
			}
		}
		Output(mostLike);

/*		imwrite(name+".bmp", charMat[i]);//保存提取的字符*/
	}
	cout << endl;
}

void LPR::processing()
{
	//高斯滤波
	gaussFilter();
	//sobel运算
	sobel();
	//二值化
	thresholding();
	//闭运算
	closeOperation();
	//去除小区域
	removeLittle();
	//取最大轮廓
	getMaxArea();
	//仿射变换
	affine();
	//再次二值化
	reThreshold();
	//识别字符
	recognition();
}


LPR::~LPR()
{
	destroyAllWindows();
}

