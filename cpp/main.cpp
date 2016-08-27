#ifdef _DEBUG
#include "testdatapath.h"
#endif 

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

#include "ImageScrap.h"

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
	CV_Assert(binary.channels() == 1 && binary.type() == CV_8UC1);
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


	//横方向
	//文字のない範囲を受け取る変数
	vector<Range> horizontalRanges;
	findSameValueHorizontal(integral, horizontalRanges);

	//1チャンネルの原画像から3チャンネルの画像を作る
	cv::Mat srcArray[] = {image, image, image};
	cv::Mat srcColor;
	cv::merge(srcArray, 3, srcColor);

	const cv::Scalar colorVertical(0, 0, 255);
	const cv::Scalar colorHorizontal(240, 176, 0);

	//文字のない範囲を書き込む画像
	cv::Mat horizontalRangeDst;
	drawRange(srcColor, horizontalRanges, ImageScrap::RANGE_HORIZONTAL, horizontalRangeDst, colorHorizontal);

	/*
	//文字のない範囲を3チャンネルの原画像に書き込む
	for (size_t i = 0; i < horizontalRanges.size(); i++){
		Range& r = horizontalRanges[i];
		//文字のない範囲を3チャンネルの原画像から切り出す
		cv::Rect rect(r.start, 0, r.end - r.start, horizontalRangeDst.rows);
		cv::Mat roi(horizontalRangeDst, rect);
		//切り出した画像を1色で塗りつぶす
		roi = cv::Scalar(240, 176, 0);
	}
	*/


	showImage(horizontalRangeDst);
	cv::imwrite("horizontalDst.jpg", horizontalRangeDst);

	//縦方向
	//文字のない範囲を受け取る変数
	vector<Range> verticalRanges;
	findSameValueVertical(integral, verticalRanges);

	//文字のない範囲を書き込む画像
	cv::Mat verticalRangeDst;
	drawRange(srcColor, verticalRanges, ImageScrap::RANGE_VERTICAL, verticalRangeDst, colorVertical);
	/**
	//文字のない範囲を3チャンネルの原画像に書き込む
	for (size_t i = 0; i < verticalRanges.size(); i++){
		Range& r = verticalRanges[i];
		//文字のない範囲を3チャンネルの原画像から切り出す
		cv::Rect rect(0, r.start, verticalRangeDst.cols, r.end - r.start);
		cv::Mat roi(verticalRangeDst, rect);
		//切り出した画像を1色で塗りつぶす
		roi = cv::Scalar(0, 0, 255);
	}
	*/

	showImage(verticalRangeDst);
	cv::imwrite("verticalDst.jpg", verticalRangeDst);

	drawRange(horizontalRangeDst, verticalRanges, ImageScrap::RANGE_VERTICAL, horizontalRangeDst, colorVertical);

	/**
	//横方向の結果と縦方向の結果を合わせる
	for (size_t i = 0; i < verticalRanges.size(); i++){
		Range& r = verticalRanges[i];
		//文字のない範囲を3チャンネルの原画像から切り出す
		cv::Rect rect(0, r.start, horizontalRangeDst.cols, r.end - r.start);
		cv::Mat roi(horizontalRangeDst, rect);
		//切り出した画像を1色で塗りつぶす
		roi = cv::Scalar(0, 0, 255);
	}
	*/

	showImage(horizontalRangeDst);
	cv::imwrite("horizontalVerticalDst.jpg", horizontalRangeDst);
}