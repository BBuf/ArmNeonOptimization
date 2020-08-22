//src conv kernel
#include <vector>
#include <iostream>
#define USE_NEON 0
#include <arm_neon.h>
#define USE_OMP 0
#define OMP_THREAD 2
using namespace std;

void conv3x3s2_neon(float *const &src, const int &inWidth, const int &inHeight,  const int &inChannel, float *const &kernel, 
                                        float* &dest, const int &outWidth, const int &outHeight, const int &outChannel){
        int ccOutChannel = outChannel >> 1;
        int ccRemainOutChannel = outChannel << 1;

        const int in_size = inWidth * inHeight;
        const int out_size = outWidth * outHeight;
        //deal two conv output 
    #if USE_OMP
        #pragma omp parallel for num_threads(OMP_THREAD)
    #endif 
        for(int cc = 0; cc < ccOutChannel; cc++){
            int c = cc << 1;
            //get two conv output in same time
            float *dest0 = dest + c * out_size;
            float *dest1 =  dest + (c + 1) * out_size;

            for(int j = 0; j < out_size; j++) dest0[j] = 0.f;
            for(int j = 0; j < out_size; j++) dest1[j] = 0.f;

            //two output rely on two kernel
            float *k0 = kernel + c * inChannel * 3 * 3;
            float *k1 = kernel + (c + 1) * inChannel * 3 * 3;

            for(int q = 0; q < inChannel; q++){
                float* destptr0 = dest0;
                float* destptr1 = dest1;

                const float* src0 = src + q * in_size;
                //deal four lines and get two outputs in a feature map
                const float* r0 = src0;
                const float* r1 = src0 + inWidth;
                const float* r2 = src0 + inWidth * 2;



    #if USE_NEON
                float32x4_t k012 = vld1q_f32(k0);
                float32x4_t k345 = vld1q_f32(k0 + 3);
                float32x4_t k678 = vld1q_f32(k0 + 6);

                float32x4_t k012_next = vld1q_f32(k1);
                float32x4_t k345_next = vld1q_f32(k1 + 3);
                float32x4_t k678_next = vld1q_f32(k1 + 6);
    #endif

                int i = 0;
                
                //deal three lines and get one output in a feature map
                for(; i < outHeight; i++){
                    
    #if USE_NEON
                    int nn = outWidth >> 2;
                    int remain = outWidth - (nn << 2);
    #else                
                    int remain = outWidth;

    #endif

    #if USE_NEON

    #if __aarch64__
                    throw Exception(1, "Error: armv8 temporarily not supported!", __FILE__, __LINE__, __FUNCTION__);
    #else
                    if(nn > 0){
                        asm  volatile(
                            
                        );
                    }
    #endif

    #endif

                    for(; remain > 0; remain--){
    #if USE_NEON
                        float32x4_t r00 = vld1q_f32(r0);
                        float32x4_t r10 = vld1q_f32(r1);
                        float32x4_t r20 = vld1q_f32(r2);

                        //conv output1->chanel q output1 
                        float32x4_t sum0 = vmulq_f32(r00, k012);
                        //conv output1->channel q output2
                        float32x4_t sum1 = vmulq_f32(r00, k012_next);
                        sum0 = vmlaq_f32(sum0, r10, k345);
                        sum1 = vmlaq_f32(sum1, r10, k345_next);
                        sum0 = vmlaq_f32(sum0, r20, k678);
                        sum1 = vmlaq_f32(sum1, r20, k678_next);


                        // use *destptr0 's data repalce sum0[3]
                        sum0 = vsetq_lane_f32(*destptr0, sum0, 3);
                        sum1 = vsetq_lane_f32(*destptr1, sum1, 3);

    #if __aarch64__
                        *destptr0 = vaddvq_f32(sum0);
                        *destptr1 = vaddvq_f32(sum1);
    #else
                        float32x2_t _ss0 = vadd_f32(vget_low_f32(sum0), vget_high_f32(sum0));
                        float32x2_t _ss1 = vadd_f32(vget_low_f32(sum1), vget_high_f32(sum1));

                        float32x2_t _ss01 = vpadd_f32(_ss0, _ss1);

                        *destptr0 =  vget_lane_f32(_ss01, 0);
                        *destptr1 =  vget_lane_f32(_ss01, 1);     
    #endif

    #else

                        float sum0 = 0.f;
                        float sum1 = 0.f;

                        //conv output1->chanel q output1
                        sum0 += r0[0] * k0[0];
                        sum0 += r0[1] * k0[1];
                        sum0 += r0[2] * k0[2];
                        sum0 += r1[0] * k0[3];
                        sum0 += r1[1] * k0[4];
                        sum0 += r1[2] * k0[5];
                        sum0 += r2[0] * k0[6];
                        sum0 += r2[1] * k0[7];
                        sum0 += r2[2] * k0[8];

                        //conv output2->channel q output1
                        sum1 += r0[0] * k1[0];
                        sum1 += r0[1] * k1[1];
                        sum1 += r0[2] * k1[2];
                        sum1 += r1[0] * k1[3];
                        sum1 += r1[1] * k1[4];
                        sum1 += r1[2] * k1[5];
                        sum1 += r2[0] * k1[6];
                        sum1 += r2[1] * k1[7];
                        sum1 += r2[2] * k1[8];

                        //sum to dest
                        *destptr0 += sum0;
                        *destptr1 += sum1;
                        //update point address
    #endif
                        r0 += 2;
                        r1 += 2;
                        r2 += 2;
                        destptr0++;
                        destptr1++;
                    }

                    r0 += 2 * (inWidth - outWidth);
                    r1 += 2 * (inWidth - outWidth);
                    r2 += 2 * (inWidth - outWidth);
                }
                
                //mov conv kernel
                k0 += 9;
                k1 += 9;
            }
        }

        //deal one conv output
    #if USE_OMP
    #pragma omp parallel for num_threads(OMP_THREAD)
    #endif 


        for(int cc = ccRemainOutChannel; cc < outChannel; cc++){

            int c = cc;
            float *dest0 = dest + c * out_size;
            for(int j = 0; j < out_size; j++) dest0[j] = 0.f;
            const float* kernel0 = kernel + c * inChannel * 3 * 3;

            for(int q = 0; q < inChannel; q++){
                float *destptr0 = dest0;
                float *destptr1 = dest0 + outWidth;

                const float* src0 = src + q * in_size;
                //deal four lines and get two outputs in a feature map
                const float* r0 = src0;
                const float* r1 = src0 + inWidth;
                const float* r2 = src0 + inWidth * 2;
                const float* r3 = src0 + inWidth * 3;

    #if USE_NEON
                float32x4_t k012 = vld1q_f32(kernel0);
                float32x4_t k345 = vld1q_f32(kernel0 + 3);
                float32x4_t k678 = vld1q_f32(kernel0 + 6);
    #else
                const float* k0 = kernel0;
                const float* k1 = kernel0 + 3;
                const float* k2 = kernel0 + 6;
    #endif

                int i = 0;
                

                for(; i < outHeight; i++){
    #if USE_NEON
                    int nn = outWidth >> 2;
                    int remain = outWidth - (nn << 2);
    #else
                    int remain = outWidth;
    #endif

    #if USE_NEON

    #if __aarch64__
                    throw Exception(1, "Error: armv8 temporarily not supported!", __FILE__, __LINE__, __FUNCTION__);
    #else    
                    if(nn > 0){
                        asm volatile(
                           
                    } 
    #endif
                    

    #endif

                    for(; remain > 0; remain--){
                        
    #if USE_NEON
                        float32x4_t r00 = vld1q_f32(r0);
                        float32x4_t r10 = vld1q_f32(r1);
                        float32x4_t r20 = vld1q_f32(r2);

                        float32x4_t sum0 = vmulq_f32(r00, k012);
                        sum0 = vmlaq_f32(sum0, r10, k345);
                        sum0 = vmlaq_f32(sum0, r20, k678);

                        sum0 = vsetq_lane_f32(*destptr0, sum0, 3);
    #if __aarch64__
                        *destptr0 = vaddvq_f32(sum0);
    #else
                        float32x2_t _ss0 = vadd_f32(vget_low_f32(sum0), vget_high_f32(sum0));
                        _ss0 = vpadd_f32(_ss0, _ss0);

                        *destptr0 = vget_lane_f32(_ss0, 0);
    #endif

    #else

                        float sum0 = 0;

                        sum0 += r0[0] * k0[0];
                        sum0 += r0[1] * k0[1];
                        sum0 += r0[2] * k0[2];
                        sum0 += r1[0] * k1[0];
                        sum0 += r1[1] * k1[1];
                        sum0 += r1[2] * k1[2];
                        sum0 += r2[0] * k2[0];
                        sum0 += r2[1] * k2[1];
                        sum0 += r2[2] * k2[2];

                        *destptr0 += sum0;
    #endif
                        r0 += 2;
                        r1 += 2;
                        r2 += 2;
                        destptr0++;
                    }

                    r0 += 2 * (inWidth - outWidth);
                    r1 += 2 * (inWidth - outWidth);
                    r2 += 2 * (inWidth - outWidth);
                }
                kernel0 += 9;
            }
        }
    }