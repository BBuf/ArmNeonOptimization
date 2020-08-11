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

void BoxFilterBetterNeonAssembly(float *Src, float *Dest, int Width, int Height, int Radius){
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
        int nn = OutWidth >> 2;
        int remain = OutWidth - (nn << 2);


        //q9->[d18, d19]
        //q10->[d20, 0]
        //neon assembly
        if(nn > 0){
            asm volatile(
                    "pld        [%3, #192]          \n" // 预读取指令, 64x3=192
                    "vld1.f32   {d18-d20}, [%3 :64] \n" // r0, :64代表内存对齐，这里写成q就是q9,q10
                    "add        %3, #16             \n" // 一个float为4个字节，r0加载了6个float即6x4=32个字节，
                                                        //r0这里移动了16个字节，即[a,b,c,d,e,f]，即处理了[a,b,c],
                                                        //[b,c,d],[c,d,e],[d,e,f]，然后r0移动到e位置
                    "vext.32    q11, q9, q10, #1    \n" // [0,d18]
                    "vext.32    q12, q9, q10, #2    \n" // [0,d20]

                    "0:                             \n"

                    "vmul.f32   q7, q9, %e14[0]     \n" // [a*k012_low,b*k012_low,c*k012_low,d*k012_low]

                    "vmul.f32   q6, q11, %e14[1]    \n" // []

                    "vmla.f32   q13, q12, %f14[0]   \n" // 

                    "pld        [%4, #192]          \n"
                    "vld1.f32   {d18-d20}, [%4]     \n" // r1
                    "add        %4, #16             \n"

                    "vmla.f32   q7, q9, %e15[0]     \n"

                    "vext.32    q11, q9, q10, #1    \n"
                    "vext.32    q12, q9, q10, #2    \n"

                    "vmla.f32   q6, q11, %e15[1]    \n"
                    "vmla.f32   q13, q12, %f15[0]   \n"

                    "vmul.f32   q8, q9, %e14[0]     \n"

                    "vmul.f32   q14, q11, %e14[1]   \n"
                    "vmla.f32   q15, q12, %f14[0]   \n"

                    "pld        [%5, #192]          \n"
                    "vld1.f32   {d18-d20}, [%5 :64] \n" // r2
                    "add        %5, #16             \n"

                    "vmla.f32   q7, q9, %e16[0]     \n"

                    "vext.32    q11, q9, q10, #1    \n"
                    "vext.32    q12, q9, q10, #2    \n"

                    "vmla.f32   q6, q11, %e16[1]    \n"
                    "vmla.f32   q13, q12, %f16[0]   \n"

                    "vmla.f32   q8, q9, %e15[0]     \n"
                    "vmla.f32   q14, q11, %e15[1]   \n"
                    "vmla.f32   q15, q12, %f15[0]   \n"

                    "pld        [%6, #192]          \n"
                    "vld1.f32   {d18-d20}, [%6]     \n" // r3
                    "add        %6, #16             \n"

                    "vmla.f32   q8, q9, %e16[0]     \n"

                    "vext.32    q11, q9, q10, #1    \n"
                    "vext.32    q12, q9, q10, #2    \n"

                    "vmla.f32   q14, q11, %e16[1]   \n"
                    "vmla.f32   q15, q12, %f16[0]   \n"

                    "vadd.f32   q7, q7, q6          \n"

                    "pld        [%3, #192]          \n"
                    "vld1.f32   {d18-d20}, [%3 :64] \n" // r0

                    "vadd.f32   q8, q8, q14         \n"
                    "vadd.f32   q7, q7, q13         \n"
                    "vadd.f32   q8, q8, q15         \n"

                    "vext.32    q11, q9, q10, #1    \n"
                    "vext.32    q12, q9, q10, #2    \n"

                    "add        %3, #16             \n"

                    "vst1.f32   {d14-d15}, [%1]!    \n"
                    "vst1.f32   {d16-d17}, [%2]!    \n"

                    "subs       %0, #1              \n"
                    "bne        0b                  \n"

                    "sub        %3, #16             \n"
                    : "=r"(nn),      // %0
                    "=r"(outptr),  // %1
                    "=r"(outptr2), // %2
                    "=r"(r0),      // %3
                    "=r"(r1),      // %4
                    "=r"(r2),      // %5
                    "=r"(r3)       // %6
                    : "0"(nn),
                    "1"(outptr),
                    "2"(outptr2),
                    "3"(r0),
                    "4"(r1),
                    "5"(r2),
                    "6"(r3),
                    "w"(k012), // %14
                    "w"(k345), // %15
                    "w"(k678), // %16
                    : "cc", "memory", "q6", "q7", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15");
            }
        }

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

            //[a,b,c,d]->[a,c]+[b,d]->[a+b,c+d]
            float32x2_t _ss = vadd_f32(vget_low_f32(sum1), vget_high_f32(sum1));
            //[e,f,g,h]->[e,g]+[f,h]->[e+f,g+h]
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
        int nn = OutWidth >> 2;
        int remain = OutWidth - (nn << 2);

        if (nn > 0){
            asm volatile(
                "pld        [%2, #192]          \n"
                "vld1.f32   {d16-d18}, [%2]     \n" // r0
                "add        %2, #16             \n" //16个字节

                "vext.32    q10, q8, q9, #1     \n"//[a+b]
                "vext.32    q11, q8, q9, #2     \n"//[c+d]

                "0:                             \n"

                "vmul.f32   q7, q8, %e10[0]     \n"

                "vand       q14, %q13, %q13     \n" // q14 = _bias0
                "vmul.f32   q13, q10, %e10[1]   \n"
                "vmla.f32   q14, q11, %f10[0]   \n"

                "pld        [%3, #192]          \n"
                "vld1.f32   {d16-d18}, [%3]     \n" // r1
                "add        %3, #16             \n" //??

                "vmla.f32   q7, q8, %e11[0]     \n"

                "vext.32    q10, q8, q9, #1     \n"
                "vext.32    q11, q8, q9, #2     \n"

                "vmla.f32   q13, q10, %e11[1]   \n"
                "vmla.f32   q14, q11, %f11[0]   \n"

                "pld        [%4, #192]          \n"
                "vld1.f32   {d16-d18}, [%4]     \n" // r2
                "add        %4, #16             \n" //??

                "vmla.f32   q7, q8, %e12[0]     \n"

                "vext.32    q10, q8, q9, #1     \n"
                "vext.32    q11, q8, q9, #2     \n"

                "vmla.f32   q13, q10, %e12[1]   \n"
                "vmla.f32   q14, q11, %f12[0]   \n"

                "pld        [%2, #192]          \n"
                "vld1.f32   {d16-d18}, [%2]     \n" // r0
                "add        %2, #16             \n" //??

                "vadd.f32   q7, q7, q13         \n"
                "vadd.f32   q7, q7, q14         \n"

                "vext.32    q10, q8, q9, #1     \n"
                "vext.32    q11, q8, q9, #2     \n"

                "vst1.f32   {d14-d15}, [%1]!    \n"

                "subs       %0, #1              \n"
                "bne        0b                  \n"

                "sub        %2, #16             \n" //??
                : "=r"(nn),     // %0
                "=r"(outptr), // %1
                "=r"(r0),     // %2
                "=r"(r1),     // %3
                "=r"(r2)      // %4
                : "0"(nn),
                "1"(outptr),
                "2"(r0),
                "3"(r1),
                "4"(r2),
                "w"(k012), // %10
                "w"(k345), // %11
                "w"(k678), // %12
                "w"(_bias0)  // %13
                : "cc", "memory", "q7", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15");
        }

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