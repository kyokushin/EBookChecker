#ifdef _DEBUG
#include "testdatapath.h"
#endif 

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace std;

//画像表示用関数
int showImage(const cv::Mat& image, int wait = 0){
	static const string wname("EBookChecker");

	cv::imshow(wname, image);
	return cv::waitKey(wait);
}

int main(int argc, char** argv)
{


#ifndef _DEBUG
	//コマンドライン引数の解析。デバッグの時は使わない
	string commandArgs =
		"@input |  | processing one image or image named serial number"
		;

	cv::CommandLineParser parser(argc, argv, commandArgs);

	string src = parser.get<string>(0);
#else
	string src = TEST_DATA_0;
#endif

	cout << "input file:" << src << endl;
	

	//画像の読み込み。グレースケール画像として読み込む
	cv::Mat image = cv::imread(src, CV_LOAD_IMAGE_GRAYSCALE);
	CV_Assert(image.channels() == 1 && image.type() == CV_8UC1);
	showImage(image);

	//二値化
	cv::Mat binary;
	int binaryMax = 1;//二値化時の最大値は1に。積分するときに白だったところか黒だったところかがわかればいい。
	int binaryThreshold = 128;
	cv::threshold(image, binary, binaryThreshold, binaryMax, cv::THRESH_BINARY_INV);
	CV_Assert(binary.channels() == 1 && image.type() == CV_8UC1);
	showImage(binary);
	cv::imwrite("binary.jpg", binary);

	//積分画像の生成
	cv::Mat integral;
	cv::integral(binary, integral);
	CV_Assert(integral.channels() == 1 && integral.type() == CV_32SC1);
	showImage(integral);
	cv::imwrite("integral.jpg", integral);

	//積分画像を見やすくする処理
	cv::Mat integralVisible;
	double max;
	double min;
	cv::minMaxLoc(integral, &min, &max);//最大値だけほしいので第3引数まで。最小値は0のはずだが本当に0か確認するために使う
	CV_Assert(min == 0.0);//本当に最小値が0になっているか確認

	integral.convertTo(integralVisible, CV_8UC1, 255.0 / max); //betaは使わない。0-255の値をとるようにalphaを与える。
	showImage(integralVisible);
	cv::imwrite("integralVisible.jpg", integralVisible);

}