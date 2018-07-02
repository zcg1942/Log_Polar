#include<iostream>
#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;
IplImage* rotateImage1(IplImage* img, int degree);

int main()
{
	IplImage *srcsrc = cvLoadImage("beaver.png");// , -1);cvLogPolarҪ���������ͨ����
	//IplImage *src = cvCreateImage(cvSize(354, 300), 8, 3);
	////cvSetData
	//Mat Big = Mat::zeros(Size(354, 300), CV_8UC3);
	////cvResize(srcsrc, src);//Ҫ��ԭͼ�����м䣬����������
	//CvRect ROI = (cvRect(27, 44.5, 300, 211));
	//cvSetImageROI(src, ROI);
	////cvAddWeighted()���Ӻ����Ի�ϲ�һ����������copyto
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
	//��ת����Ϊͼ������  
	//CvPoint2D32f center;
	//center.x = float(img->width / 2.0 + 0.5);
	//center.y = float(img->height / 2.0 + 0.5);����ΪʲôҪ��0.5��
	double m = 50.0;
	cvLogPolar(src, dst, center, m, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS);
	cvLogPolar(dst, src2, center, m, CV_INTER_LINEAR + CV_WARP_INVERSE_MAP);
	//CV_WARP_INVERSE_MAP - ��ʾ���������ͼ������ͼ�����任��������˿���ֱ���������ز�ֵ�����򣬺�����map_matrix��Ѱ����任��
	//cvLogPolar(src, dstLiner, center, m, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS);
	//cvLogPolar(dstLiner, src2Liner, center, m, CV_INTER_LINEAR + CV_WARP_INVERSE_MAP);

	//��ԭͼ��ת
	IplImage *RotateImage = rotateImage1(srcsrc, 20);
	cvNamedWindow("��תͼ", CV_WINDOW_AUTOSIZE);
	cvShowImage("��תͼ", RotateImage);
	//����תͼ������任
	IplImage *dstR = cvCreateImage(cvGetSize(src), 8, 3);
	cvLogPolar(RotateImage, dstR, center, m, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS);
	cvNamedWindow("��תͼ������", CV_WINDOW_AUTOSIZE);
	cvShowImage("��תͼ������", dstR);
	//��ԭͼ������С
	//IplImage *srcResize = cvCreateImage(cvSize(354, 300), 8, 3);
	//cvResize(src, srcResize);
	//cvNamedWindow("ԭͼresize", CV_WINDOW_AUTOSIZE);
	//cvShowImage("ԭͼresize", srcResize);
	//cvLogPolar(src, dst, center, m, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS);

	cvNamedWindow("ԭͼ", CV_WINDOW_AUTOSIZE);
	cvShowImage("ԭͼ", src);
	cvNamedWindow("�任���ͼ", CV_WINDOW_AUTOSIZE);
	cvShowImage("�任���ͼ", dst);

	//cvNamedWindow("���Լ�����任���ͼ", CV_WINDOW_AUTOSIZE);
	//cvShowImage("���Լ�����任���ͼ", dstLiner);
	cvNamedWindow("���任���ͼ", CV_WINDOW_AUTOSIZE);
	cvShowImage("���任���ͼ", src2);
	//cvNamedWindow("���Է��任���ͼ", CV_WINDOW_AUTOSIZE);
	//cvShowImage("���Է��任���ͼ", src2Liner);

	cvWaitKey();
	cvReleaseImage(&src);
	cvReleaseImage(&dst);
	//cvReleaseImage(&dstLiner);
	//cvReleaseImage(&src2Liner);
	cvReleaseImage(&RotateImage);
	cvReleaseImage(&dstR);
	cvReleaseImage(&src2);

	cvDestroyWindow("ԭͼ");
	cvDestroyWindow("�任���ͼ");
	//cvDestroyWindow("���Լ�����任���ͼ");
	//cvDestroyWindow("���Է��任���ͼ");
	cvDestroyWindow("��תͼ");
	cvDestroyWindow("��תͼ������");
	cvDestroyWindow("���任���ͼ");

	return 0;
}
//��תͼ�����ݲ��䣬�ߴ���Ӧ���  https://blog.csdn.net/qingzai_/article/details/51095297
IplImage* rotateImage1(IplImage* img, int degree){
	double angle = degree  * CV_PI / 180.; // ����    
	double a = sin(angle), b = cos(angle);
	int width = img->width;
	int height = img->height;
	int width_rotate = int(height * fabs(a) + width * fabs(b));//354
	int height_rotate = int(width * fabs(a) + height * fabs(b));//300
	//��ת����map  
	// [ m0  m1  m2 ] ===>  [ A11  A12   b1 ]  
	// [ m3  m4  m5 ] ===>  [ A21  A22   b2 ]  
	float map[6];
	CvMat map_matrix = cvMat(2, 3, CV_32F, map);
	// ��ת����  
	CvPoint2D32f center = cvPoint2D32f(width / 2, height / 2);
	cv2DRotationMatrix(center, degree, 1.0, &map_matrix);
	map[2] += (width_rotate - width) / 2;
	map[5] += (height_rotate - height) / 2;
	IplImage* img_rotate = cvCreateImage(cvSize(width_rotate, height_rotate), 8, 3);
	//��ͼ��������任  
	//CV_WARP_FILL_OUTLIERS - ����������ͼ������ء�  
	//�������������������ͼ��ı߽��⣬��ô���ǵ�ֵ�趨Ϊ fillval.  
	//CV_WARP_INVERSE_MAP - ָ�� map_matrix �����ͼ������ͼ��ķ��任��  
	cvWarpAffine(img, img_rotate, &map_matrix, CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS, cvScalarAll(0));
	return img_rotate;
}
