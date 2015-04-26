#include "camera.h"
#include <math.h>

#define PI 3.14159265358979

float gMin(float a,float b)
{
    return a<=b?a:b;
}

float gMax(float a,float b)
{
    return a>=b?a:b;
}

float radians(float angle)              //角度转换为弧度
{
    return angle*(PI/180.0);
}

Camera::Camera(Camera::CameraMode mode)
    :mode(mode)
    ,mPosition(QVector3D(0,0,0))
    ,mRotation(QPointF(0.0,0.0))
    ,mBind(false)
    ,kBind(false)
    ,mouseLevel(0.5)
{
    setMouseLevel(mouseLevel);
}

Camera::Camera(const QVector3D &position, const QPointF &rotation, Camera::CameraMode mode)
    :mode(mode)
    ,mPosition(position)
    ,mRotation(rotation)
    ,mBind(false)
    ,kBind(false)
    ,mouseLevel(0.5)
{
    setMouseLevel(mouseLevel);
}

void Camera::sightMove(const QPointF &dp)
{
    if(!mBind)
        return ;
    float xr=dp.x()*dlAngle;
    float yr=dp.y()*dlAngle;
    float x=mRotation.x()+xr;
    float y=mRotation.y()+yr;
    y=gMax(-90.0,gMin(90.0,y));                                         //俯仰度在-90到90度之间
    mRotation=QPointF(x,y);
}

void Camera::keyPress(int key)
{
    if(!kBind)
        return ;
}

void Camera::bind()
{
    mBind=kBind=true;
    lastTime=QTime::currentTime();
}

void Camera::unBind()
{
    mBind=kBind=false;
}

QVector3D Camera::getSightVector() const
{
    float x=mRotation.x();
    float y=mRotation.y();

    float m=cos(radians(y));
    float dy=sin(radians(y));
    float dx=cos(radians(x-90.0))*m;
    float dz=sin(radians(x-90.0))*m;
    QVector3D vec=QVector3D(dx,dy,dz);
    return vec.normalized();
}
float Camera::getMouseLevel() const
{
    return mouseLevel;
}

void Camera::setMouseLevel(float value)
{
    mouseLevel = gMax(0.0,gMin(value,1.0));
    dlAngle=value*5.0f;
}
QVector3D Camera::position() const
{
    return mPosition;
}

void Camera::setPosition(const QVector3D &position)
{
    mPosition = position;
}
QPointF Camera::rotation() const
{
    return mRotation;
}

void Camera::setRotation(const QPointF &rotation)
{
    mRotation = rotation;
}



