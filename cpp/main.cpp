#ifdef _DEBUG
#include "testdatapath.h"
#endif 

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

#include <qdir.h>
#include <qstring.h>
#include <qstringlist.h>

#include "cvutils.h"
#include "ImageScrap.h"

using namespace std;

enum PagePosition {
	TOP = 0,
	BOTTOM = 1
};
//#define TEST_DATA
int main(int argc, char** argv)
{
	PagePosition pagePos;
	//コマンドライン引数の解析。デバッグの時は使わない
	const string commandArgs =
		"{@input |  | directory including ebook (jpg or png)}"
		"{position | top | page number position. top or bottom}"
		;

	cv::CommandLineParser parser(argc, argv, commandArgs);

	string src = parser.get<string>(0);
	string posStr = parser.get<string>("position");
	if (posStr == "top") {
		pagePos = PagePosition::TOP;
		cout << "page number top" << endl;
	}
	else if (posStr == "bottom") {
		pagePos = PagePosition::BOTTOM;
		cout << "page number bottom" << endl;
	}
	else {
		cerr << "error: page position is incorrect. specify top or bottom. specified:" << posStr << endl;
		return 1;
	}
#ifdef TEST_DATA
	//string src = TEST_DATA_0;
	string src = "C:\\Users\\kyokushin\\Pictures\\testData_Top\\";
	pagePos = PagePosition::TOP;
	//string src = "C:\\Users\\kyokushin\\Pictures\\testData_Bottom\\";
	//pagePos = PagePosition::BOTTOM;
#endif

	QDir dir(QString::fromLocal8Bit(src.c_str()));//Qt用の文字コードに変換
	if (!dir.exists()) {
		cerr << "error directory not exists. entered directory is \"" << src << "\"" << endl;
		return 1;
	}

	QFileInfoList fileList = dir.entryInfoList(QDir::Files);
	fileList.size();

	cout << "input file:" << src << endl;

	int waitTime = 10;


	vector<int> imageHeights;
	vector<cv::Mat> pageNumbers;
	cv::Mat rowImage(1, 1, CV_8UC1);
	cv::Mat image;
	for (int i = 0; i < fileList.size(); i++){
		string fname(fileList[i].absoluteFilePath().toLocal8Bit().constData());
		cout << fname << endl;
		cv::Mat colorImage = cv::imread(fname);

		//画像の読み込み。グレースケール画像として読み込む
		cv::cvtColor(colorImage, image, CV_BGR2GRAY);

		ImageScrap scrapImage(image, ImageScrap::RANGE_ALL);
		//scrapImage.show();

		if (pagePos == PagePosition::TOP) {
			scrapImage.getRow(0).copyTo(rowImage);
		}
		else {
			scrapImage.getRow(scrapImage.getRows() - 1).copyTo(rowImage);
		}
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
	int maxHeight = heightMed * 1.5;
	cout << "height median:" << heightMed << endl;
	vector<int> isPageNumber;
	int totalHeight = 0;
	int maxWidth = 0;
	int acceptedNum = 0;
	for (int i = 0; i < imageHeights.size(); i++){
		cout << "height:" << imageHeights[i] << endl;
		bool accept = minHeight <= imageHeights[i] && imageHeights[i] <= maxHeight;
		isPageNumber.push_back(accept);
		if(accept){
			totalHeight += imageHeights[i];
			acceptedNum++;
			maxWidth = max(maxWidth, pageNumbers[i].cols);
		}
	}
	cout << "total height:" << totalHeight << endl;
	const int indexWidth = 100;
	cv::Mat allPageNumImage(totalHeight + acceptedNum, indexWidth + maxWidth, CV_8UC1);
	allPageNumImage.setTo(255);
	int currentTop = 0;
	stringstream sstr;
	for (int i = 0; i < isPageNumber.size(); i++){
		if (isPageNumber[i]){
			cv::Mat &page = pageNumbers[i];
			ImageScrap scrap(page, ImageScrap::RANGE_COLS);
			int numOfWords = scrap.getCols();
			vector<cv::Mat> wordImages;
			int pageImageWidth = 0;
			for (int h = 0; h < numOfWords; h++) {
				cv::Mat wordImage = scrap.getCol(h);
				wordImages.push_back(wordImage);
				pageImageWidth += wordImage.cols;
			}
			cv::Mat wordImage(page.rows, pageImageWidth + 2 * numOfWords, CV_8UC1);
			wordImage.setTo(255);
			int currentWidth = 0;
			for (int h = 0; h < wordImages.size(); h++) {
				cv::Mat& word = wordImages[h];
				word.copyTo(cv::Mat(wordImage, cv::Rect(currentWidth, 0, word.cols, word.rows)));
				currentWidth += word.cols + 2;
			}
			//showImage(wordImage);

			//showImage(page, waitTime);
			//cv::Mat tmpImage(allPageNumImage, cv::Rect(indexWidth, currentTop, page.cols, page.rows));
			//page.copyTo(tmpImage);
			cv::Mat tmpImage(allPageNumImage, cv::Rect(indexWidth, currentTop, wordImage.cols, wordImage.rows));
			wordImage.copyTo(tmpImage);

			sstr.str("");
			sstr << i << flush;
			cv::putText(allPageNumImage, sstr.str(), cv::Point(0, currentTop + page.rows),
				CV_FONT_HERSHEY_COMPLEX, 0.5,
				cv::Scalar(0, 0, 0), 1);
			currentTop += page.rows;
			cv::Mat(allPageNumImage, cv::Rect(0, currentTop, allPageNumImage.cols, 1)).setTo(0);
			currentTop++;
			showImage(tmpImage, waitTime);
		}
		else{
			continue;
			cv::Mat colorImage = cv::imread(fileList[i].absoluteFilePath().toStdString());
			showImage(colorImage);
		}
	}
	cout << "total" << endl;
	showImage(allPageNumImage);
	cv::imwrite("pagenumbers.png", allPageNumImage);

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