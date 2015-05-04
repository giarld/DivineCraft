#include "camera.h"
#include <math.h>
#include <string.h>
#include "gmath.h"

Camera::Camera(Camera::CameraMode mode, QObject *parent)
    :QObject(parent)
    ,mode(mode)
    ,mRotation(QPointF(0.0,0.0))
    ,mBind(false)
    ,kBind(false)
    ,mouseLevel(0.5)
    ,moveSpeed(0.005)
    ,jumSpeed(0.0125)
    ,G(0.00005)
    ,MaxSpeed(0.1)
{
    setMouseLevel(mouseLevel);
    setPosition(QVector3D(0,0,0));
    reMotionVector();
    setGameMode(Camera::SURVIVAL);
    memset(keyMap,0,sizeof(keyMap));
    setPause(true);
}

Camera::Camera(const QVector3D &position, const QPointF &rotation, Camera::CameraMode mode, QObject *parent)
    :QObject(parent)
    ,mode(mode)
    ,mRotation(rotation)
    ,mBind(false)
    ,kBind(false)
    ,mouseLevel(0.5)
    ,moveSpeed(0.005)
    ,jumSpeed(0.0125)
    ,G(0.00005)
    ,MaxSpeed(0.1)
{
    setMouseLevel(mouseLevel);
    setPosition(position);
    reMotionVector();
    setGameMode(Camera::SURVIVAL);
    memset(keyMap,0,sizeof(keyMap));
    setPause(true);
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
    y=GMath::gMax(-89.999,GMath::gMin(89.999,y));                                         //俯仰度在-89.999到89.999度之间(不限制在90度是为了让位移向量能识别到前进方向)
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
}

void Camera::unBind()
{
    mBind=kBind=false;
}

QVector3D Camera::getSightVector() const
{
    float x=mRotation.x();
    float y=mRotation.y();

    float m=cos(GMath::radians(y));
    float dy=sin(GMath::radians(y));
    float dx=cos(GMath::radians(x-90.0))*m;
    float dz=sin(GMath::radians(x-90.0))*m;
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
    mouseLevel = GMath::gMax(0.0,GMath::gMin(value,1.0));
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

void Camera::cMove()
{
    if(pause){                                                  //暂停时的处理
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
            if(ySpeed==0.0){
                ySpeed=jumSpeed;
                mPosition.setY(mPosition.y()+0.0001);
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

    //!!碰撞检测

    float x=mPosition.x();
    float y=mPosition.y();
    float z=mPosition.z();

//===================================================
    strafe.normalize();                                             //方向矢量单位化
    mPosition+=(strafe*timeC*moveSpeed);
    if(mPosition.y()<=0.0){
        ySpeed=0;
        mPosition.setY(0.0);
    }
    if(gameMode==SURVIVAL && mPosition.y()>0){
        if(ySpeed>-MaxSpeed){
            ySpeed-=(G*timeC);
        }
        mPosition.setY(mPosition.y()+ySpeed*timeC);
    }
    else if(gameMode==GOD)
        ySpeed=0.0;
//    qDebug()<<mPosition.y();

    emit cameraMove(mPosition);                                                     //发出移动了的信号

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
bool Camera::getPause() const
{
    return pause;
}

void Camera::setPause(bool value)
{
    pause = value;
}


void Camera::setWorld(World *value)
{
    myWorld = value;
}

float Camera::getG() const
{
    return G;
}

void Camera::setG(float value)
{
    G = value;
}

int Camera::getGameMode() const
{
    return gameMode;
}

void Camera::setGameMode(const int &value)
{
        gameMode = value;
}




