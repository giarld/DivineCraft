#ifndef CAMERA_H
#define CAMERA_H

#include <QtWidgets>

#include <QVector3D>
#include <QQuaternion>

class Camera
{
public:
    enum CameraMode{
        My,
        Your,
        Her
    };
    Camera(CameraMode mode=My);
    Camera(const QVector3D &position,const QVector3D &rotation ,CameraMode mode=My);


public:
    enum{
        WALK_UP=0x10,
        WALK_DOWN,
        WALK_LEFT,
        WALK_RIGHT
    };

private:
    int mode;
    QVector3D mPosition;                   //camera坐标
    QVector3D mRotation;                  //camera方向(单位向量)

};

#endif // CAMERA_H
