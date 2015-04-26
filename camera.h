#ifndef CAMERA_H
#define CAMERA_H

#include <QtWidgets>
#include <QTime>
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
    Camera(const QVector3D &position,const QPointF &rotation ,CameraMode mode=My);

    void sightMove(const QPointF & dp);                                                                 //视角转动（其实就是鼠标移动）
    void keyPress(int key);

    void bind();                                                                                //绑定键盘与鼠标
    void unBind();                                                                          //解除键盘鼠标的绑定

    QVector3D getSightVector() const;                                                 //返还视线向量

    float getMouseLevel() const;                                            //对于鼠标灵敏度的设置与返还
    void setMouseLevel(float value);

    QVector3D position() const;                                                     //位置坐标的设置与返还
    void setPosition(const QVector3D &position);

    QPointF rotation() const;                                                         //视角向量的设置与返还
    void setRotation(const QPointF &rotation);

private:
    int mode;
    QVector3D mPosition;                   //camera坐标
    QPointF mRotation;                  //camera眼方向(单位向量)
    bool mBind;                                   //鼠标是否处于绑定状态
    bool kBind;                                     //键盘是否处于绑定状态
    float mouseLevel;                           //鼠标灵敏度(0-1)
    float dlAngle;                                      //每一个鼠标偏移方位带来的角度修正

    QTime lastTime;
};

#endif // CAMERA_H
