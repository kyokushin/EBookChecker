#ifdef _DEBUG
#include "testdatapath.h"
#endif 

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char** argv)
{

#ifndef _DEBUG
	string commandArgs =
		"@input |  | processing one image or image named serial number"
		;

	cv::CommandLineParser parser(argc, argv, commandArgs);

	string src = parser.get<string>(0);
#endif
	string src = TEST_DATA_0;

	cout << "input file:" << src << endl;
	

}