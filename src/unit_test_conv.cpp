#include <iostream>
#include <stdio.h>
// #define CATCH_CONFIG_MAIN
// #include <catch.h>
#include <math.h>
#include <convolution_3x3s1.h>
#include <convolution_3x3s2.h>
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

float a[200]={0.9693, -0.3127,  0.5993,  1.1899,  1.1922,
          -0.7460, -0.3461, -2.4447, -0.8302,  0.1879,
           0.2836, -1.1807,  2.2776,  0.5846,  0.7483,
           0.1169,  1.3915,  0.4392, -0.7050, -0.1771,
           0.0775, -0.7553, -1.4840, -0.4110, -0.8680,

         -1.4014, -0.5142,  0.8176, -1.4706,  0.4082,
           0.1941,  0.5164,  0.8029,  2.2918, -0.3986,
          -0.1148, -1.3970, -0.3677,  0.2416, -0.1691,
           1.5199, -0.1172, -0.4487,  0.9206, -0.1974,
           0.4170, -0.9105, -0.4584, -0.2534, -2.4894,

         -0.2689,  0.3504,  2.3823, -2.2410, -0.7617,
           0.1518,  1.1613, -0.2931,  2.0837, -0.3363,
           0.2677,  0.2750,  0.1113,  1.8593,  0.2516,
           2.5699, -1.0455,  0.3231, -0.5418,  0.2948,
          -0.1190,  2.5510,  0.8176, -1.2129, -1.8236};

float b[200]={0.0474, -0.1511,  0.1587,
          -0.0604, -0.0858, -0.0527,
          -0.1009,  0.1101,  0.1481,

         -0.0866,  0.1222,  0.0691,
          -0.1683,  0.1096,  0.1746,
           0.0184, -0.1525,  0.0164,

         -0.0855, -0.1915, -0.0064,
          -0.1891, -0.1165,  0.1326,
           0.1314, -0.0340, -0.1741,


         0.1603,  0.1911,  0.0509,
          -0.1473,  0.0431, -0.1617,
           0.1581,  0.1039, -0.1767,

          0.0755,  0.1817, -0.0037,
           0.0216, -0.1792,  0.0973,
          -0.0882,  0.0325,  0.0996,

         -0.1751, -0.0597, -0.1521,
          -0.0717, -0.0829,  0.0938,
           0.0313,  0.1094,  0.0533,


        -0.0457,  0.1650,  0.0640,
          -0.0555, -0.0156, -0.0019,
          -0.0393, -0.0785, -0.0075,

          0.0234,  0.0423,  0.1106,
          -0.0599, -0.1688,  0.0928,
           0.0436, -0.0126,  0.0677,

          0.1096, -0.0538,  0.1900,
          -0.0865, -0.1427,  0.1775,
          -0.1731, -0.1793, -0.1459,


        -0.0618,  0.1741,  0.0230,
           0.0773, -0.1617, -0.1663,
           0.1088,  0.0732, -0.0366,

          0.0353, -0.1806, -0.0126,
           0.1597, -0.0060, -0.0351,
           0.0748, -0.1413,  0.0600,

         -0.0927, -0.1791,  0.0127,
          -0.0606,  0.1636, -0.1602,
           0.1292,  0.0369,  0.0106};

float c[200]={
   0.7989, -0.1245,
          -0.9434,  0.4788,

         -0.5745,  0.2203,
          -0.0880, -0.3918,

          0.1192, -0.7320,
          -0.7763,  0.1785,

          0.6309,  1.3897,
          -0.1714, -0.7308
};


int main(){
    const int inw = 15;
    const int inh = 15;
    const int inch = 512;
    const int kw = 3;
    const int kh = 3;
    int stride = 1;
    const int outw = (inw - kw) / stride + 1;
    const int outh = (inh - kh) / stride + 1;
    const int outch = 1024;

    //5x5x3
    float *src = new float[inw * inh * inch];
    //3x3x4
    float *kernel = new float[kw * kh * outch * inch];
    //3x3x4
    float *dest = new float[outw * outh * outch];

    //赋值
    for(int i = 0; i < inw * inh * inch; i++){
        src[i] = 0.1;
    }

    for(int i = 0; i < kw * kh * inch * outch; i++){
        kernel[i] = 0.2;
    }
    
    int64 st = cvGetTickCount();

    for(int i = 0; i < 10; i++){
        //memset(dest, 0, sizeof(dest));
        for(int j = 0; j < outw * outh * outch; j++) dest[j] = 0.f;
        conv3x3s1_neon(src, inw, inh, inch, kernel, dest, outw, outh, outch);
    }
    
    double duration = (cv::getTickCount() - st) / cv::getTickFrequency() * 100;

    // for(int i = 0; i < outw * outh * outch ; i++){
    //     bool flag = cmp(dest[i], c[i]);
    //     if(flag == false){
    //         printf("WA: %d\n", i);
    //         printf("Expected: %.4f, ConvOutput: %.4f\n", c[i], dest[i]);
    //     }
    // }

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