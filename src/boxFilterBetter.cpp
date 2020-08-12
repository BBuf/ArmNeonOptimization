#if __ARM_NEON
#include <arm_neon.h>
#endif // __ARM_NEON
#include "boxFilterBetter.h"

void BoxFilterBetterOrigin(float *Src, float *Dest, int Width, int Height, int Radius){
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

        r0 += 2 + Width;
        r1 += 2 + Width;
        r2 += 2 + Width;
        r3 += 2 + Width;

        outptr += OutWidth;
        outptr2 += OutWidth;
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
            *outptr = sum1;
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
    // 这里虽然 kernel 大小是根据输入设置
    // 但是下面的计算写死了是3x3的kernel
    // boxfilter 权值就是1，直接加法即可，
    // 额外的乘法会增加耗时
    float *kernel = new float[Radius*Radius];
    for(int i = 0; i < Radius*Radius; i++){
        kernel[i] = 1.0;
    }
    // 下面代码，把 kernel 的每一行存一个 q 寄存器
    // 而因为一个 vld1q 会加载 4 个浮点数，比如 k012
    // 会多加载下一行的一个数字，所以下面 
    // 会用 vsetq_lane_f32 把最后一个数字置0
    float32x4_t k012 = vld1q_f32(kernel);
    float32x4_t k345 = vld1q_f32(kernel + 3);
    // 这里 kernel 的空间如果 Radius 设为3
    // 则长度为9，而从6开始读4个，最后一个就读
    // 内存越界了，可能会有潜在的问题。
    float32x4_t k678 = vld1q_f32(kernel + 6);

    k012 = vsetq_lane_f32(0.f, k012, 3);
    k345 = vsetq_lane_f32(0.f, k345, 3);
    k678 = vsetq_lane_f32(0.f, k678, 3);

    // 输入需要同时读4行
    float* r0 = Src;
    float* r1 = Src + Width;
    float* r2 = Src + Width * 2;
    float* r3 = Src + Width * 3;
    float* outptr = Dest;
    float* outptr2 = Dest + OutWidth;
    int i = 0;
    // 同时计算输出两行的结果
    for (; i + 1 < OutHeight; i += 2){
        int remain = OutWidth;
        for(; remain > 0; remain--){
            // 从当前输入位置连续读取4个数据
            float32x4_t r00 = vld1q_f32(r0);
            float32x4_t r10 = vld1q_f32(r1);
            float32x4_t r20 = vld1q_f32(r2);
            float32x4_t r30 = vld1q_f32(r3);

            // 因为 Kernel 最后一个权值置0，所以相当于是
            // 在计算一个 3x3 的卷积点乘累加中间结果
            // 最后的 sum1 中的每个元素之后还需要再加在一起
            // 还需要一个 reduce_sum 操作
            float32x4_t sum1 = vmulq_f32(r00, k012);
            sum1 = vmlaq_f32(sum1, r10, k345);
            sum1 = vmlaq_f32(sum1, r20, k678);

            // 同理计算得到第二行的中间结果
            float32x4_t sum2 = vmulq_f32(r10, k012);
            sum2 = vmlaq_f32(sum2, r20, k345);
            sum2 = vmlaq_f32(sum2, r30, k678);

            // [a,b,c,d]->[a+b,c+d]
            // 累加 这里 vadd 和下面的 vpadd 相当于是在做一个 reduce_sum
            float32x2_t _ss = vadd_f32(vget_low_f32(sum1), vget_high_f32(sum1));
            // [e,f,g,h]->[e+f,g+h]
            float32x2_t _ss2 = vadd_f32(vget_low_f32(sum2), vget_high_f32(sum2));
            // [a+b+c+d,e+f+g+h]
            // 这里因为 intrinsic 最小的单位是 64 位，所以用 vpadd_f32 把第一行和第二行最后结果拼在一起了
            float32x2_t _sss2 = vpadd_f32(_ss, _ss2);
            // _sss2第一个元素 存回第一行outptr
            *outptr = vget_lane_f32(_sss2, 0);
            *outptr2 = vget_lane_f32(_sss2, 1);
            
            //同样这样直接读4个数据，也会有读越界的风险
            r0++;
            r1++;
            r2++;
            r3++;
            outptr++;
            outptr2++;
        }
        
        r0 += 2 + Width;
        r1 += 2 + Width;
        r2 += 2 + Width;
        r3 += 2 + Width;

        outptr += OutWidth;
        outptr2 += OutWidth;
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
	//注意这个过程是计算盒子滤波，所以不会像NCNN一样考虑Bias
    for (; i + 1 < OutHeight; i += 2){
        // 在循环体内每行同时计算4个输出
        // 同时计算两行，也就是一次输出 2x4 个点
        int nn = OutWidth >> 2;
        int remain = OutWidth - (nn << 2);


        //q9->[d18, d19]
        //q10->[d20, 0]
        //neon assembly
                //         : "0"(nn),
                // "1"(outptr),
                // "2"(outptr2),
                // "3"(r0),
                // "4"(r1),
                // "5"(r2),
                // "6"(r3),
                // "w"(k012), // %14
                // "w"(k345), // %15
                // "w"(k678) // %16
        if(nn > 0){
            asm volatile(
                "pld        [%3, #192]          \n"
                // 因为每一行连续计算 4 个输出，所以连续加载 
                // 6个数据即可，4个窗口移动步长为1，有重叠
                // r0 原来的内存排布 [a, b, c, d, e, f]
                // d18 -> [a, b], r19 -> [c, d], r20 -> [e, f]
                "vld1.f32   {d18-d20}, [%3 :64] \n" //r0
                // r0 指针移动到下一次读取起始位置也就是 e
                "add        %3, #16             \n" 

                // q9 = [d18, d19] = [a, b, c, d]
                // q10 = [d20, d21] = [e, f, *, *]
                // q11 = [b, c, d, e]
                // q12 = [c, d, e, f]
                // 关于 vext 见：https://community.arm.com/developer/ip-products/processors/b/processors-ip-blog/posts/coding-for-neon---part-5-rearranging-vectors
                // 
                "vext.32    q11, q9, q10, #1    \n" 
                "vext.32    q12, q9, q10, #2    \n"

                "0:                             \n"

                // 这里计算有点巧妙
                // 首先因为4个卷积窗口之间是部分重叠的
                // q9 其实可以看做是4个连续窗口的第1个元素排在一起
                // q11 可以看做是4个连续窗口的第2个元素排在一起
                // q12 可以看做是4个连续窗口的第3个元素排在一起

                // 原来连续4个卷积窗口对应的数据是 
                // [a, b, c], [b, c, d], [c, d, e], [d, e, f]
                // 现在相当于 是数据做了下重排，但是重排的方式很巧妙
                // q9 = [a, b, c, d]
                // q11 = [b, c, d, e]
                // q12 = [c, d, e, f]

                // 然后下面的代码就很直观了，q9 和 k012 权值第1个权值相乘
                // 因为 4 个窗口的第1个元素就只和 k012 第1个权值相乘
                // %14 指 k012，假设 %14 放 q0 寄存器，%e 表示取 d0, %f指取 d1
                "vmul.f32   q7, q9, %e14[0]     \n" //
                // 4 个窗口的第2个元素就只和 k012 第2个权值相乘
                "vmul.f32   q6, q11, %e14[1]    \n" //
                // 4 个窗口的第3个元素就只和 k012 第3个权值相乘
                // 这样子窗口之间的计算结果就可以直接累加
                // 然后q13相当于只算了3x3卷积第一行 1x3 卷积，中间结果
                // 下面指令是把剩下 的 两行计算完
                "vmul.f32   q13, q12, %f14[0]   \n" 

                 // 计算第二行
                "pld        [%4, #192]          \n"
                "vld1.f32   {d18-d20}, [%4]     \n" // r1
                "add        %4, #16             \n"

				//把第二行的[a, b, c, d] 和 k345 的第1个权值相乘，然后累加到q7寄存器上
                "vmla.f32   q7, q9, %e15[0]     \n"

                "vext.32    q11, q9, q10, #1    \n"
                "vext.32    q12, q9, q10, #2    \n"
				//把第二行的[b, c, d, e] 和 k345 的第2个权值相乘，然后累加到q6寄存器上
                "vmla.f32   q6, q11, %e15[1]    \n"
				//把第三行的[c, d, e, f] 和 k345 的第3个权值相乘，然后累加到q13寄存器上
                "vmla.f32   q13, q12, %f15[0]   \n"


				// 为outptr2做准备，计算第二行的 [a, b, c, d, e, f] 和 k012 的乘积
				// 把第二行的 [a, b, c, d] 和 k012的第1个权值相乘，赋值给q8寄存器
                "vmul.f32   q8, q9, %e14[0]     \n"
				// 把第二行的 [b, c, d, e] 和 k012的第2个权值相乘，赋值给q14寄存器
                "vmul.f32   q14, q11, %e14[1]   \n"
				// 把第二行的 [c, d, e, f] 和 k012的第3个权值相乘，赋值给q15寄存器
                "vmul.f32   q15, q12, %f14[0]   \n"
				
				//和上面的过程完全一致，这里是针对第三行
                "pld        [%5, #192]          \n"
                "vld1.f32   {d18-d20}, [%5 :64] \n" // r2
                "add        %5, #16             \n"
				// 把第三行的 [a, b, c, d] 和 k678 的第1个权值相乘，然后累加到q7寄存器上
                "vmla.f32   q7, q9, %e16[0]     \n"
				
                "vext.32    q11, q9, q10, #1    \n"
                "vext.32    q12, q9, q10, #2    \n"

				// 把第三行的 [b, c, d, e] 和 k678 的第2个权值相乘，然后累加到q6寄存器上
                "vmla.f32   q6, q11, %e16[1]    \n"
				// 把第三行的 [c, d, e, f] 和 k678 的第3个权值相乘，然后累加到q13寄存器上
                "vmla.f32   q13, q12, %f16[0]   \n"

				// 把第三行的 [a, b, c, d] 和 k345 的第1个权值相乘，然后累加到q8寄存器上
                "vmla.f32   q8, q9, %e15[0]     \n"
				// 把第三行的 [b, c, d, e] 和 k345 的第2个权值相乘，然后累加到q14寄存器
                "vmla.f32   q14, q11, %e15[1]   \n"
				// 把第三行的 [c, d, e, f] 和 k345 的第3个权值相乘，然后累加到q15寄存器
                "vmla.f32   q15, q12, %f15[0]   \n"

                "pld        [%6, #192]          \n"
                "vld1.f32   {d18-d20}, [%6]     \n" // r3
                "add        %6, #16             \n"

				// 把第四行的 [a, b, c, d] 和 k678 的第1个权值相乘，然后累加到q8寄存器上
                "vmla.f32   q8, q9, %e16[0]     \n"

                "vext.32    q11, q9, q10, #1    \n"
                "vext.32    q12, q9, q10, #2    \n"

				// 把第四行的 [b, c, d, e] 和 k678 的第2个权值相乘，然后累加到q14寄存器上
                "vmla.f32   q14, q11, %e16[1]   \n"
				// 把第四行的 [c, d, e, f] 和 k678 的第3个权值相乘，然后累加到q15寄存器上
                "vmla.f32   q15, q12, %f16[0]   \n"

                "vadd.f32   q7, q7, q6          \n" // 将q6和q7累加到q7上，针对的是outptr

                "pld        [%3, #192]          \n"
                "vld1.f32   {d18-d20}, [%3 :64] \n" // r0

                "vadd.f32   q8, q8, q14         \n" // 将q14和q8累加到q8上，针对的是outptr2
                "vadd.f32   q7, q7, q13         \n" // 将q13累加到q7上，针对的是outptr
                "vadd.f32   q8, q8, q15         \n" // 将q15和q8累加到q8上，针对的是outptr2

                "vext.32    q11, q9, q10, #1    \n"
                "vext.32    q12, q9, q10, #2    \n"

                "add        %3, #16             \n"

                "vst1.f32   {d14-d15}, [%1]!    \n" // 将q7寄存器的值存储到outptr
                "vst1.f32   {d16-d17}, [%2]!    \n" // 将q8寄存器的值存储到outptr2

                "subs       %0, #1              \n" // nn -= 1
                "bne        0b                  \n" // 判断条件：nn != 0

                "sub        %3, #16             \n" // 
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
                "w"(k678) // %16
                : "cc", "memory", "q6", "q7", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
            );
        }

        for(; remain > 0; remain--){
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
        
        r0 += 2 + Width;
        r1 += 2 + Width;
        r2 += 2 + Width;
        r3 += 2 + Width;

        outptr += OutWidth;
        outptr2 += OutWidth;
    }

    for(; i < OutHeight; i++){
        int nn = OutWidth >> 2;
        int remain = OutWidth - (nn << 2);

        if (nn > 0){
            asm volatile(
                "pld        [%2, #192]          \n"
                    "vld1.f32   {d16-d18}, [%2]     \n" // r0
                    "add        %2, #16             \n"

                    "vext.32    q10, q8, q9, #1     \n"
                    "vext.32    q11, q8, q9, #2     \n"

                    "0:                             \n"

                    "vmul.f32   q7, q8, %e10[0]     \n"

                    "vmul.f32   q13, q10, %e10[1]   \n"
                    "vmul.f32   q14, q11, %f10[0]   \n"

                    "pld        [%3, #192]          \n"
                    "vld1.f32   {d16-d18}, [%3]     \n" // r1
                    "add        %3, #16             \n"

                    "vmla.f32   q7, q8, %e11[0]     \n"

                    "vext.32    q10, q8, q9, #1     \n"
                    "vext.32    q11, q8, q9, #2     \n"

                    "vmla.f32   q13, q10, %e11[1]   \n"
                    "vmla.f32   q14, q11, %f11[0]   \n"

                    "pld        [%4, #192]          \n"
                    "vld1.f32   {d16-d18}, [%4]     \n" // r2
                    "add        %4, #16             \n"

                    "vmla.f32   q7, q8, %e12[0]     \n"

                    "vext.32    q10, q8, q9, #1     \n"
                    "vext.32    q11, q8, q9, #2     \n"

                    "vmla.f32   q13, q10, %e12[1]   \n"
                    "vmla.f32   q14, q11, %f12[0]   \n"

                    "pld        [%2, #192]          \n"
                    "vld1.f32   {d16-d18}, [%2]     \n" // r0
                    "add        %2, #16             \n"

                    "vadd.f32   q7, q7, q13         \n"
                    "vadd.f32   q7, q7, q14         \n"

                    "vext.32    q10, q8, q9, #1     \n"
                    "vext.32    q11, q8, q9, #2     \n"

                    "vst1.f32   {d14-d15}, [%1]!    \n"

                    "subs       %0, #1              \n"
                    "bne        0b                  \n"

                    "sub        %2, #16             \n"
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
                    "w"(k678) // %12
                    : "cc", "memory", "q7", "q8", "q9", "q10", "q11", "q12", "q13", "q14"
                );
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

void BoxFilterBetterNeonAssemblyV2(float *Src, float *Dest, int Width, int Height, int Radius){
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
	//注意这个过程是计算盒子滤波，所以不会像NCNN一样考虑Bias
    for (; i + 1 < OutHeight; i += 2){
        // 在循环体内每行同时计算4个输出
        // 同时计算两行，也就是一次输出 2x4 个点
        int nn = OutWidth >> 2;
        int remain = OutWidth - (nn << 2);


        //q9->[d18, d19]
        //q10->[d20, 0]
        //neon assembly
                //         : "0"(nn),
                // "1"(outptr),
                // "2"(outptr2),
                // "3"(r0),
                // "4"(r1),
                // "5"(r2),
                // "6"(r3),
                // "w"(k012), // %14
                // "w"(k345), // %15
                // "w"(k678) // %16
        if(nn > 0){
            asm volatile(
                "pld        [%3, #192]          \n"
                // 因为每一行连续计算 4 个输出，所以连续加载 
                // 6个数据即可，4个窗口移动步长为1，有重叠
                // r0 原来的内存排布 [a, b, c, d, e, f]
                // d18 -> [a, b], r19 -> [c, d], r20 -> [e, f]
                "vld1.f32   {d18-d20}, [%3 :64] \n" //r0
                // r0 指针移动到下一次读取起始位置也就是 e
                "add        %3, #16             \n" 

                // q9 = [d18, d19] = [a, b, c, d]
                // q10 = [d20, d21] = [e, f, *, *]
                // q11 = [b, c, d, e]
                // q12 = [c, d, e, f]
                // 关于 vext 见：https://community.arm.com/developer/ip-products/processors/b/processors-ip-blog/posts/coding-for-neon---part-5-rearranging-vectors
                // 
                "vext.32    q11, q9, q10, #1    \n" 
                "vext.32    q12, q9, q10, #2    \n"

                "0:                             \n"

                // 这里计算有点巧妙
                // 首先因为4个卷积窗口之间是部分重叠的
                // q9 其实可以看做是4个连续窗口的第1个元素排在一起
                // q11 可以看做是4个连续窗口的第2个元素排在一起
                // q12 可以看做是4个连续窗口的第3个元素排在一起

                // 原来连续4个卷积窗口对应的数据是 
                // [a, b, c], [b, c, d], [c, d, e], [d, e, f]
                // 现在相当于 是数据做了下重排，但是重排的方式很巧妙
                // q9 = [a, b, c, d]
                // q11 = [b, c, d, e]
                // q12 = [c, d, e, f]

                // 然后下面的代码就很直观了，q9 和 k012 权值第1个权值相乘
                // 因为 4 个窗口的第1个元素就只和 k012 第1个权值相乘
                // %14 指 k012，假设 %14 放 q0 寄存器，%e 表示取 d0, %f指取 d1
                "vmov.f32   q7, q9     \n" //
                // 4 个窗口的第2个元素就只和 k012 第2个权值相乘
                "vmov.f32   q6, q11    \n" //
                // 4 个窗口的第3个元素就只和 k012 第3个权值相乘
                // 这样子窗口之间的计算结果就可以直接累加
                // 然后q13相当于只算了3x3卷积第一行 1x3 卷积，中间结果
                // 下面指令是把剩下 的 两行计算完
                "vmov.f32   q13, q12   \n" 

                 // 计算第二行
                "pld        [%4, #192]          \n"
                "vld1.f32   {d18-d20}, [%4]     \n" // r1
                "add        %4, #16             \n"

				//把第二行的[a, b, c, d] 和 k345 的第1个权值相乘，然后累加到q7寄存器上
                "vadd.f32   q7, q7, q9     \n"

                "vext.32    q11, q9, q10, #1    \n"
                "vext.32    q12, q9, q10, #2    \n"
				//把第二行的[b, c, d, e] 和 k345 的第2个权值相乘，然后累加到q6寄存器上
                "vadd.f32   q6, q11, q6    \n"
				//把第三行的[c, d, e, f] 和 k345 的第3个权值相乘，然后累加到q13寄存器上
                "vadd.f32   q13, q12, q13   \n"


				// 为outptr2做准备，计算第二行的 [a, b, c, d, e, f] 和 k012 的乘积
				// 把第二行的 [a, b, c, d] 和 k012的第1个权值相乘，赋值给q8寄存器
                "vmov.f32   q8, q9     \n"
				// 把第二行的 [b, c, d, e] 和 k012的第2个权值相乘，赋值给q14寄存器
                "vmov.f32   q14, q11   \n"
				// 把第二行的 [c, d, e, f] 和 k012的第3个权值相乘，赋值给q15寄存器
                "vmov.f32   q15, q12   \n"
				
				//和上面的过程完全一致，这里是针对第三行
                "pld        [%5, #192]          \n"
                "vld1.f32   {d18-d20}, [%5 :64] \n" // r2
                "add        %5, #16             \n"
				// 把第三行的 [a, b, c, d] 和 k678 的第1个权值相乘，然后累加到q7寄存器上
                "vadd.f32   q7, q9, q7     \n"
				
                "vext.32    q11, q9, q10, #1    \n"
                "vext.32    q12, q9, q10, #2    \n"

				// 把第三行的 [b, c, d, e] 和 k678 的第2个权值相乘，然后累加到q6寄存器上
                "vadd.f32   q6, q11, q6    \n"
				// 把第三行的 [c, d, e, f] 和 k678 的第3个权值相乘，然后累加到q13寄存器上
                "vadd.f32   q13, q12, q13   \n"

				// 把第三行的 [a, b, c, d] 和 k345 的第1个权值相乘，然后累加到q8寄存器上
                "vmov.f32   q8, q9     \n"
				// 把第三行的 [b, c, d, e] 和 k345 的第2个权值相乘，然后累加到q14寄存器
                "vmov.f32   q14, q11   \n"
				// 把第三行的 [c, d, e, f] 和 k345 的第3个权值相乘，然后累加到q15寄存器
                "vmov.f32   q15, q12   \n"

                "pld        [%6, #192]          \n"
                "vld1.f32   {d18-d20}, [%6]     \n" // r3
                "add        %6, #16             \n"

				// 把第四行的 [a, b, c, d] 和 k678 的第1个权值相乘，然后累加到q8寄存器上
                "vmov.f32   q8, q9     \n"

                "vext.32    q11, q9, q10, #1    \n"
                "vext.32    q12, q9, q10, #2    \n"

				// 把第四行的 [b, c, d, e] 和 k678 的第2个权值相乘，然后累加到q14寄存器上
                "vmov.f32   q14, q11   \n"
				// 把第四行的 [c, d, e, f] 和 k678 的第3个权值相乘，然后累加到q15寄存器上
                "vmov.f32   q15, q12   \n"

                "vadd.f32   q7, q7, q6          \n" // 将q6和q7累加到q7上，针对的是outptr

                "pld        [%3, #192]          \n"
                "vld1.f32   {d18-d20}, [%3 :64] \n" // r0

                "vadd.f32   q8, q8, q14         \n" // 将q14和q8累加到q8上，针对的是outptr2
                "vadd.f32   q7, q7, q13         \n" // 将q13累加到q7上，针对的是outptr
                "vadd.f32   q8, q8, q15         \n" // 将q15和q8累加到q8上，针对的是outptr2

                "vext.32    q11, q9, q10, #1    \n"
                "vext.32    q12, q9, q10, #2    \n"

                "add        %3, #16             \n"

                "vst1.f32   {d14-d15}, [%1]!    \n" // 将q7寄存器的值存储到outptr
                "vst1.f32   {d16-d17}, [%2]!    \n" // 将q8寄存器的值存储到outptr2

                "subs       %0, #1              \n" // nn -= 1
                "bne        0b                  \n" // 判断条件：nn != 0

                "sub        %3, #16             \n" // 
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
                "w"(k678) // %16
                : "cc", "memory", "q6", "q7", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
            );
        }

        for(; remain > 0; remain--){
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
        
        r0 += 2 + Width;
        r1 += 2 + Width;
        r2 += 2 + Width;
        r3 += 2 + Width;

        outptr += OutWidth;
        outptr2 += OutWidth;
    }

    for(; i < OutHeight; i++){
        int nn = OutWidth >> 2;
        int remain = OutWidth - (nn << 2);

        if (nn > 0){
            asm volatile(
                "pld        [%2, #192]          \n"
                    "vld1.f32   {d16-d18}, [%2]     \n" // r0
                    "add        %2, #16             \n"

                    "vext.32    q10, q8, q9, #1     \n"
                    "vext.32    q11, q8, q9, #2     \n"

                    "0:                             \n"

                    "vmul.f32   q7, q8, %e10[0]     \n"

                    "vmul.f32   q13, q10, %e10[1]   \n"
                    "vmul.f32   q14, q11, %f10[0]   \n"

                    "pld        [%3, #192]          \n"
                    "vld1.f32   {d16-d18}, [%3]     \n" // r1
                    "add        %3, #16             \n"

                    "vmla.f32   q7, q8, %e11[0]     \n"

                    "vext.32    q10, q8, q9, #1     \n"
                    "vext.32    q11, q8, q9, #2     \n"

                    "vmla.f32   q13, q10, %e11[1]   \n"
                    "vmla.f32   q14, q11, %f11[0]   \n"

                    "pld        [%4, #192]          \n"
                    "vld1.f32   {d16-d18}, [%4]     \n" // r2
                    "add        %4, #16             \n"

                    "vmla.f32   q7, q8, %e12[0]     \n"

                    "vext.32    q10, q8, q9, #1     \n"
                    "vext.32    q11, q8, q9, #2     \n"

                    "vmla.f32   q13, q10, %e12[1]   \n"
                    "vmla.f32   q14, q11, %f12[0]   \n"

                    "pld        [%2, #192]          \n"
                    "vld1.f32   {d16-d18}, [%2]     \n" // r0
                    "add        %2, #16             \n"

                    "vadd.f32   q7, q7, q13         \n"
                    "vadd.f32   q7, q7, q14         \n"

                    "vext.32    q10, q8, q9, #1     \n"
                    "vext.32    q11, q8, q9, #2     \n"

                    "vst1.f32   {d14-d15}, [%1]!    \n"

                    "subs       %0, #1              \n"
                    "bne        0b                  \n"

                    "sub        %2, #16             \n"
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
                    "w"(k678) // %12
                    : "cc", "memory", "q7", "q8", "q9", "q10", "q11", "q12", "q13", "q14"
                );
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