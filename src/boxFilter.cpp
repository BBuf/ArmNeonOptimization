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


void BoxFilterOpenCV(unsigned char *Src, unsigned char *Dest, int Width, int Height, int Radius, vector <float>&cache){
    float *cachePtr = &(cache[0]);
    // chuizhi
    for(int Y = 0; Y < Height; Y++){
        for(int X = 0; X < Width; X++){
            int ST_X = X - Radius;
            if(ST_X < 0) ST_X = 0;
            int EN_X = X + Radius;
            if(EN_X > Width-1) EN_X = Width-1;
            float sum = 0;
            for(int tx = ST_X; tx <= EN_X; tx++){
                sum += Src[Y * Width + tx];
            }
            cachePtr[Y * Width + X] = sum;
        }
    }

    //shuiping
    for(int Y = 0; Y < Height; Y++){
        int ST_Y = Y - Radius;
        if(ST_Y < 0) ST_Y = 0;
        int EN_Y = Y + Radius;
        if(EN_Y > Height-1) EN_Y = Height-1;
        for(int X = 0; X < Width; X++){
            float sum = 0;
            for(int ty = ST_Y; ty <= EN_Y; ty++){
                sum += cachePtr[ty * Width + X];
            }
            Dest[Y * Width + X] = sum;
        }
    }
}

 void BoxFilterOpenCV2(unsigned char *Src, unsigned char *Dest, int Width, int Height, int Radius, vector<float>&cache){
     float *cachePtr = &(cache[0]);
     //chuizhi
     for(int Y = 0; Y < Height; Y++){
         int Stride = Y * Width;
         //head
         float sum = 0;
         for(int X = 0; X < Radius; X++){
             sum += Src[Stride + X];
         }
         for(int X = 0; X <= Radius; X++){
             sum += Src[Stride + X + Radius];
             cachePtr[Stride + X] = sum;
         }
        //middle
        for(int X = Radius+1; X <= Width-1-Radius; X++){
            sum += Src[Stride + X + Radius];
            sum -= Src[Stride + X - Radius - 1];
            cachePtr[Stride + X] = sum;
        }
        //tail
        for(int X = Width - Radius; X < Width; X++){
            sum -= Src[Stride + X - Radius - 1];
            cachePtr[Stride + X] = sum;
        }
     }
    //shuipin

    for(int X = 0; X < Width; X++){
        //head
        float sum = 0;
        for(int Y = 0; Y < Radius; Y++){
            sum += cachePtr[Y * Width + X];
        }
        for(int Y = 0; Y <= Radius; Y++){
            sum += cachePtr[Y * Width + Radius * Width + X];
            Dest[Y * Width + X] = sum;
        }
        //middle
        for(int Y = Radius+1; Y <= Height-1-Radius; Y++){
            sum += cachePtr[Y * Width + Radius * Width + X];
            sum -= cachePtr[Y * Width - (Radius + 1) * Width + X];
            Dest[Y * Width + X] = sum;
        }
        //tail
        for(int Y = Height-Radius; Y < Height; Y++){
            sum -= cachePtr[Y * Width - (Radius + 1) * Width + X];
            Dest[Y * Width + X] = sum;
        }
    }    
 }

void BoxFilterCache(unsigned char *Src, unsigned char *Dest, int Width, int Height, int Radius, vector<float>&cache){
    float *cachePtr = &(cache[0]);
     //chuizhi
    for(int Y = 0; Y < Height; Y++){
        int Stride = Y * Width;
        //head
        float sum = 0;
        for(int X = 0; X < Radius; X++){
            sum += Src[Stride + X];
        }
        for(int X = 0; X <= Radius; X++){
            sum += Src[Stride + X + Radius];
            cachePtr[Stride + X] = sum;
        }
        //middle
        for(int X = Radius+1; X <= Width-1-Radius; X++){
            sum += Src[Stride + X + Radius];
            sum -= Src[Stride + X - Radius - 1];
            cachePtr[Stride + X] = sum;
        }
        //tail
        for(int X = Width - Radius; X < Width; X++){
            sum -= Src[Stride + X - Radius - 1];
            cachePtr[Stride + X] = sum;
        }
    }

    vector <float> colsum;
    colsum.resize(Width); 
    float *colsumPtr = &(colsum[0]);
    for(int X = 0;  X < Width; X++){
        colsumPtr[X] = 0;
    }
    //shuipin
    for(int Y = 0; Y < Radius; Y++){
        int Stride = Y * Width;
        for(int X = 0; X < Width; X++){
            colsumPtr[X] += colsumPtr[Stride + X];
        }
    }
    //head
    for(int Y = 0; Y <= Radius; Y++){
        int Stride = Y * Width;
        for(int X = 0; X < Width; X++){
            colsumPtr[X] += cachePtr[(Y + Radius) * Width + X];
            Dest[Stride + X] = colsumPtr[X];
        }
    }
    //middle
    for(int Y = Radius+1; Y <= Height-1-Radius; Y++){
        int Stride = Y * Width;
        for(int X = 0; X < Width; X++){
            colsumPtr[X] += cachePtr[(Y + Radius) * Width + X];
            colsumPtr[X] -= cachePtr[(Y - Radius - 1) * Width + X];
            Dest[Stride + X] = colsumPtr[X];
        }
    }
    //tail
    for(int Y = Height-Radius; Y < Height; Y++){
        int Stride = Y * Width;
        for(int X = 0; X < Width; X++){
            colsumPtr[X] -= cachePtr[(Y - Radius - 1) * Width + X];
            Dest[Stride + X] = colsumPtr[X];
        }
    }
}