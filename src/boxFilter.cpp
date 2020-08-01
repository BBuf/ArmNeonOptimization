#include "boxFilter.h"
#include <chrono>
#include <random>
#include <iostream>

#if __ARM_NEON
#include <arm_neon.h>
#endif // __ARM_NEON

void BoxFilterOrigin(unsigned char *Src, unsigned char *Dest, int Width, int Height, int Radius){
    for(int Y = 0; Y < Height; Y++){
        for(int X = 0; X < Width; X++){
            int ST_Y = Y - Radius;
            if(ST_Y < 0) ST_Y = 0;
            int EN_Y = Y + Radius;
            if(EN_Y > Height-1) EN_Y = Height-1;
            int ST_X = X - Radius;
            if(ST_X < 0) ST_X = 0;
            int EN_X = X + Radius;
            if(EN_X > Width-1) EN_X = Width-1;
            float sum = 0;
            for(int ty = ST_Y; ty <= EN_Y; ty++){
                for(int tx = ST_X; tx <= EN_X; tx++){
                    sum += Src[ty * Width + tx];
                }
            }
            Dest[Y * Width + X] = sum;
        }
    }
}
