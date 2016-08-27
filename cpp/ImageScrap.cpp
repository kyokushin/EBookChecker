#include <opencv2/opencv.hpp>
#include <vector>

#include "ImageScrap.h"

using namespace std;

const int Range::VERTICAL(0);
const int Range::HORIZONTAL(1);

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
	range.dir = Range::HORIZONTAL;
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
	range.dir = Range::VERTICAL;

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

void drawRange(const cv::Mat& src, const vector<Range>& ranges, cv::Mat& dst,
	const cv::Scalar& colorVertical, const cv::Scalar& colorHorizontal){

	if (&src != &dst && dst.size() != src.size()){
		src.copyTo(dst);
	}

	for (size_t i = 0; i < ranges.size(); i++){
		const Range& r = ranges[i];
		if (r.dir == Range::VERTICAL){
			//文字のない範囲を3チャンネルの原画像から切り出す
			cv::Rect rect(0, r.start, dst.cols, r.end - r.start);
			cv::Mat roi(dst, rect);
			//切り出した画像を1色で塗りつぶす
			roi = colorVertical;
		}
		else if (r.dir == Range::HORIZONTAL){
			//文字のない範囲を3チャンネルの原画像から切り出す
			cv::Rect rect(r.start, 0, r.end - r.start, dst.rows);
			cv::Mat roi(dst, rect);
			//切り出した画像を1色で塗りつぶす
			roi = colorHorizontal;
		}
	}
}