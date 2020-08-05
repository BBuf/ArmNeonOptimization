#include <vector>
#include <iostream>
#define __ARM_NEON 1
using namespace std;

void BoxFilterOrigin(float *Src, float *Dest, int Width, int Height, int Radius);

void BoxFilterOpenCV(float *Src, float *Dest, int Width, int Height, int Radius, vector<float>&cache);

void BoxFilterOpenCV2(float *Src, float *Dest, int Width, int Height, int Radius, vector<float>&cache);

void BoxFilterCache(float *Src, float *Dest, int Width, int Height, int Radius, vector<float>&cache);

void BoxFilterNeonIntrinsics(float *Src, float *Dest, int Width, int Height, int Radius, vector<float>&cache);

void BoxFilterNeonAssembly(float *Src, float *Dest, int Width, int Height, int Radius, vector<float>&cache);