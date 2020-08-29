#include <iostream>
#include <stdio.h>
// #define CATCH_CONFIG_MAIN
// #include <catch.h>
#include <math.h>
#include <batchnorm.h>
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

float a[200]={2.0872, -0.5132,
          -0.0857,  0.6196,

          0.3193,  0.4106,
          -1.0289,  0.6152,

         -0.9827,  0.2736,
           0.2867,  0.9270};


float c[200]={
  2.0872, -0.5132,
          -0.0857,  0.6196,

          0.3193,  0.4106,
          -1.0289,  0.6152,

         -0.9827,  0.2736,
           0.2867,  0.9270
};


int main(){
    const int inw = 2;
    const int inh = 2;
    const int inch = 3;
    
    float Scales[3] = {1., 1., 1};
    float biases[3] = {0.5, 0.5, 0.5};
    float rollMean[3] = {0., 0., 0.};
    float rollVariance[3] = {0.25, 0.25, 0.25};

    float *src = new float[inw * inh * inch];
    float *dest = new float[inw * inh * inch];

    for(int i=0; i<inw * inh * inch; i++){
        src[i] = a[i];
    }
    
    int64 st = cvGetTickCount();

    //for(int i = 0; i < 10; i++){
    BatchNorm(src, inw, inh, inch, dest, Scales, rollMean, rollVariance, biases);
    //}
    
    double duration = (cv::getTickCount() - st) / cv::getTickFrequency() * 100;

    for(int i = 0; i < inh * inw * inch ; i++){
        bool flag = cmp(dest[i], c[i]);
        if(flag == false){
            printf("WA: %d\n", i);
            printf("Expected: %.4f, ConvOutput: %.4f\n", c[i], dest[i]);
        }
    }

    printf("Time: %.5f\n", duration);

    free(src);

    return 0;
}