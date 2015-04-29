#include "gmath.h"

#define PI 3.14159265358979

float GMath::gMin(float a, float b)
{
    return a<=b?a:b;
}

float GMath::gMax(float a, float b)
{
    return a>=b?a:b;
}

QVector2D GMath::v3d2v2d(const QVector3D &v3d)
{
    float x=v3d.x();
    float z=v3d.z();
    return QVector2D(x,z);
}

float GMath::radians(float angle)
{
    return angle*(PI/180.0);
}

int GMath::gAbs(int x)
{
    return x>=0?x:-x;
}

