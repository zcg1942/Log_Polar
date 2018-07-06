#include<iostream>
#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;
int main()
{
	Mat img, imgcenter,warp_rotate_dst;
	Mat rot_mat(2, 3, CV_32FC1);
	img = imread("lena.jpg");
	if (img.data == 0)
	{
	   cout << "读取图像失败" << endl;
	   getchar();
	   return 0;
    }
	/*imgcenter = Mat::zeros(width_rotate, height_rotate, img.type());
	Mat ROI = imgcenter(Rect((width_rotate - img.cols) / 2, (height_rotate - img.rows) / 2, img.cols, img.rows));*/
	imgcenter = Mat::zeros(img.rows*2, img.cols*2, img.type());//纵坐标在前面
	warp_rotate_dst = Mat::zeros(img.rows*2, img.cols*2, img.type());
	Mat ROI = imgcenter(Rect( img.cols/ 2,  img.rows / 2, img.cols, img.rows));
	/// 计算绕图像中点顺时针旋转50度缩放因子为0.6的旋转矩阵
	//Point center = Point(imgcenter.cols / 2, imgcenter.rows / 2);
	Point Rotationcenter = Point(imgcenter.cols / 2, imgcenter.rows / 2);//取成小图中心作原点试试看
	Point LPcenter = Point(imgcenter.cols / 2, imgcenter.rows / 2);//取成小图中心作原点试试看
	//Point LPcenter = Point(0, imgcenter.rows / 2);//取成小图中心作原点试试看
	double degree = -70.0;
	double scale = 1.0;

	//保证旋转之后完全显示
	double angle = degree * CV_PI / 180.; // 转化为弧度    
	double a = sin(angle), b = cos(angle);
	int width =img.cols;
	int height = img.rows;
	int width_rotate = int(height * fabs(a) + width * fabs(b));
	int height_rotate = int(width * fabs(a) + height * fabs(b));
	
	Mat rotate_dst = Mat::zeros(img.cols, img.rows, img.type());

	
	img.copyTo(ROI);
	imshow("原图", imgcenter);
	/// 通过上面的旋转细节信息求得旋转矩阵
	rot_mat = getRotationMatrix2D(Rotationcenter, degree, scale);//这里的角度的单位是度

	/// 旋转已扭曲图像
	warpAffine(imgcenter, warp_rotate_dst, rot_mat, warp_rotate_dst.size());//这里要对填充后的图像旋转
	imshow("旋转后的图像", warp_rotate_dst);
	//waitKey();
	IplImage *src = &IplImage(imgcenter);//将Mat转化为iplimage
	IplImage *RotateImage = &IplImage(warp_rotate_dst);
	IplImage *dstR = cvCreateImage(cvGetSize(src), 8, 3);
	IplImage *dst = cvCreateImage(cvGetSize(src), 8, 3);
	//IplImage *img_ipl = &IplImage(img);
	//IplImage *dst = cvCreateImage(cvGetSize(img_ipl), 8, 3);
	double m = 50.0;
	cvLogPolar(src, dst, LPcenter, m, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS);
	cvLogPolar(RotateImage, dstR, LPcenter, m, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS);
	Mat src_LogP(dst);
	Mat Ro_LogP(dstR);
	imshow("原图极坐标", src_LogP);
	imshow("旋转图极坐标", Ro_LogP);
	//傅里叶变换

	//转为灰度图
	Mat src_LogPG, Ro_LogPG;
	cvtColor(src_LogP, src_LogP, CV_BGR2GRAY);
	cvtColor(Ro_LogP, Ro_LogP, CV_BGR2GRAY);
	src_LogP.convertTo(src_LogPG, CV_32FC1);
	Ro_LogP.convertTo(Ro_LogPG, CV_32FC1);
	Point2d phase_shift;
	phase_shift = phaseCorrelate(src_LogPG, Ro_LogPG);
	cout << endl << "warp :" << endl << "\tX shift : " << phase_shift.x << "\tY shift : " << phase_shift.y << endl;
	waitKey();
}