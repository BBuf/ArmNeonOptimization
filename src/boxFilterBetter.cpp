#include "boxFilterBetter.h"

void BoxFilterBetterOrigin(float *Src, float *Dest, int Width, int Height, int Radius){
    //only 3x3
    int OutWidth = Width - Radius + 1;
    int OutHeight = Height - Radius + 1;
    float *kernel = new float[Radius*Radius];
    for(int i = 0; i < Radius*Radius; i++){
        kernel[i] = 1.0;
    }
    float *k0 = kernel;
    float *k1 = kernel + 3;
    float *k2 = kernel + 6;
    float* r0 = Src;
    float* r1 = Src + Width;
    float* r2 = Src + Width * 2;
    float* r3 = Src + Width * 3;
    float* outptr = Dest;
    float* outptr2 = Dest + OutWidth;
    int i = 0;
    for (; i + 1 < OutHeight; i += 2){
        int remain = OutWidth;
        for(; remain > 0; remain--){
            float sum1 = 0, sum2 = 0;
            sum1 += r0[0] * k0[0];
            sum1 += r0[1] * k0[1];
            sum1 += r0[2] * k0[2];
            sum1 += r1[0] * k1[0];
            sum1 += r1[1] * k1[1];
            sum1 += r1[2] * k1[2];
            sum1 += r2[0] * k2[0];
            sum1 += r2[1] * k2[1];
            sum1 += r2[2] * k2[2];

            sum2 += r1[0] * k0[0];
            sum2 += r1[1] * k0[1];
            sum2 += r1[2] * k0[2];
            sum2 += r2[0] * k1[0];
            sum2 += r2[1] * k1[1];
            sum2 += r2[2] * k1[2];
            sum2 += r3[0] * k2[0];
            sum2 += r3[1] * k2[1];
            sum2 += r3[2] * k2[2];
            *outptr = sum1;
            *outptr2 = sum2;
            r0++;
            r1++;
            r2++;
            r3++;
            outptr++;
            outptr2++;
        }

        r0 += 2 + w;
        r1 += 2 + w;
        r2 += 2 + w;
        r3 += 2 + w;

        outptr += outw;
        outptr2 += outw;
    }

    for(; i < OutHeight; i++){
        int remain = OutWidth;
        for(; remain > 0; remain--){
            float sum1 = 0;
            sum1 += r0[0] * k0[0];
            sum1 += r0[1] * k0[1];
            sum1 += r0[2] * k0[2];
            sum1 += r1[0] * k1[0];
            sum1 += r1[1] * k1[1];
            sum1 += r1[2] * k1[2];
            sum1 += r2[0] * k2[0];
            sum1 += r2[1] * k2[1];
            sum1 += r2[2] * k2[2];
            *outptr = sum;
            r0++;
            r1++;
            r2++;
            outptr++;
        }

        r0 += 2;
        r1 += 2;
        r2 += 2;
    }

}

void BoxFilterBetterNeonIntrinsics(float *Src, float *Dest, int Width, int Height, int Radius){
    int OutWidth = Width - Radius + 1;
    int OutHeight = Height - Radius + 1;
    float *kernel = new float[Radius*Radius];
    for(int i = 0; i < Radius*Radius; i++){
        kernel[i] = 1.0;
    }
    
    float32x4_t k012 = vld1q_f32(kernel);
    float32x4_t k345 = vld1q_f32(kernel + 3);
    float32x4_t k678 = vld1q_f32(kernel + 6);

    k012 = vsetq_lane_f32(0.f, k012, 3);
    k345 = vsetq_lane_f32(0.f, k345, 3);
    k678 = vsetq_lane_f32(0.f, k678, 3);

    float* r0 = Src;
    float* r1 = Src + Width;
    float* r2 = Src + Width * 2;
    float* r3 = Src + Width * 3;
    float* outptr = Dest;
    float* outptr2 = Dest + OutWidth;
    int i = 0;
    for (; i + 1 < OutHeight; i += 2){
        int remain = OutWidth;
        for(; remain > 0; i--){
            float32x4_t r00 = vld1q_f32(r0);
            float32x4_t r10 = vld1q_f32(r1);
            float32x4_t r20 = vld1q_f32(r2);
            float32x4_t r30 = vld1q_f32(r3);
            //sum1
            float32x4_t sum1 = vmulq_f32(r00, k012);
            sum1 = vmlaq_f32(sum1, r10, k345);
            sum1 = vmlaq_f32(sum1, r20, k678);

            //sum2
            float32x4_t sum2 = vmulq_f32(r10, k012);
            sum2 = vmlaq_f32(sum2, r20, k345);
            sum2 = vmlaq_f32(sum2, r30, k678);

            //[a,b,c,d]->[a+b,c+d]
            float32x2_t _ss = vadd_f32(vget_low_f32(sum1), vget_high_f32(sum1));
            //[e,f,g,h]->[e+f,g+h]
            float32x2_t _ss2 = vadd_f32(vget_low_f32(sum2), vget_high_f32(sum2));
            //[a+b+c+d,e+f+g+h]
            float32x2_t _sss2 = vpadd_f32(_ss, _ss2);

            *outptr = vget_lane_f32(_sss2, 0);
            *outptr2 = vget_lane_f32(_sss2, 1);
            
            r0++;
            r1++;
            r2++;
            r3++;
            outptr++;
            outptr2++;
        }
        
        r0 += 2 + w;
        r1 += 2 + w;
        r2 += 2 + w;
        r3 += 2 + w;

        outptr += outw;
        outptr2 += outw;
    }

    for(; i < OutHeight; i++){
        int remain = OutWidth;
        for(; remain > 0; remain--){
            float32x4_t r00 = vld1q_f32(r0);
            float32x4_t r10 = vld1q_f32(r1);
            float32x4_t r20 = vld1q_f32(r2);

            //sum1
            float32x4_t sum1 = vmulq_f32(r00, k012);
            sum1 = vmlaq_f32(sum1, r10, k345);
            sum1 = vmlaq_f32(sum1, r20, k678);

            float32x2_t _ss = vadd_f32(vget_low_f32(sum1), vget_high_f32(sum1));
            _ss = vpadd_f32(_ss, _ss);

            *outptr = vget_lane_f32(_ss, 0);

            r0++;
            r1++;
            r2++;
            outptr++;
        }

        r0 += 2;
        r1 += 2;
        r2 += 2;
    }
}