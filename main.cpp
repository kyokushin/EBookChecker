#ifdef _DEBUG
#include "testdatapath.h"
#endif 

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace std;

int showImage(const cv::Mat& image, int wait = 0){
	static const string wname("EBookChecker");

	cv::imshow(wname, image);
	return cv::waitKey(wait);
}

int main(int argc, char** argv)
{


#ifndef _DEBUG
	string commandArgs =
		"@input |  | processing one image or image named serial number"
		;

	cv::CommandLineParser parser(argc, argv, commandArgs);

	string src = parser.get<string>(0);
#endif
	string src = TEST_DATA_0;

	cout << "input file:" << src << endl;
	

	cv::Mat image = cv::imread(src, CV_LOAD_IMAGE_GRAYSCALE);
	CV_Assert(image.channels() == 1 && image.type() == CV_8UC1);
	showImage(image);

	cv::Mat binary;
	int binaryMax = 255;
	int binaryThreshold = 128;
	cv::threshold(image, binary, binaryThreshold, binaryMax, cv::THRESH_BINARY_INV);
	CV_Assert(binary.channels() == 1 && image.type() == CV_8UC1);
	showImage(binary);
}