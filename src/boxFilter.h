#include <vector>
#include <iostream>
#define __ARM_NEON 1
using namespace std;

void BoxFilterOrigin(unsigned char *Src, unsigned char *Dest, int Width, int Height, int Radius);

void BoxFilterOpenCV(unsigned char *Src, unsigned char *Dest, int Width, int Height, int Radius, vector<float>&cache);

void BoxFilterOpenCV2(unsigned char *Src, unsigned char *Dest, int Width, int Height, int Radius, vector<float>&cache);

void BoxFilterCache(unsigned char *Src, unsigned char *Dest, int Width, int Height, int Radius, vector<float>&cache);

void BoxFilterNeonIntrinsics(unsigned char *Src, unsigned char *Dest, int Width, int Height, int Radius, vector<float>&cache);