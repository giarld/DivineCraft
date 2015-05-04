#ifndef GMATH_H
#define GMATH_H

#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <math.h>

//自定义的数学公式类
class GMath
{
public:
    static float gMin(float a,float b);
    static float gMax(float a,float b);
    static QVector2D v3d2v2d(const QVector3D & v3d);                            //将QVector3D转换为QVector2D,舍弃y
    static float radians(float angle);              //角度转换为弧度
    static int gAbs(int x);
    static int f2int(float a);
};

#endif // GMATH_H
