#ifndef cvutils_h
#define cvutils_h

#include <opencv2/opencv.hpp>

//画像表示用関数
int showImage(const cv::Mat& image, int wait = 0);

void destroyWindow();

#endif