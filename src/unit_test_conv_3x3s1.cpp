#include <iostream>
#include <stdio.h>
// #define CATCH_CONFIG_MAIN
#include <dclock.h>
#include <math.h>
#include <MsnhConvolution3x3s1.h>
using namespace std;

bool cmp(float x, float y){
    if(fabs(x - y) < 0.01){
        return true;
    }
    else{
        return false;
    }
}

float a[200]={-1.1268, -1.0129, -1.6802,  0.4670,  0.6153,   
           0.3901, -1.0640, -0.2936, -1.2315,  0.5493,   
          -0.0420, -0.2721,  0.1954, -0.2216,  0.6879,   
          -0.2050, -1.0803, -0.3176,  0.5296,  0.3935,   
           2.0697,  0.5919,  0.6199, -0.0346, -0.7999,  

          0.2293, -1.5282,  1.0790, -0.2825,  1.0728,   
          -0.4607, -0.7011, -0.2361,  0.2906,  0.1105,   
          -0.9928, -0.5154,  0.7045, -0.9297,  0.7138,   
          -0.3283,  0.2736, -0.8493, -0.7276,  2.4235,   
          -0.6976,  1.2200, -0.4467,  1.7764, -0.0902,  

         -0.5115,  0.7125,  2.6900,  1.6307, -1.0520,   
           0.8062, -1.5037, -0.2208, -0.3704, -0.0657,   
          -0.7742, -0.5371,  0.1098, -1.2455, -1.6273,   
          -0.7954, -0.0381, -0.9875,  0.6384,  0.0350,   
           0.5414,  0.6222, -0.6658,  0.3772, -0.8883};

float b[200]={-0.0073,  0.1526, -0.0270,
          -0.0072,  0.0659,  0.1265,
           0.1657, -0.0500,  0.0438,

         -0.0798,  0.0382, -0.0959,
          -0.0398, -0.0483, -0.1100,
           0.0023,  0.0489,  0.0612,

         -0.1398, -0.1093,  0.1475,
           0.1383,  0.0858, -0.0167,
          -0.0942, -0.0131, -0.1723,


         0.0643,  0.1751, -0.0905,
          -0.1898, -0.0066,  0.0137,
          -0.0252,  0.0558,  0.1431,

         -0.1606, -0.0474, -0.0856,
          -0.0632, -0.0011, -0.0256,
          -0.0368,  0.0699,  0.0243,

         -0.1506, -0.0868,  0.1396,
           0.0641, -0.1450,  0.0830,
          -0.0781, -0.1140, -0.1721,


        -0.0237, -0.0605, -0.0206,
           0.0383, -0.0095,  0.0241,
          -0.0739,  0.1794, -0.1479,

          0.0660, -0.0909, -0.0488,
          -0.0889, -0.0074,  0.1274,
           0.0130, -0.1445,  0.0469,

         -0.1116,  0.0339, -0.0671,
          -0.1096, -0.1420, -0.1370,
           0.0070, -0.1404, -0.1416,


        -0.0915, -0.1106, -0.0094,
           0.1703, -0.1753, -0.0935,
          -0.1359,  0.0788, -0.0177,

         -0.1294, -0.0438, -0.0608,
          -0.1557, -0.1188,  0.1214,
          -0.0438, -0.1629, -0.0785,

         -0.0058,  0.1322,  0.1835,
          -0.0944, -0.0943, -0.0746,
          -0.0756, -0.1007, -0.0068};

float c[200]={
   0.1554, -0.4298, -0.5700,
          -0.0541, -0.0924, -0.0473,
           0.4419,  0.1781, -0.1656,

          0.5869,  0.1084, -0.4887,
           0.2125,  0.2375, -0.1172,
           0.3615,  0.4178, -0.2846,

          0.3886,  0.2065,  0.4286,
           0.2377,  0.4176,  0.6548,
          -0.1712,  0.2952,  0.4079,

          1.4470,  1.5160,  0.2381,
           0.4402,  0.4872,  0.4865,
          -0.1318, -0.4056, -0.5152
};

static double get_time(struct timespec *start,
                       struct timespec *end) {
    return end->tv_sec - start->tv_sec + (end->tv_nsec - start->tv_nsec) * 1e-9;
}

int main(){
    struct timespec start, end;

    double time_used = 0.0;

    const int inw = 5;
    const int inh = 5;
    const int inch = 3;
    const int kw = 3;
    const int kh = 3;
    int stride = 1;
    const int outw = (inw - kw) / stride + 1;
    const int outh = (inh - kh) / stride + 1;
    const int outch = 4;

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

    for(int i = 0; i < 10; i++){
        //memset(dest, 0, sizeof(dest));
        for(int j = 0; j < outw * outh * outch; j++) dest[j] = 0.f;
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        conv3x3s1Neon(src, inw, inh, inch, kernel, dest, outw, outh, outch);
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        time_used = get_time(&start, &end);
        printf("%.5f\n", time_used);
    }


    for(int i = 0; i < outw * outh * outch ; i++){
        bool flag = cmp(dest[i], c[i]);
        if(flag == false){
            printf("WA: %d\n", i);
            printf("Expected: %.4f, ConvOutput: %.4f\n", c[i], dest[i]);
        }
    }


    for(int i = 0; i < outw * outh * outch; i++){
        printf("%.4f ", dest[i]);
    }

    // printf("\n");
    free(src);
    free(kernel);
    free(dest);

    return 0;
}