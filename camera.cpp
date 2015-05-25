/*
 * Author:Gxin
 */
#include "camera.h"
#include <math.h>
#include <string.h>
#include "gmath.h"
#include <QDataStream>

QString MyName="xm";

Camera::Camera(Camera::CameraMode mode, QObject *parent)
    :QObject(parent)
    ,mode(mode)
    ,mBind(false)
    ,kBind(false)
{
    setDefaultValue();
    setMouseLevel(mouseLevel);
    setPosition(QVector3D(0,0,0));
    setRotation(QPointF(0.0,0.0));
    reMotionVector();
    setGameMode(Camera::SURVIVAL);
    memset(keyMap,0,sizeof(keyMap));
    setPause(true);
    setBlockId(1);
}

Camera::Camera(const QVector3D &position, const QPointF &rotation, Camera::CameraMode mode, QObject *parent)
    :QObject(parent)
    ,mode(mode)
    ,mBind(false)
    ,kBind(false)
{
    setDefaultValue();
    setMouseLevel(mouseLevel);
    setPosition(position);
    setRotation(rotation);
    reMotionVector();
    setGameMode(Camera::SURVIVAL);
    memset(keyMap,0,sizeof(keyMap));
    setPause(true);
    setBlockId(1);
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
    QVector3D vec=QVector3D(dx,-dy,dz);
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

void Camera::loadPosRot()
{
    if(myWorld==NULL){
        qWarning("警告:可能存在对Camera::loadPosRot的调用顺序错误，本次调用已被判定为无效");
        return;
    }
    QString filePath=tr("%1/%2.dat").arg(myWorld->getFilePath()).arg(MyName);
    QFile file(filePath);
    if(file.open(QIODevice::ReadOnly)){
        QDataStream in(&file);
        int ak;
        QVector3D pos=this->position();
        QPointF rot=this->rotation();
        in>>ak;
        if(ak==0x54cbff){
            in>>pos>>rot;
        }
        setPosition(pos);
        setRotation(rot);
        if(myWorld)                                 //进行初始位置设定
            myWorld->setFirstCameraPosition(mPosition);
    }
    else{
        savePosRot();
    }
    file.close();
}

void Camera::savePosRot()
{
    QString filePath=tr("%1/%2.dat").arg(myWorld->getFilePath()).arg(MyName);
    QFile file(filePath);
    if(file.open(QIODevice::WriteOnly)){
        QDataStream out(&file);
        out<<0x54cbff<<position()<<rotation();
    }
    file.close();
}

QVector3D Camera::getEyePosition() const
{
    QVector3D eyeP=position();
    eyeP.setY(eyeP.y()+1.650);
    return eyeP;
}

QVector3D Camera::getKeyPosition() const
{
    return keyBlock;
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
//    if(timeC<20)
//        timeC=20;

    QVector3D strafe(0,0,0);
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
                mPosition.setY(mPosition.y()+0.1);
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
    collision(strafe,timeC);
    //进行鼠标可操作方块检测
    hitTest();

    //    qDebug()<<getEyePosition()<<keyBlock<<preBlock;
    //        qDebug()<<mPosition.y();

    emit cameraMove(mPosition);                     //发出移动了的信号
    emit getPositions(position(),getEyePosition());

    lastTime=nowTime;
}

void Camera::addBlock()
{
    if(pause){
        return ;
    }
    if(preBlock.y()<0)              //preBlock没有指定位置或人站在该位置上，则忽略
        return ;

    float h=1.8;                                        //身高
    for(float i=0;i<h;i+=0.2){
        QVector3D temp=GMath::v3toInt(QVector3D(mPosition.x(),mPosition.y()+i,mPosition.z()));
        if(preBlock==temp)
            return ;
    }

    myWorld->addBlock(new Block(preBlock,myWorld->getBlockIndex(this->blockId)),true);
}

void Camera::setBlockId(int id)
{
    blockId=id;
}

void Camera::removeBlock()
{
    if(pause){
        return ;
    }
    if(keyBlock.y()<0)              //keyBlock没有指定位置或人站在该位置上，则忽略
        return ;
    myWorld->removeBlock(keyBlock,true);
}

void Camera::setDefaultValue()
{
    mouseLevel=0.5;
    moveSpeed=0.0035;
    jumSpeed=0.0100;
    G=0.00003;
    MaxSpeed=0.1;
}

void Camera::reMotionVector()
{
    QVector3D sv=getSightVector();
    udMotion=QVector3D(sv.x(),0,sv.z());
    udMotion.normalize();
    lfMotion=QVector3D(udMotion.z(),0,-udMotion.x());
    lfMotion.normalize();
}

void Camera::hitTest()
{
    int handLen=8;      //手长
    int mm=8;               //分片精度
    QVector3D keyB;
    QVector3D preB=QVector3D(0,-100,0);           //一个默认的实体坐标
    QVector3D temp=getEyePosition();
    float tx=temp.x();
    float ty=temp.y();
    float tz=temp.z();
    QVector3D sightVector=getSightVector();

    preBlock=keyBlock=preB;                         //初始化放置位置和选定实体

    for(int i=0;i<handLen*mm;i++){
        keyB=GMath::v3toInt(QVector3D(tx,ty,tz));
        Block *tb=myWorld->getBlock(keyB);
        if(keyB!=preB && tb!=NULL && !tb->isAir()){      //找到一个实体方块
            keyBlock=keyB;
            preBlock=preB;
            break;
        }
        preB=keyB;
        tx=tx+sightVector.x()/mm*1.0;
        ty=ty+sightVector.y()/mm*1.0;
        tz=tz+sightVector.z()/mm*1.0;
    }
}

void Camera::collision(QVector3D strafe,int timeC)
{
    strafe.normalize();                                             //方向矢量单位化

    float m=0.25;                                       //体胖
    float h=1.8;                                        //身高

    float x=mPosition.x();
    float y=mPosition.y();
    float z=mPosition.z();

    QVector3D mBPos=GMath::v3toInt(mPosition);         //当前所在的方块坐标
    QVector3D newPosition=mPosition+(strafe*timeC*moveSpeed);               //新的坐标

    if(gameMode==GOD)
        ySpeed=0.0;
    else if(gameMode==SURVIVAL){         //跳跃动作
        if(ySpeed>-MaxSpeed){
            ySpeed-=(G*timeC);
        }
        newPosition.setY(y+ySpeed*timeC);
    }

    //x方向碰撞检测
    for(float i=0;i<h;i+=0.2){
        if(myWorld->collision(QVector3D(newPosition.x()+(strafe.x()>=0?m:-m),y+i,newPosition.z()))){
            if(strafe.x()<0){
                newPosition.setX(mBPos.x()+m);
            }
            else{
                newPosition.setX(mBPos.x()+1-m);
            }
            break;
        }
    }

    //z方向碰撞检测
    for(float i=0;i<h;i+=0.2){
        if(myWorld->collision(QVector3D(newPosition.x(),y+i,newPosition.z()+(strafe.z()>=0?m:-m)))){
            if(strafe.z()<0){
                newPosition.setZ(mBPos.z()+m);
            }
            else{
                newPosition.setZ(mBPos.z()+1-m);
            }
            break;
        }
    }

    //y方向的检测
    if(myWorld->collision(QVector3D(x,newPosition.y(),z))){             //脚踩在方块上
        newPosition.setY(mBPos.y());
        ySpeed=0.0;
    }

    if(myWorld->collision(QVector3D(x,newPosition.y()+h,z))){                   //头顶住方块，回弹
        newPosition.setY(float(GMath::g2Int(newPosition.y()+h))-h);
        ySpeed=-G;
    }

    mPosition=newPosition;
}
bool Camera::getPause() const
{
    return pause;
}

void Camera::setPause(bool value)
{
    pause = value;
}

int Camera::getBlockId()
{
    return blockId;
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




