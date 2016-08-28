#include <opencv2/opencv.hpp>
#include <vector>

#include "ImageScrap.h"

#include "cvutils.h"

using namespace std;

const int ImageScrap::RANGE_NONE(0);
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

ImageScrap::ImageScrap(const cv::Mat& src, const int compute)
	:image(src){
	CV_Assert(image.channels() == 1 && image.type() == CV_8UC1);

	computeRange(compute);
}


void ImageScrap::computeRange(const int dir){
	if (dir != RANGE_ALL
		&& dir != RANGE_ROWS
		&& dir != RANGE_COLS){
		return;
	}

	cv::Mat binary;
	int binaryMax = 1;//二値化時の最大値は1に。積分するときに白だったところか黒だったところかがわかればいい。
	int binaryThreshold = 128;
	cv::threshold(image, binary, binaryThreshold, binaryMax, cv::THRESH_BINARY_INV);
	CV_Assert(binary.channels() == 1 && binary.type() == CV_8UC1);

	//積分画像の生成
	cv::Mat integral;
	cv::integral(binary, integral);
	integral = cv::Mat(integral, cv::Rect(1, 1, integral.cols-1, integral.rows-1));//積分画像は元画像より1ピクセル大きいので
	cout << "image:" << image.size() << endl
		<< "integral:" << integral.size() << endl;
	CV_Assert(integral.channels() == 1 && integral.type() == CV_32SC1);

	if (dir == RANGE_ROWS || dir == RANGE_ALL){
		vector<Range> tmpRanges;
		findSameValueVertical(integral, tmpRanges);
		verticalRanges.clear();
		if (tmpRanges.size() == 1 && 
			(tmpRanges[0].start == 0 && tmpRanges[0].end == image.rows - 1)) {
			//do nothing
		}
		else if (0 < tmpRanges.size()){
			Range r;
			if (tmpRanges[0].start != 0){
				r.start = 0;
				r.end = tmpRanges[0].start;
				verticalRanges.push_back(r);
			}
			r.start = tmpRanges[0].end;
			int beforeIndex = 0;
			for (int i = 1; i < tmpRanges.size() - 1; i++){
				r.end = tmpRanges[i].start;
				verticalRanges.push_back(r);
				r.start = tmpRanges[i].end;
				beforeIndex = i;
			}
			if (beforeIndex < tmpRanges.size() - 1) {
				Range& lr = tmpRanges[tmpRanges.size() - 1];
				r.end = lr.start;
				verticalRanges.push_back(r);
				if (lr.end != image.rows - 1) {
					r.start = lr.end;
					r.end = image.rows - 1;
					verticalRanges.push_back(r);
				}
			}
		}
		else if (tmpRanges.size() == 0) {
			Range r;
			r.start = 0;
			r.end = image.rows - 1;
			verticalRanges.push_back(r);
		}
	}

	if (dir == RANGE_COLS || dir == RANGE_ALL){
		vector<Range> tmpRanges;
		findSameValueHorizontal(integral, tmpRanges);
		horizontalRanges.clear();

		if (tmpRanges.size() == 1
			&& (tmpRanges[0].start == 0 && tmpRanges[0].end == image.cols - 1)
			) {
			//do nothing
		}
		else if (0 < tmpRanges.size()) {
			Range r;
			if (tmpRanges[0].start != 0){
				r.start = 0;
				r.end = tmpRanges[0].start;
				horizontalRanges.push_back(r);
			}
			r.start = tmpRanges[0].end;
			int beforeIndex = 0;
			for (int i = 1; i < tmpRanges.size() - 1; i++){
				r.end = tmpRanges[i].start;
				horizontalRanges.push_back(r);
				r.start = tmpRanges[i].end;
				beforeIndex = i;
			}
			if (beforeIndex < tmpRanges.size() - 1) {
				Range& lr = tmpRanges[tmpRanges.size() - 1];
				r.end = lr.start;
				horizontalRanges.push_back(r);
				if (lr.end != image.cols - 1) {
					r.start = lr.end;
					r.end = image.cols - 1;
					horizontalRanges.push_back(r);
				}
			}
		}
		else if (tmpRanges.size() == 0) {
			Range r;
			r.start = 0;
			r.end = image.cols - 1;
			verticalRanges.push_back(r);
		}
	}

	if (dir == RANGE_ALL
		|| (dir == RANGE_ROWS && computedRange == RANGE_COLS)
		|| (dir == RANGE_COLS && computedRange == RANGE_ROWS)
		){
		computedRange = RANGE_ALL;
	}
	else {
		computedRange = dir;
	}

}

cv::Mat ImageScrap::getRow(const int i){

	if (computedRange != RANGE_ALL || computedRange != RANGE_ROWS){
		computeRange(RANGE_ROWS);
	}

	if (verticalRanges.size() <= i){
		return cv::Mat();
	}

	Range& r = verticalRanges[i];
	return image.rowRange(r.start, r.end);
}

cv::Mat ImageScrap::getCol(const int i){
	if (computedRange != RANGE_ALL || computedRange != RANGE_COLS){
		computeRange(RANGE_COLS);
	}

	if (horizontalRanges.size() <= i){
		return cv::Mat();
	}


	Range& r = horizontalRanges[i];
	return image.colRange(r.start, r.end);
}

void ImageScrap::show(const string& wname){

	cv::Mat rangeImage;
	cv::Mat imgArray[] = { image, image, image };

	cv::merge(imgArray, 3, rangeImage);


	for (int i = 0; i < horizontalRanges.size(); i++){
		Range& r = horizontalRanges[i];
		rangeImage.colRange(r.start, r.end) = cv::Scalar(240, 176, 0);
	}

	for (int i = 0; i < verticalRanges.size(); i++){
		Range& r = verticalRanges[i];
		rangeImage.rowRange(r.start, r.end) = cv::Scalar(0, 0, 255);
	}

	int key = -1;
	cv::Mat showImage;
	float scale = 1.0;
	float scaleUnit = 0.1;
	rangeImage.copyTo(showImage);
	while (key != 0x1b){
		cv::imshow(wname, showImage);
		key = cv::waitKey();

		if ('-' == key){
			scale -= scaleUnit;
			if (scale < 0.1){
				scale = 0.1;
			}
			cv::resize(rangeImage, showImage, cv::Size(), scale, scale);
		}
		else if ('+' == key){
			scale += scaleUnit;
			if (scale > 10.0){
				scale = 10.0;
			}
			cv::resize(rangeImage, showImage, cv::Size(), scale, scale);
		}
	}

	cv::destroyWindow(wname);
}