#include <vector>
#include <iostream>
#define USE_NEON 0
#include <arm_neon.h>
#define USE_OMP 0
#define OMP_THREAD 2
using namespace std;

void BatchNorm(float * &src, const int &inWidth, const int &inHeight,  const int &inChannel,
                    float *const &Scales, float *const &rollMean, float *const &rollVariance, float *const &Biases){
        // a = bias - slope * mean / sqrt(var)
        // b = slope / sqrt(var)
        // value = b * value + a
#if USE_OMP
    #pragma omp parallel for num_threads(OMP_THREAD)
#endif

        int in_size = inWidth * inHeight;
        float *srcPtr = src;
        int nn, remain;
        for(int i = 0; i < inChannel; i++){

            float sqrtVar = sqrt(rollVariance[i]);
            float a = Biases[i] - Scales[i] * rollMean[i] / sqrtVar;
            float b = Scales[i] / sqrtVar;
            
            

            #if USE_NEON
                nn = in_size >> 2;
                remain = in_size - nn << 2;
                float32x4_t a_new = vdupq_n_f32(a);
                float32x4_t b_new = vdupq_n_f32(b);
            #else
                remain = in_size;
            #endif    

            #if USE_NEON
                for(; nn > 0; nn--){
                    #if __aarch64__
                        throw Exception(1, "Error: armv8 temporarily not supported!", __FILE__, __LINE__, __FUNCTION__);
                    #else
                        float32x4_t tmp = vld1q_f32(srcPtr);
                        float32x4_t sum = vmulq_f32(tmp, a_new);
                        sum = vaddq_f32(sum, b);
                        vstlq_f32(srcPtr, sum);
                        srcPtr += 4;
                    #endif 
                }
            #endif

            for(; remain > 0; remain--){
                *srcPtr = b * (*srcPtr) + a;
                srcPtr++;
            }
            srcPtr += in_size;
        }

    }
