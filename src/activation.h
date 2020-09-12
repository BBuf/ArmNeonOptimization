#include <vector>
#include <iostream>
#define USE_NEON 1
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
                int remain = nn << 2;
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