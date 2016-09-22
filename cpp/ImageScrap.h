#ifndef ImageScrap_h
#define ImageScrap_h

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

class Range{
public:
	int start;
	int end;
	Range(int start, int end) :start(start), end(end){}
	Range():start(-1), end(-1){}
};

class ImageScrap {
public:
	ImageScrap(const cv::Mat& src, const int compute = RANGE_NONE, const unsigned int binaryThreshold = 192);
	cv::Mat getRow(const int i);
	cv::Mat getCol(const int i);
	int getRows(){
		return verticalRanges.size();
	}
	int getCols(){
		return horizontalRanges.size();
	}
	void computeRange(const int dir = RANGE_ALL);

	void show(const std::string& wname="ScrapImage");

	static const int RANGE_NONE;
	static const int RANGE_ALL;
	static const int RANGE_ROWS;
	static const int RANGE_COLS;

private:
	std::vector<Range> horizontalRanges;
	std::vector<Range> verticalRanges;
	cv::Mat image;
	int computedRange = RANGE_NONE;
	const unsigned int binaryThreshold;
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