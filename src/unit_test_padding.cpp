#include <iostream>
#include <stdio.h>
// #define CATCH_CONFIG_MAIN
// #include <catch.h>
#include <math.h>
#include <convolution_3x3s1.h>
#include <convolution_3x3s2.h>
#include <opencv2/opencv.hpp>
#include <activation.h>
#include <padding.h>
#include <convolution_sgemm.h>
using namespace std;
using namespace cv;




float a[200]= {-0.3670,  0.6706, -0.4710, -0.4232,  0.0396,
           1.1935,  1.2700, -0.4183, -0.2690, -1.1960,
          -0.3244,  0.3431, -0.0428, -0.7895, -1.1025,
          -0.3374, -2.0180, -2.8447,  1.1366,  0.9439,
          -1.7559, -0.2478,  0.2211,  1.4843, -0.4977,

          0.7037,  0.1641,  0.5758,  0.4975, -1.5003,
          -0.7084, -0.6419, -0.2333, -2.3270, -0.9529,
          -0.0327,  0.1763, -0.4550, -1.9170,  1.3147,
           0.9061,  1.1292,  0.3392, -0.8160, -0.1567,
           0.6644,  0.1139,  0.1924, -0.6630,  0.7814,

          0.3477, -0.2124, -0.0696, -0.6485,  1.0422,
          -1.5059,  0.6144, -0.9405, -0.3811,  0.4394,
           0.2629, -1.4742, -0.5631,  1.1443, -0.1476,
           1.3373, -1.6746,  0.8771, -2.1727, -6.2477,
          -0.4628, -0.1945, -0.1130, -0.1061,  6.6973};


int main(){
    const int inw = 5;
    const int inh = 5;
    const int inch = 3;
    const int outw = 8;
    const int outh = 8;
    const int outch = inch;


    //5x5x3
    float *src = new float[inw * inh * inch];

    float *dest = new float[outw * outh * outch];

    //赋值
    for(int i = 0; i < inw * inw * inch; i++){
        src[i] = a[i];
    }


    int64 st = cvGetTickCount();

    
    padding(src, inw, inh, inch, dest, 1, 2, 1, 2, 10);

    double duration = (cv::getTickCount() - st) / cv::getTickFrequency() * 1000;

    printf("Time: %.5f\n", duration);


    for(int i = 0; i < outch; i++){
        for(int j = 0; j < outh; j++){
            for(int k = 0; k < outw; k++){
                printf("%.5f ", dest[i*outh*outw + j*outw + k]);
            }
            printf("\n");
        }
        printf("\n");
    }

    printf("\n");
    free(src);

    return 0;
}