#include <vector>
#include <iostream>
#define USE_NEON 0
#include <arm_neon.h>
#define USE_OMP 1
#define OMP_THREAD 2
using namespace std;

void reluActivate(float * &src, const int &inWidth, const int &inHeight,  const int &inChannel){
    int in_size = inWidth * inHeight;

#if USE_OMP
    #pragma omp parallel for num_threads(OMP_THREAD)
#endif
    for(int i = 0; i < inChannel; i++){

        float *srcPtr = src + i * in_size;

        #if USE_NEON
                int nn = in_size >> 2;
                int remain = in_size - (nn << 2);
        #else
                int remain = in_size;
        #endif

        #if USE_NEON
            if(nn > 0){
                #if __aarch64__
                    throw Exception(1, "Error: armv8 temporarily not supported!", __FILE__, __LINE__, __FUNCTION__);
                #else
                    asm volatile(
                    "veor       q1, q0, q0          \n"
                    
                    "0:                             \n"
                    "pld        [%1, #128]          \n"
                    "vld1.f32   {d0-d1}, [%1]  \n"
                    "vmax.f32   q0, q0, q1          \n"
                    "vst1.f32   {d0-d1}, [%1]! \n"
                    "subs       %0, #1              \n"
                    "bne        0b                  \n"
                    
                    : "=r"(nn), // %0
                    "=r"(srcPtr) // %1
                    : "0"(nn),
                    "1"(srcPtr)
                    : "cc", "memory", "q0", "q1");
                #endif
            }
        #endif

        for(; remain > 0; remain--){
            *srcPtr = std::max(*srcPtr, 0.f);
            srcPtr++;
        }
    }
}


void relu6Activate(float * &src, const int &inWidth, const int &inHeight,  const int &inChannel){
    int in_size = inWidth * inHeight;

    const float six = 6.0;

#if USE_OMP
    #pragma omp parallel for num_threads(OMP_THREAD)
#endif
    for(int i = 0; i < inChannel; i++){

        float *srcPtr = src + i * in_size;

        #if USE_NEON
                int nn = in_size >> 2;
                int remain = in_size - (nn << 2);
        #else
                int remain = in_size;
        #endif

        #if USE_NEON
            if(nn > 0){
                #if __aarch64__
                    throw Exception(1, "Error: armv8 temporarily not supported!", __FILE__, __LINE__, __FUNCTION__);
                #else
                    asm volatile(
                    "veor       q1, q0, q0          \n" 
                    "vdup.f32   q2, %4              \n"

                    "0:                             \n"
                    "pld        [%1, #128]          \n"
                    "vld1.f32   {d0-d1}, [%1]  \n"
                    "vmax.f32   q0, q0, q1          \n"
                    "vmin.f32   q0, q0, q2          \n"

                    "vst1.f32   {d0-d1}, [%1]! \n"

                    "subs       %0, #1              \n"
                    "bne        0b                  \n"
                    : "=r"(nn), // %0
                    "=r"(srcPtr) // %1
                    : "0"(nn),
                    "1"(srcPtr),
                    "r"(six) // %4
                    : "cc", "memory", "q0", "q1");
                #endif
            }
        #endif

        for(; remain > 0; remain--){
            *srcPtr = std::min(std::max(*srcPtr, 0.f), six);
            srcPtr++;
        }
    }
}

void leakyActivate(float * &src, const int &inWidth, const int &inHeight,  const int &inChannel, const float &params){
    int in_size = inWidth * inHeight;

    #if USE_OMP
    #pragma omp parallel for num_threads(OMP_THREAD)
#endif
    for(int i = 0; i < inChannel; i++){

        float *srcPtr = src + i * in_size;

        #if USE_NEON
                int nn = in_size >> 2;
                int remain = in_size - (nn << 2);
        #else
                int remain = in_size;
        #endif

        #if USE_NEON
            if(nn > 0){
                #if __aarch64__
                    throw Exception(1, "Error: armv8 temporarily not supported!", __FILE__, __LINE__, __FUNCTION__);
                #else
                    asm volatile(
                    "veor       q1, q0, q0          \n"
                    // slope
                    "vdup.f32   q2, %4              \n"
                    
                    "0:                             \n"
                    "pld        [%1, #128]          \n"
                    "vld1.f32   {d0-d1}, [%1]       \n"

                    // r[i] = a[i] <= b[i] ? 1: 0
                    "vcle.f32   q3, q0, q1          \n"
                    // 
                    "vmul.f32   q4, q0, q2          \n"
                    // _p = vbslq_f32(_lemask, _ps, _p);
                    "vbit.32    q0, q4, q3          \n"

                    "vst1.f32   {d0-d1}, [%1]! \n"

                    "subs       %0, #1              \n"
                    "bne        0b                  \n"
                    : "=r"(nn), // %0
                    "=r"(srcPtr) // %1
                    : "0"(nn),
                    "1"(srcPtr),
                    "r"(params) // %4
                    : "cc", "memory", "q0", "q1", "q2", "q3", "q4");
                #endif
            }
        #endif

        for(; remain > 0; remain--){
            if((*srcPtr) < 0){
                *srcPtr *= params;
            }
            srcPtr++;
        }
    }
}