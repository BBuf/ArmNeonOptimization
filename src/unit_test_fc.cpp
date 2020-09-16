#include <iostream>
#include <stdio.h>
// #define CATCH_CONFIG_MAIN
// #include <catch.h>
#include "opencv2/opencv.hpp"
#include <math.h>
#include <innerproduct.h>
using namespace std;
using namespace cv;




float a[200]= {0.2257,  0.0121, -1.3306,  0.1647,  1.0636};

float b[200] = { 0.3227,  0.3356,  0.0804,  0.4121,  0.2817,
        -0.0523, -0.4441,  0.1091, -0.2768,  0.2344,
        -0.3236, -0.3373, -0.3080,  0.3878, -0.2579,
         0.0323, -0.0680,  0.2735, -0.2507,  0.3750,
        -0.0678,  0.0591, -0.1246,  0.0840, -0.1515,
        -0.2360, -0.2440,  0.1295,  0.2099,  0.3450,
        -0.2095,  0.1793,  0.3417, -0.0057, -0.3862,
        -0.4196, -0.2469,  0.0508, -0.1741, -0.2394,
        -0.1452,  0.0960,  0.1259,  0.3661,  0.3111,
         0.1407,  0.2389, -0.0888,  0.3204, -0.3112};

float c[200] = {0.33740 ,0.04137 ,0.12228 ,0.00011 ,0.00390 ,0.17298 ,-0.91148 ,-0.44859 ,0.19205, -0.12542};

int main(){
    const int inch = 5;
    const int outch = 10;

    //5x5x3
    float *src = new float[inch];

    float *weight = new float[inch * outch];

    float *dest = new float[outch];

    //赋值
    for(int i = 0; i < inch; i++){
        src[i] = a[i];
    }

    for(int i = 0; i < inch * outch; i++){
        weight[i] = b[i];
    }

    int64 st = cvGetTickCount();

    InnerProduct(src, inch, weight, dest, outch);
    

    double duration = (cv::getTickCount() - st) / cv::getTickFrequency() * 1000;

    printf("Time: %.5f\n", duration);


    for(int i = 0; i < outch; i++){
        printf("%.5f ", dest[i]);
    }
    printf("\n");
    free(src);
    free(weight);

    return 0;
}