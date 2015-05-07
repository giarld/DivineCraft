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

int GMath::g2Int(float x)
{
    if(float(int(x))==x)
        return x;
    if(x>=0.0)
        return int(x);
    else{
        return int(x)-1;
    }
}

QVector3D GMath::v3toInt(const QVector3D &vpos)
{
    return QVector3D(g2Int(vpos.x()),g2Int(vpos.y()),g2Int(vpos.z()));
}
