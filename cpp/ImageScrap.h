#ifndef ImageScrap_h
#define ImageScrap_h

#include <opencv2/opencv.hpp>
#include <vector>

class Range{
public:
	int start;
	int end;
	Range(int start, int end) :start(start), end(end){}
	Range():start(-1), end(-1){}
};

class ImageScrap {
public:
	ImageScrap(const cv::Mat& src);
	cv::Mat getRow(int i);
	cv::Mat getCol(int i);
	void computeRange(const int dir = RANGE_ALL);

	static const int RANGE_ALL;
	static const int RANGE_VERTICAL;
	static const int RANGE_HORIZONTAL;

private:
	std::vector<Range> horizontalRange;
	std::vector<Range> verticalRange;
};

//横方向をしらべる
//文字のない範囲をvectorで返す
void findSameValueHorizontal(const cv::Mat& src, std::vector<Range>& ranges);

//縦方向をしらべる
//文字のない範囲をvectorで返す
void findSameValueVertical(const cv::Mat& src, std::vector<Range>& ranges);


void drawRange(const cv::Mat& src, const std::vector<Range>& ranges, const int direction, cv::Mat& dst,
	const cv::Scalar& color);

#endif