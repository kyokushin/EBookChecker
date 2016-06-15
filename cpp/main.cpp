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

class Range{
public:
	int start;
	int end;
	Range(int start, int end) :start(start), end(end){}
	Range():start(-1), end(-1){}
};

//横方向をしらべる
//文字のない範囲をvectorで返す
void findSameValueHorizontal(const cv::Mat& src, std::vector<Range>& ranges)
{
	//積分画像が欲しい
	CV_Assert(src.type() == CV_32SC1);

	//値が入っているかもしれないので空にする
	ranges.clear();

	//積分画像はintなのでint型のポインタを取得。下端なので位置はsrc.rows - 1
	const int* srcLine = src.ptr<int>(src.rows - 1);

	Range range;
	for (int i = 1; i < src.cols; i++){
		//左隣と同じ値
		bool sameValue = srcLine[i] == srcLine[i - 1];
		//左隣と同じ値 かつ 範囲のstartが初期値（-1）のとき
		if (sameValue && range.start < 0){
			//文字のない範囲の始まり
			range.start = i - 1;
		}
		//左隣と違う値 かつ 範囲のstartが代入済み
		else if (!sameValue && range.start >= 0){
			
			//文字のない範囲の終わり
			range.end = i - 1;
			//結果として保存
			ranges.push_back(range);
			//文字のない範囲を初期値に戻す
			range.start = -1;
			range.end = -1;
		}
	}
	//最後の範囲が画像の右端まである場合はfor文を抜けてから結果を保存する
	//文字のない範囲のstartは代入済み かつ 範囲のendは初期値のとき
	if (range.start >= 0 && range.end < 0){
		range.end = src.cols - 1;
		ranges.push_back(range);
	}
}

//縦方向をしらべる
//文字のない範囲をvectorで返す
void findSameValueVertival(const cv::Mat& src, std::vector<Range>& ranges)
{
	//積分画像が欲しい
	CV_Assert(src.type() == CV_32SC1);

	//値が入っているかもしれないので空にする
	ranges.clear();


	Range range;

	const int endPos = src.cols - 1;
	int src0 = src.ptr<int>(0)[endPos];
	int src1 = src.ptr<int>(1)[endPos];
	if (src0 == src1){
		range.start = 0;
	}
	src0 = src1;

	for (int i = 1; i < src.rows; i++){
		src1 = src.ptr<int>(i)[endPos];

		//上隣と同じ値
		bool sameValue = src0 == src1;
		//左隣と同じ値 かつ 範囲のstartが初期値（-1）のとき
		if (sameValue && range.start < 0){
			//文字のない範囲の始まり
			range.start = i - 1;
		}
		//左隣と違う値 かつ 範囲のstartが代入済み
		else if (!sameValue && range.start >= 0){

			//文字のない範囲の終わり
			range.end = i - 1;
			//結果として保存
			ranges.push_back(range);
			//文字のない範囲を初期値に戻す
			range.start = -1;
			range.end = -1;
		}

		src0 = src1;
	}
	//最後の範囲が画像の右端まである場合はfor文を抜けてから結果を保存する
	//文字のない範囲のstartは代入済み かつ 範囲のendは初期値のとき
	if (range.start >= 0 && range.end < 0){
		range.end = src.cols - 1;
		ranges.push_back(range);
	}
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

	//文字のない範囲を書き込む画像
	cv::Mat horizontalRangeDst;
	//1チャンネルの原画像から3チャンネルの画像を作る
	cv::Mat srcArray[] = {image, image, image};
	cv::merge(srcArray, 3, horizontalRangeDst);

	//文字のない範囲を3チャンネルの原画像に書き込む
	for (size_t i = 0; i < horizontalRanges.size(); i++){
		Range& r = horizontalRanges[i];
		//文字のない範囲を3チャンネルの原画像から切り出す
		cv::Rect rect(r.start, 0, r.end - r.start + 1, horizontalRangeDst.rows);
		cv::Mat roi(horizontalRangeDst, rect);
		//切り出した画像を1色で塗りつぶす
		roi = cv::Scalar(240, 176, 0);
	}


	showImage(horizontalRangeDst);
	cv::imwrite("horizontalDst.jpg", horizontalRangeDst);

	//縦方向
	//文字のない範囲を受け取る変数
	vector<Range> verticalRanges;
	findSameValueVertival(integral, verticalRanges);

	//文字のない範囲を書き込む画像
	cv::Mat verticalRangeDst;
	//1チャンネルの原画像から3チャンネルの画像を作る
	cv::merge(srcArray, 3, verticalRangeDst);

	//文字のない範囲を3チャンネルの原画像に書き込む
	for (size_t i = 0; i < verticalRanges.size(); i++){
		Range& r = verticalRanges[i];
		//文字のない範囲を3チャンネルの原画像から切り出す
		cv::Rect rect(0, r.start, verticalRangeDst.cols, r.end - r.start + 1);
		cv::Mat roi(verticalRangeDst, rect);
		//切り出した画像を1色で塗りつぶす
		roi = cv::Scalar(0, 0, 255);
	}

	showImage(verticalRangeDst);
	cv::imwrite("verticalDst.jpg", verticalRangeDst);


	//横方向の結果と縦方向の結果を合わせる
	for (size_t i = 0; i < verticalRanges.size(); i++){
		Range& r = verticalRanges[i];
		//文字のない範囲を3チャンネルの原画像から切り出す
		cv::Rect rect(0, r.start, horizontalRangeDst.cols, r.end - r.start + 1);
		cv::Mat roi(horizontalRangeDst, rect);
		//切り出した画像を1色で塗りつぶす
		roi = cv::Scalar(0, 0, 255);
	}

	showImage(horizontalRangeDst);
	cv::imwrite("horizontalVerticalDst.jpg", horizontalRangeDst);
}