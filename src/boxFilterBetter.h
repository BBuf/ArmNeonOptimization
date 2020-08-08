#include <vector>
#include <iostream>
#define __ARM_NEON 1
using namespace std;

void BoxFilterBetterOrigin(float *Src, float *Dest, int Width, int Height, int Radius);