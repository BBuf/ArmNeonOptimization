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
	unsigned char *Src = src.data;
	float *Src_float = new float[Height * Width];
	float *Dest = new float[Height * Width];

	for(int i = 0; i < Height*Width; i++){
		Src_float[i] = (float)(Src[i]);
	}

	vector <float> cache;
	cache.resize(Height * Width);

	int64 st = cvGetTickCount();
	for(int i=0; i<10; i++){
		//BoxFilterOrigin(Src_float, Dest, Width, Height, Radius);
		//BoxFilterOpenCV(Src_float, Dest, Width, Height, Radius, cache);
		//BoxFilterOpenCV2(Src_float, Dest, Width, Height, Radius, cache);
		//BoxFilterCache(Src_float, Dest, Width, Height, Radius, cache);
		//BoxFilterNeonAssembly(Src_float, Dest, Width, Height, Radius, cache);
		//BoxFilterNeonAssemblyV1(Src_float, Dest, Width, Height, Radius, cache);
		BoxFilterNeonAssemblyV2(Src_float, Dest, Width, Height, Radius, cache);
		cache.clear();
		printf("Here!\n");
	}
	double duration = (cv::getTickCount() - st) / cv::getTickFrequency() * 100;
	printf("%.5f\n", duration);
	return 0;
}