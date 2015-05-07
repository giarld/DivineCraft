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
    static int gAbs(int x);                                     //整数取绝对值
    static int g2Int(float x);                                                                          //按规则取整
    static QVector3D v3toInt(const QVector3D & vpos);                           //将坐标按坐标系统规则取整数
};

#endif // GMATH_H
