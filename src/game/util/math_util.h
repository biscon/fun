//
// Created by bison on 15-10-2017.
//

#ifndef CRPG_MATH_H
#define CRPG_MATH_H

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

double remapDouble(double oMin, double oMax, double nMin, double nMax, double x);
float remapFloat(float oMin, float oMax, float nMin, float nMax, float x);

#endif //CRPG_MATH_H
