#include "iostream"
#include "src/boxFilter.h"
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

int main(){
	// input image is gray
	Mat src = cv::imread("test_img/car.jpg", 0);
	int Height = src.rows;
	int Width = src.cols;
	unsigned char *Src = src.data;
	unsigned char *Dest = new unsigned char[Height * Width];

	

}