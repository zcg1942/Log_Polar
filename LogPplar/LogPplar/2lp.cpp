#include<iostream>
#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;
IplImage* rotateImage1(IplImage* img, int degree);

int main()
{
	IplImage *srcsrc = cvLoadImage("beaver.png");// , -1);cvLogPolar要求必须是三通道的
	//IplImage *src = cvCreateImage(cvSize(354, 300), 8, 3);
	////cvSetData
	//Mat Big = Mat::zeros(Size(354, 300), CV_8UC3);
	////cvResize(srcsrc, src);//要把原图放在中间，而不是缩放
	//CvRect ROI = (cvRect(27, 44.5, 300, 211));
	//cvSetImageROI(src, ROI);
	////cvAddWeighted()叠加和线性混合不一样，叠加用copyto
	//cvCopy(srcsrc, src);
	Mat src1 = imread("beaver.png");
	Mat Big = Mat::zeros(Size(354, 300), CV_8UC3);
	Mat ROI = Big(Rect(27, 44, 300, 211));
	src1.copyTo(ROI);
	IplImage srcipl = IplImage(Big);
	IplImage *src = &srcipl;



	IplImage *dst = cvCreateImage(cvGetSize(src), 8, 3);
	//IplImage *dstLiner = cvCreateImage(cvGetSize(src), 8, 3);
	IplImage* src2 = cvCreateImage(cvGetSize(src), 8, 3);
	//IplImage* src2Liner = cvCreateImage(cvGetSize(src), 8, 3);

	CvPoint2D32f center = cvPoint2D32f(src->width/2, src->height/2);
	//旋转中心为图像中心  
	//CvPoint2D32f center;
	//center.x = float(img->width / 2.0 + 0.5);
	//center.y = float(img->height / 2.0 + 0.5);这里为什么要加0.5？
	double m = 50.0;
	cvLogPolar(src, dst, center, m, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS);
	cvLogPolar(dst, src2, center, m, CV_INTER_LINEAR + CV_WARP_INVERSE_MAP);
	//CV_WARP_INVERSE_MAP - 表示矩阵由输出图像到输入图像的逆变换，并且因此可以直接用于像素插值。否则，函数从map_matrix中寻找逆变换。
	//cvLogPolar(src, dstLiner, center, m, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS);
	//cvLogPolar(dstLiner, src2Liner, center, m, CV_INTER_LINEAR + CV_WARP_INVERSE_MAP);

	//对原图旋转
	IplImage *RotateImage = rotateImage1(srcsrc, 20);
	cvNamedWindow("旋转图", CV_WINDOW_AUTOSIZE);
	cvShowImage("旋转图", RotateImage);
	//对旋转图极坐标变换
	IplImage *dstR = cvCreateImage(cvGetSize(src), 8, 3);
	cvLogPolar(RotateImage, dstR, center, m, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS);
	cvNamedWindow("旋转图极坐标", CV_WINDOW_AUTOSIZE);
	cvShowImage("旋转图极坐标", dstR);
	//对原图调整大小
	//IplImage *srcResize = cvCreateImage(cvSize(354, 300), 8, 3);
	//cvResize(src, srcResize);
	//cvNamedWindow("原图resize", CV_WINDOW_AUTOSIZE);
	//cvShowImage("原图resize", srcResize);
	//cvLogPolar(src, dst, center, m, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS);

	cvNamedWindow("原图", CV_WINDOW_AUTOSIZE);
	cvShowImage("原图", src);
	cvNamedWindow("变换后的图", CV_WINDOW_AUTOSIZE);
	cvShowImage("变换后的图", dst);

	//cvNamedWindow("线性极坐标变换后的图", CV_WINDOW_AUTOSIZE);
	//cvShowImage("线性极坐标变换后的图", dstLiner);
	cvNamedWindow("反变换后的图", CV_WINDOW_AUTOSIZE);
	cvShowImage("反变换后的图", src2);
	//cvNamedWindow("线性反变换后的图", CV_WINDOW_AUTOSIZE);
	//cvShowImage("线性反变换后的图", src2Liner);

	cvWaitKey();
	cvReleaseImage(&src);
	cvReleaseImage(&dst);
	//cvReleaseImage(&dstLiner);
	//cvReleaseImage(&src2Liner);
	cvReleaseImage(&RotateImage);
	cvReleaseImage(&dstR);
	cvReleaseImage(&src2);

	cvDestroyWindow("原图");
	cvDestroyWindow("变换后的图");
	//cvDestroyWindow("线性极坐标变换后的图");
	//cvDestroyWindow("线性反变换后的图");
	cvDestroyWindow("旋转图");
	cvDestroyWindow("旋转图极坐标");
	cvDestroyWindow("反变换后的图");

	return 0;
}
//旋转图像内容不变，尺寸相应变大  https://blog.csdn.net/qingzai_/article/details/51095297
IplImage* rotateImage1(IplImage* img, int degree){
	double angle = degree  * CV_PI / 180.; // 弧度    
	double a = sin(angle), b = cos(angle);
	int width = img->width;
	int height = img->height;
	int width_rotate = int(height * fabs(a) + width * fabs(b));//354
	int height_rotate = int(width * fabs(a) + height * fabs(b));//300
	//旋转数组map  
	// [ m0  m1  m2 ] ===>  [ A11  A12   b1 ]  
	// [ m3  m4  m5 ] ===>  [ A21  A22   b2 ]  
	float map[6];
	CvMat map_matrix = cvMat(2, 3, CV_32F, map);
	// 旋转中心  
	CvPoint2D32f center = cvPoint2D32f(width / 2, height / 2);
	cv2DRotationMatrix(center, degree, 1.0, &map_matrix);
	map[2] += (width_rotate - width) / 2;
	map[5] += (height_rotate - height) / 2;
	IplImage* img_rotate = cvCreateImage(cvSize(width_rotate, height_rotate), 8, 3);
	//对图像做仿射变换  
	//CV_WARP_FILL_OUTLIERS - 填充所有输出图像的象素。  
	//如果部分象素落在输入图像的边界外，那么它们的值设定为 fillval.  
	//CV_WARP_INVERSE_MAP - 指定 map_matrix 是输出图像到输入图像的反变换，  
	cvWarpAffine(img, img_rotate, &map_matrix, CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS, cvScalarAll(0));
	return img_rotate;
}
