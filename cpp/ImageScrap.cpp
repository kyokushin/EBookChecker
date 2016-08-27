#include <opencv2/opencv.hpp>
#include <vector>

#include "ImageScrap.h"

using namespace std;

const int ImageScrap::RANGE_ALL(0);
const int ImageScrap::RANGE_ROWS(1);
const int ImageScrap::RANGE_COLS(2);

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
void findSameValueVertical(const cv::Mat& src, std::vector<Range>& ranges)
{
	//積分画像が欲しい
	CV_Assert(src.type() == CV_32SC1);

	//値が入っているかもしれないので空にする
	ranges.clear();


	Range range;

	const int endPos = src.cols - 1;
	int src0 = src.ptr<int>(0)[endPos];
	int src1;

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
		range.end = src.rows - 1;
		ranges.push_back(range);
	}
}

void drawRange(const cv::Mat& src, const vector<Range>& ranges, const int direction, cv::Mat& dst,
	const cv::Scalar& color){

	if (&src != &dst && dst.size() != src.size()){
		src.copyTo(dst);
	}

	if (direction == ImageScrap::RANGE_ROWS){
		for (size_t i = 0; i < ranges.size(); i++){
			const Range& r = ranges[i];
			//切り出した画像を1色で塗りつぶす
			dst.rowRange(r.start, r.end) = color;
		}
	}
	else if (direction == ImageScrap::RANGE_COLS){
		for (size_t i = 0; i < ranges.size(); i++){
			const Range& r = ranges[i];
			//切り出した画像を1色で塗りつぶす
			dst.colRange(r.start, r.end) = color;
		}
	}
}


void ImageScrap::computeRange(const int dir){

	if (dir == RANGE_ROWS|| dir == RANGE_ALL){
		findSameValueVertical(image, verticalRanges);
	}

	if (dir == RANGE_COLS|| dir == RANGE_ALL){
		findSameValueHorizontal(image, horizontalRanges);

	}
}

cv::Mat ImageScrap::getRow(const int i){

	if (computedRange != RANGE_ALL || computedRange != RANGE_ROWS){
		findSameValueVertical(image, verticalRanges);
	}

	if (verticalRanges.size() <= i){
		return cv::Mat();
	}

	Range& r = verticalRanges[i];
	return image.rowRange(r.start, r.end);
}

cv::Mat ImageScrap::getCol(const int i){
	if (computedRange != RANGE_ALL || computedRange != RANGE_COLS){
		findSameValueHorizontal(image, horizontalRanges);
	}

	if (horizontalRanges.size() <= i){
		return cv::Mat();
	}


	Range& r = horizontalRanges[i];
	return image.colRange(r.start, r.end);
}