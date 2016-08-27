#ifndef ImageScrap_h
#define ImageScrap_h

#include <opencv2/opencv.hpp>
#include <vector>

class Range{
public:
	int start;
	int end;
	int dir;
	Range(int start, int end) :start(start), end(end){}
	Range():start(-1), end(-1){}


	static const int VERTICAL;
	static const int HORIZONTAL;
};

//‰¡•ûŒü‚ğ‚µ‚ç‚×‚é
//•¶š‚Ì‚È‚¢”ÍˆÍ‚ğvector‚Å•Ô‚·
void findSameValueHorizontal(const cv::Mat& src, std::vector<Range>& ranges);

//c•ûŒü‚ğ‚µ‚ç‚×‚é
//•¶š‚Ì‚È‚¢”ÍˆÍ‚ğvector‚Å•Ô‚·
void findSameValueVertical(const cv::Mat& src, std::vector<Range>& ranges);


void drawRange(const cv::Mat& src, const std::vector<Range>& ranges, cv::Mat& dst,
	const cv::Scalar& colorVertical = cv::Scalar(0, 0, 255),
	const cv::Scalar& colorHorizontal = cv::Scalar(240, 176, 0));

#endif