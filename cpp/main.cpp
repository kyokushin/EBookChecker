#ifdef _DEBUG
#include "testdatapath.h"
#endif 

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace std;

//�摜�\���p�֐�
int showImage(const cv::Mat& image, int wait = 0){
	static const string wname("EBookChecker");

	cv::imshow(wname, image);
	return cv::waitKey(wait);
}

int main(int argc, char** argv)
{


#ifndef _DEBUG
	//�R�}���h���C�������̉�́B�f�o�b�O�̎��͎g��Ȃ�
	string commandArgs =
		"@input |  | processing one image or image named serial number"
		;

	cv::CommandLineParser parser(argc, argv, commandArgs);

	string src = parser.get<string>(0);
#else
	string src = TEST_DATA_0;
#endif

	cout << "input file:" << src << endl;
	

	//�摜�̓ǂݍ��݁B�O���[�X�P�[���摜�Ƃ��ēǂݍ���
	cv::Mat image = cv::imread(src, CV_LOAD_IMAGE_GRAYSCALE);
	CV_Assert(image.channels() == 1 && image.type() == CV_8UC1);
	showImage(image);

	//��l��
	cv::Mat binary;
	int binaryMax = 1;//��l�����̍ő�l��1�ɁB�ϕ�����Ƃ��ɔ��������Ƃ��납���������Ƃ��납���킩��΂����B
	int binaryThreshold = 128;
	cv::threshold(image, binary, binaryThreshold, binaryMax, cv::THRESH_BINARY_INV);
	CV_Assert(binary.channels() == 1 && image.type() == CV_8UC1);
	showImage(binary);
	cv::imwrite("binary.jpg", binary);

	//�ϕ��摜�̐���
	cv::Mat integral;
	cv::integral(binary, integral);
	CV_Assert(integral.channels() == 1 && integral.type() == CV_32SC1);
	showImage(integral);
	cv::imwrite("integral.jpg", integral);

	//�ϕ��摜�����₷�����鏈��
	cv::Mat integralVisible;
	double max;
	double min;
	cv::minMaxLoc(integral, &min, &max);//�ő�l�����ق����̂ő�3�����܂ŁB�ŏ��l��0�̂͂������{����0���m�F���邽�߂Ɏg��
	CV_Assert(min == 0.0);//�{���ɍŏ��l��0�ɂȂ��Ă��邩�m�F

	integral.convertTo(integralVisible, CV_8UC1, 255.0 / max); //beta�͎g��Ȃ��B0-255�̒l���Ƃ�悤��alpha��^����B
	showImage(integralVisible);
	cv::imwrite("integralVisible.jpg", integralVisible);

}