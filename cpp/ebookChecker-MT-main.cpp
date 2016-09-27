#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

#include <qdir.h>
#include <qstring.h>
#include <qstringlist.h>

#include "cvutils.h"

#define TEST_DATA

using namespace std;


struct UserInstruction {
	int mouse_l_down = false;
	cv::Mat* pImage;
	cv::Mat showImage;
	cv::Scalar color;
	int thickness = 1;

	cv::Point mousePos_start;
	cv::Point mousePos_end;

	cv::Rect rect;


	string wname; 
	UserInstruction()
		:color(0, 0, 255)
	{
	}

	void imshow( const std::string wname, cv::Mat& image) {
		pImage = &image;
		resetShowImage();
		this->wname = wname;
		cv::imshow(wname, showImage);
	}

	void resetShowImage() {
		pImage->copyTo(showImage);
	}

	void imshow() {
		cv::imshow(wname, showImage);
	}

	void resetMouseState() {
		mouse_l_down = false;
	}

	cv::Rect& getRect() {
		rect.x = min(mousePos_start.x, mousePos_end.x);
		rect.y = min(mousePos_start.y, mousePos_end.y);
		rect.width = abs(mousePos_start.x - mousePos_end.x);
		rect.height = abs(mousePos_start.y - mousePos_end.y);

		return rect;
	}

	void setStartPos(int x, int y) {
		mousePos_start.x = x;
		mousePos_start.y = y;
	}

	void setEndPos(int x, int y) {
		mousePos_end.x = x;
		mousePos_end.y = y;
	}

	cv::Mat& getImage() {
		return showImage;
	}
};

void message(const std::string& str, int line, const char* file) {
	cout << str << " " << line << " in " << file << endl;
}

#define MESSAGE(str) message(str, __LINE__, __FILE__)


void mouseCallback(int event, int x, int y, int flag, void* data) {

	UserInstruction &udata = *((UserInstruction*)data);

	if (udata.mouse_l_down && event == CV_EVENT_MOUSEMOVE) {
		MESSAGE("move");
		udata.setEndPos(x, y);
		udata.resetShowImage();
		cv::rectangle(udata.getImage(), udata.getRect(), udata.color, udata.thickness);
		udata.imshow();
	}
	else if (event == CV_EVENT_LBUTTONDOWN) {
		MESSAGE("down");
		udata.mouse_l_down = true;
		udata.setStartPos(x, y);
		udata.setEndPos(x + 1, y + 1);
		udata.resetShowImage();
		cv::rectangle(udata.getImage(), udata.getRect(), udata.color, udata.thickness);
		udata.imshow();
	}
	else if (event == CV_EVENT_LBUTTONUP) {
		MESSAGE("down");
		udata.mouse_l_down = false;
		udata.setEndPos(x, y);
		udata.imshow();
	}
	else {
		MESSAGE("nothing");
	}

}

int main(int argc, char** argv) {

#ifdef TEST_DATA
	string src = "C:\\Users\\kyokushin\\Pictures\\testData_Top\\";
	//string src = "C:\\Users\\kyokushin\\Pictures\\testData_Bottom\\";

#else
	const string commandArgs =
		"{@input | | directory including ebook (jpg or png)}"
		;
	cv::CommandLineParser parser(argc, argv, commandArgs);

	string src = parser.get<string>(0);
#endif

	QDir dir(QString::fromLocal8Bit(src.c_str()));
	if (!dir.exists()) {
		cerr << "error directory not exists. entered directory is \"" << src << "\"" << endl;
		return 1;
	}
	QFileInfo savePath(QString::fromLocal8Bit((src + '/' + "pagenumbers_mt.png").c_str()));
	if (savePath.exists()) {
		cerr << "already exists pagenumber result file.:" << savePath.absoluteFilePath().toLocal8Bit().constData() << endl;
		return 2;
	}

	QFileInfoList fileList = dir.entryInfoList(QDir::Files);

	UserInstruction udata;
	const cv::Size displaySize(1920, 900);

	const string wname("eBookCkecker MT");
	cv::namedWindow(wname);
	cv::setMouseCallback(wname, mouseCallback, &udata);
	int key;
	char ckey;
	for (int i = 0; i < fileList.size(); i++) {

		while (1) {
			string fname = fileList[i].absoluteFilePath().toLocal8Bit().constData();
			cout << fname << endl;
			cv::Mat srcImage = cv::imread(fname, CV_LOAD_IMAGE_COLOR);

			if (srcImage.empty()) {
				break;
			}

			cv::Mat smallImage;
			double scale = min((double)displaySize.width / srcImage.cols, (double)displaySize.height / srcImage.rows);
			cv::resize(srcImage, smallImage, cv::Size(), scale, scale);

			udata.imshow(wname, smallImage);
			key = cv::waitKey();
			ckey = (char)key;
			if (ckey == 'n') {
				break;
			}
			if (key == 0x1b) {
				return 0;
			}
		}
	}

	return 0;
}