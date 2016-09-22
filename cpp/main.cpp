﻿#ifdef _DEBUG
#include "testdatapath.h"
#endif 

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

#include <qdir.h>
#include <qstringlist.h>

#include "cvutils.h"
#include "ImageScrap.h"

using namespace std;

int main(int argc, char** argv)
{
#ifdef _DEBUG
	//コマンドライン引数の解析。デバッグの時は使わない
	string commandArgs =
		"@input |  | processing one image or image named serial number"
		;

	cv::CommandLineParser parser(argc, argv, commandArgs);

	string src = parser.get<string>(0);
#else
	//string src = TEST_DATA_0;
	string src = "C:\\Users\\kyokushin\\Pictures\\testData\\";
#endif

	QDir dir(src.c_str());
	if (!dir.exists()) {
		cerr << "failed to find files" << endl;
		return 1;
	}

	QFileInfoList fileList = dir.entryInfoList(QDir::Files);
	fileList.size();

	cout << "input file:" << src << endl;

	int waitTime = 10;

	vector<int> imageHeights;
	vector<cv::Mat> pageNumbers;
	for (int i = 0; i < fileList.size(); i++){
		string fname = fileList[i].absoluteFilePath().toStdString();
		cout << fname << endl;
		cv::Mat colorImage = cv::imread(fname);

		//画像の読み込み。グレースケール画像として読み込む
		cv::Mat image;
		cv::cvtColor(colorImage, image, CV_BGR2GRAY);

		ImageScrap scrapImage(image, ImageScrap::RANGE_ALL);
		//scrapImage.show();

		cv::Mat rowImage;
		scrapImage.getRow(0).copyTo(rowImage);
		imageHeights.push_back(rowImage.rows);//ページ番号と思われる領域を保存
		pageNumbers.push_back(rowImage);

		cout << "height:" << rowImage.rows << endl;
		showImage(rowImage, waitTime);
	}

	vector<int> sortedImageHeights;
	copy(imageHeights.begin(), imageHeights.end(), back_inserter(sortedImageHeights));

	sort(sortedImageHeights.begin(), sortedImageHeights.end());
	int start;
	int acceptHeight = 10;
	for (start = 0; sortedImageHeights[start] < acceptHeight; start++) {
	}

	int heightMed = sortedImageHeights[start + (sortedImageHeights.size() - start)/2];

	int minHeight = heightMed * 0.5;
	int maxHeight = heightMed * 2.0;
	cout << "height median:" << heightMed << endl;
	vector<int> isPageNumber;
	for (int i = 0; i < imageHeights.size(); i++){
		cout << "height:" << imageHeights[i] << endl;
		isPageNumber.push_back(minHeight <= imageHeights[i] && imageHeights[i] <= maxHeight);
	}

	for (int i = 0; i < isPageNumber.size(); i++){
		if (isPageNumber[i]){
			showImage(pageNumbers[i]);
		}
		else{
			continue;
			cv::Mat colorImage = cv::imread(fileList[i].absoluteFilePath().toStdString());
			showImage(colorImage);
		}
	}

	return 0;
}

void showNoneCharacterRange(const cv::Mat& image){
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
	drawRange(srcColor, horizontalRanges, ImageScrap::RANGE_COLS, horizontalRangeDst, colorHorizontal);
	showImage(horizontalRangeDst);
	cv::imwrite("horizontalDst.jpg", horizontalRangeDst);

	//縦方向
	//文字のない範囲を受け取る変数
	vector<Range> verticalRanges;
	findSameValueVertical(integral, verticalRanges);

	//文字のない範囲を書き込む画像
	cv::Mat verticalRangeDst;
	drawRange(srcColor, verticalRanges, ImageScrap::RANGE_ROWS, verticalRangeDst, colorVertical);
	showImage(verticalRangeDst);

	//縦横で文字のない範囲を書き込む
	cv::imwrite("verticalDst.jpg", verticalRangeDst);
	drawRange(horizontalRangeDst, verticalRanges, ImageScrap::RANGE_ROWS, horizontalRangeDst, colorVertical);
	showImage(horizontalRangeDst);
	cv::imwrite("horizontalVerticalDst.jpg", horizontalRangeDst);
}