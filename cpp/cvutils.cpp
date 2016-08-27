#include <string>
#include <opencv2/opencv.hpp>

#include "cvutils.h"

using namespace std;

int showImage(const cv::Mat& image, int wait = 0){
	static const string wname("EBookChecker");

	cv::imshow(wname, image);
	return cv::waitKey(wait);
}
