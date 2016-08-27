#include <string>
#include <opencv2/opencv.hpp>

#include "cvutils.h"

using namespace std;

static const string wname("EBookChecker");

int showImage(const cv::Mat& image, int wait){
	static const string wname(wname);

	cv::imshow(wname, image);
	return cv::waitKey(wait);
}

void destroyWindow(){

	cv::destroyWindow(wname);
}
