#include "camera.h"
#include <math.h>
#include <string.h>

#define PI 3.14159265358979

float gMin(float a,float b)
{
    return a<=b?a:b;
}

float gMax(float a,float b)
{
    return a>=b?a:b;
}


Camera::Camera(Camera::CameraMode mode, QObject *parent)
    :QObject(parent)
    ,mode(mode)
    ,mPosition(QVector3D(0,0,0))
    ,mRotation(QPointF(0.0,0.0))
    ,mBind(false)
    ,kBind(false)
    ,mouseLevel(0.5)
    ,moveSpeed(0.005)
    ,G(0.05)
    ,MaxSpeed(0.5)
{
    setMouseLevel(mouseLevel);
    reMotionVector();
    setGameMode(Camera::SURVIVAL);
    memset(keyMap,0,sizeof(keyMap));
}

Camera::Camera(const QVector3D &position, const QPointF &rotation, Camera::CameraMode mode, QObject *parent)
    :QObject(parent)
    ,mode(mode)
    ,mPosition(position)
    ,mRotation(rotation)
    ,mBind(false)
    ,kBind(false)
    ,mouseLevel(0.5)
    ,moveSpeed(0.005)
    ,G(0.05)
    ,MaxSpeed(0.5)
{
    setMouseLevel(mouseLevel);
    reMotionVector();
    setGameMode(Camera::SURVIVAL);
    memset(keyMap,0,sizeof(keyMap));
}

void Camera::sightMove(const QPointF &dp)
{
    if(!mBind)
        return ;
    //    qDebug()<<dp;
    float xr=dp.x()*dlAngle;
    float yr=dp.y()*dlAngle;
    float x=mRotation.x()+xr;
    float y=mRotation.y()+yr;
    y=gMax(-89.9,gMin(89.9,y));                                         //俯仰度在-90到90度之间
    mRotation=QPointF(x,y);
    reMotionVector();
}

void Camera::keyPress(const int key)
{
    if(!kBind)
        return ;
    if(key==Qt::Key_W){
        keyMap[0]=true;
    }
    if(key==Qt::Key_S){
        keyMap[1]=true;
    }
    if(key==Qt::Key_A){
        keyMap[2]=true;
    }
    if(key==Qt::Key_D){
        keyMap[3]=true;
    }
    if(key==Qt::Key_Space){
        keyMap[4]=true;
    }
    if(key==Qt::Key_Shift){
        keyMap[5]=true;
    }

}

void Camera::keyRelease(const int key)
{
    if(key==Qt::Key_W){
        keyMap[0]=false;
    }
    if(key==Qt::Key_S){
        keyMap[1]=false;
    }
    if(key==Qt::Key_A){
        keyMap[2]=false;
    }
    if(key==Qt::Key_D){
        keyMap[3]=false;
    }
    if(key==Qt::Key_Space){
        keyMap[4]=false;
    }
    if(key==Qt::Key_Shift){
        keyMap[5]=false;
    }
}

void Camera::bind()
{
    mBind=kBind=true;
    lastTime=QTime::currentTime();
    ySpeed=0.0;
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
    if(value<0.01)
        value=0.01;
    if(value>1.0)
        value=1.0;
    mouseLevel = gMax(0.0,gMin(value,1.0));
    dlAngle=mouseLevel*0.1f;
}
QVector3D Camera::position() const
{
    return mPosition;
}

void Camera::setPosition(const QVector3D &position)
{
    mPosition = position;
}

QVector3D Camera::getEyePosition() const
{
    QVector3D eyeP=position();
    eyeP.setY(eyeP.y()+1.650);
    return eyeP;
}
QPointF Camera::rotation() const
{
    return mRotation;
}

void Camera::setRotation(const QPointF &rotation)
{
    mRotation = rotation;
}

float Camera::radians(float angle)
{
    return angle*(PI/180.0);
}

void Camera::cMove()
{
    if(!kBind){
        lastTime=QTime::currentTime();
        return;
    }

    QTime nowTime=QTime::currentTime();
    int timeC=lastTime.msecsTo(nowTime);
    if(timeC<20)
        timeC=20;

    QVector3D strafe;
    if(keyMap[0]){
        strafe+=udMotion;
    }
    if(keyMap[1]){
        strafe-=udMotion;
    }
    if(keyMap[2]){
        strafe+=lfMotion;
    }
    if(keyMap[3]){
        strafe-=lfMotion;
    }
    if(keyMap[4]){
        if(gameMode==GOD){
            strafe.setY(strafe.y()+1.0);
        }
        else if(gameMode==SURVIVAL){
            if(ySpeed==0){
                ySpeed=MaxSpeed;
            }
        }
    }
    if(keyMap[5]){
        if(gameMode==GOD){
            strafe.setY(strafe.y()-1.0);
        }
        else if(gameMode==SURVIVAL){

        }
    }

    if(gameMode==SURVIVAL){
        if(mPosition.y()>0){
            ySpeed-=(G*timeC);
        }
    }
    if(mPosition.y()<=0)
        ySpeed=0;
    strafe.setY(strafe.y()+ySpeed*timeC);
    strafe.normalize();                                             //方向矢量单位化
    mPosition+=(strafe*timeC*moveSpeed);

    lastTime=nowTime;
}

void Camera::reMotionVector()
{
    QVector3D sv=getSightVector();
    udMotion=QVector3D(sv.x(),0,sv.z());
    udMotion.normalize();
    lfMotion=QVector3D(udMotion.z(),0,-udMotion.x());
    lfMotion.normalize();
}
int Camera::getGameMode() const
{
    return gameMode;
}

void Camera::setGameMode(const int &value)
{
    gameMode = value;
}




