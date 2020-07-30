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

            int EN_Y = Y + Radis;
            if(EN_Y > Height) EN_Y = Height;

            
        }
    }
}
