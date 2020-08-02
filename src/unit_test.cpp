#include "iostream"
#include "boxFilter.h"
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

int main(){
	// input image is gray
	Mat src = cv::imread("test_img/car.jpg", 0);
	int Height = src.rows;
	int Width = src.cols;
	int Radius = 3;
	float *Src = (float *)src.data;
	float *Dest = new float[Height * Width];

	vector <float> cache;
	cache.resize(Height * Width);

	int64 st = cvGetTickCount();
	for(int i=0; i<10; i++){
		BoxFilterOrigin(Src, Dest, Width, Height, Radius);
		//BoxFilterNeonIntrinsics(Src, Dest, Width, Height, Radius, cache);
		cache.clear();
		printf("Here!\n");
	}
	double duration = (cv::getTickCount() - st) / cv::getTickFrequency() * 100;
	printf("%.5f\n", duration);
	return 0;
}