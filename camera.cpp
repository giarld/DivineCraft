#include "camera.h"

Camera::Camera(Camera::CameraMode mode)
    :mode(mode)
    ,mPosition(QVector3D(0,0,0))
    ,mRotation(QVector3D(-1,0,0))
{
    mRotation.normalize();
}

Camera::Camera(const QVector3D &position, const QVector3D &rotation, Camera::CameraMode mode)
    :mPosition(position)
    ,mRotation(rotation)
    ,mode(mode)
{
    mRotation.normalize();
}
