# LPR(Vehicle License Plate Recognition)

## 前言

#### 这是一个车牌识别的实例

我的平台是vs2017和opencv3.4

opencv下载链接[点这里][]

[点这里]:https://opencv.org/releases.html

写这个是因为实验课布置了一个车牌识别的作业，
但是网上车牌识别的教程都很复杂，也没有太实际。
对于完全没有接触过的人，完全是一头雾水。
所以我在摸索了一个星期后，终于把这个车牌识别了出来，
希望给新手一个指导，怎么一步步地实现一个简单的车牌识别。
(暂时写个大概，有时间后继续完善！觉得有用的话可以给我一个star)
***
>任务:识别下图中的车牌
>
>![car](pictures/car.bmp)


	要实现车牌识别，一共分为两大步。
	第一，车牌定位，就是在图片中确定出车牌的位置；
	第二，字符识别，将提取出来的字符图片进行识别。

## 正文

#### 一、车牌定位

##### 1. 高斯滤波
![car](pictures/高斯滤波.jpg)
##### 2. sobel边缘提取
![car](pictures/sobel图像.jpg)
##### 3.二值化图像
![car](pictures/二值化处理.jpg)
##### 4.闭运算
![car](pictures/闭运算.jpg)
##### 5.去除小区域
![car](pictures/去除小区域.jpg)
![car](pictures/填充.jpg)
##### 6.提取轮廓
![car](pictures/车牌区域.jpg)
##### 7.仿射变换
![car](pictures/仿射变换.jpg)

#### 二、字符识别

##### 1.提取字符轮廓
![car](pictures/字符轮廓.jpg)
##### 2.识别字符
![car](pictures/计算匹配度.jpg)
![car](pictures/输出车牌结果.jpg)