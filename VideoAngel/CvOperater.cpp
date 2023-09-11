#include "CvOperater.h"

cv::Mat rgb_2_mat(const void* rgb, const int& width, const int& height)
{
	cv::Mat mat(height, width, CV_8UC3);
	mat.data = (uchar *)rgb;
	cv::cvtColor(mat, mat, CV_RGB2GRAY);
	cv::Canny(mat, mat, 50, 50 * 2);
	memset((void *)rgb, 0, width * height * 3);
	int size = (unsigned long long)mat.dataend - (unsigned long long)mat.datastart;
	memcpy((void*)rgb, mat.data, size);
	cv::imshow("123", mat);
	return cv::Mat();
}
