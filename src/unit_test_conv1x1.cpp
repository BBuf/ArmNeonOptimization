#include <iostream>
#include <stdio.h>
// #define CATCH_CONFIG_MAIN
// #include <catch.h>
#include <math.h>
#include <convolution1x1s1.h>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

bool cmp(float x, float y){
    if(fabs(x - y) < 0.01){
        return true;
    }
    else{
        return false;
    }
}

float a[200]={0.4244, 0.3091, 0.7797,
          0.7496, 0.7195, 0.6889,
          0.3440, 0.2429, 0.5033,

         0.3550, 0.3360, 0.5398,
          0.3238, 0.7674, 0.7014,
          0.9794, 0.8237, 0.3186,

         0.4728, 0.3721, 0.6978,
          0.4609, 0.1075, 0.4288,
          0.1426, 0.9303, 0.2984,

         0.7029, 0.4923, 0.0368,
          0.2966, 0.4804, 0.3115,
          0.1282, 0.6885, 0.5157,

         0.1035, 0.3871, 0.2454,
          0.3217, 0.0062, 0.8199,
          0.2483, 0.0706, 0.6427};

float b[200]={-0.1499,

          0.0072,

         -0.1595,

          0.1800,

          0.3299,


        -0.0621,

          0.3646,

          0.3682,

         -0.3047,

         -0.1166,


         0.3721,

         -0.2708,

          0.3296,

          0.3266,

         -0.2199,


         0.2098,

         -0.0757,

          0.0936,

         -0.3217,

         -0.3813,


         0.4034,

         -0.0514,

         -0.2285,

         -0.3653,

         -0.2004,


         0.3478,

         -0.2777,

         -0.3766,

         -0.4414,

          0.1110,


        -0.1633,

         -0.2303,

         -0.4248,

          0.0939,

          0.4397};

float c[200]={
   0.0242,  0.1131, -0.1367,
          -0.0240, -0.0309,  0.1600,
           0.0378, -0.0316,  0.1841,

          0.0509,  0.0452,  0.3655,
           0.1133,  0.1276,  0.1803,
           0.3202,  0.4097, -0.0373,

          0.4245,  0.2223,  0.3320,
           0.3693,  0.2509,  0.1292,
          -0.1029,  0.3834,  0.2264,

         -0.1592, -0.2317,  0.0826,
          -0.0422, -0.0540, -0.2813,
          -0.1246, -0.1727, -0.3016,

         -0.2326, -0.2350,  0.0647,
           0.0076,  0.0495, -0.1343,
          -0.0408, -0.4226, -0.1988,

         -0.4278, -0.3003, -0.1305,
          -0.0980, -0.2148, -0.1632,
          -0.2351, -0.7907, -0.1822,

         -0.2403, -0.0694, -0.4367,
          -0.2234, -0.2920, -0.0663,
          -0.2210, -0.5287,  0.0488
};


int main(){
    const int inw = 3;
    const int inh = 3;
    const int inch = 5;
    const int kw = 1;
    const int kh = 1;
    int stride = 1;
    const int outw = (inw - kw) / stride + 1;
    const int outh = (inh - kh) / stride + 1;
    const int outch = 7;

    //5x5x3
    float *src = new float[inw * inh * inch];
    //3x3x4
    float *kernel = new float[kw * kh * outch * inch];
    //3x3x4
    float *dest = new float[outw * outh * outch];

    //赋值
    for(int i = 0; i < inw * inh * inch; i++){
        src[i] = a[i];
    }

    for(int i = 0; i < kw * kh * inch * outch; i++){
        kernel[i] = b[i];
    }
    
    int64 st = cvGetTickCount();

    for(int i = 0; i < 10; i++){
        //memset(dest, 0, sizeof(dest));
        for(int j = 0; j < outw * outh * outch; j++) dest[j] = 0.f;
        conv1x1s1(src, inw, inh, inch, kernel, dest, outw, outh, outch);
    }
    
    double duration = (cv::getTickCount() - st) / cv::getTickFrequency() * 100;

    for(int i = 0; i < outw * outh * outch ; i++){
        bool flag = cmp(dest[i], c[i]);
        if(flag == false){
            printf("WA: %d\n", i);
            printf("Expected: %.4f, ConvOutput: %.4f\n", c[i], dest[i]);
        }
    }

    printf("Time: %.5f\n", duration);

    // for(int i = 0; i < outw * outh * outch; i++){
    //     printf("%.4f ", dest[i]);
    // }

    // printf("\n");
    free(src);
    free(kernel);
    free(dest);

    return 0;
}