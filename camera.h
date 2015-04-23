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
    Camera(const QVector3D &position,const QVector3D &direction,CameraMode mode=My);
    void setCamera(const QVector3D &position,const QVector3D &direction,CameraMode mode=My);
    void walk(float step);                        //行走
    void move(const QPointF &p);                //鼠标移动（眼方向改变）
    void jump(float h);                                                    //跳跃(传入高度)
    void jumpOff(const QVector3D &position);         //跳跃结束，修正位置

    QVector3D getPosition();                        //返还位置坐标
    QVector3D getDirection();                       //返还视觉方向

public:
    enum{
        WALK_UP=0x10,
        WALK_DOWN,
        WALK_LEFT,
        WALK_RIGHT
    };

private:
    QVector3D m_position;                   //camera坐标
    QVector3D m_direction;                  //camera方向
//    QQuaternion s_ratation;                 //场景旋转

};

#endif // CAMERA_H
