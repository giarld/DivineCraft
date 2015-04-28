#ifndef CAMERA_H
#define CAMERA_H

#include <QtWidgets>
#include <QTime>
#include <QVector3D>
#include <QQuaternion>

class Camera : public QObject
{
    Q_OBJECT

public:
    enum CameraMode{
        My,
        Your,
        Her
    };
    enum GameMode{          //游戏模式，生存还是上帝
        SURVIVAL=0x3ef,
        GOD
    };

    Camera(CameraMode mode=My,QObject *parent = 0);
    Camera(const QVector3D &position,const QPointF &rotation ,CameraMode mode=My,QObject *parent = 0);

    void sightMove(const QPointF & dp);                                                                 //视角转动（其实就是鼠标移动）
    void keyPress(const int key);                                                                                 //按下键盘
    void keyRelease(const int key);                                                                                //松开按键

    void bind();                                                                                //绑定键盘与鼠标
    void unBind();                                                                          //解除键盘鼠标的绑定

    QVector3D getSightVector() const;                                                 //返还视线向量

    float getMouseLevel() const;                                            //对于鼠标灵敏度的设置与返还
    void setMouseLevel(float value);

    QVector3D position() const;                                                     //位置坐标的设置与返还
    void setPosition(const QVector3D &position);

    QVector3D getEyePosition() const;

    QPointF rotation() const;                                                         //视角向量的设置与返还
    void setRotation(const QPointF &rotation);

    static float radians(float angle);              //角度转换为弧度

    int getGameMode() const;
    void setGameMode(const int &value);

public slots:
    void cMove();                                               //对camera进行移动操作的槽，与timer的timeout信号连接

private:
    void reMotionVector();                                  //计算位移分量

private:
    int mode;
    int gameMode;
    QVector3D mPosition;                   //camera坐标
    QPointF mRotation;                  //camera眼方向(单位向量)
    bool mBind;                                   //鼠标是否处于绑定状态
    bool kBind;                                     //键盘是否处于绑定状态
    float mouseLevel;                           //鼠标灵敏度(0-1)
    float moveSpeed;                                //每毫秒的移动速度
    float dlAngle;                                      //每一个鼠标偏移方位带来的角度修正
    QVector3D udMotion;                     //前后位移分量
    QVector3D lfMotion;                         //左右位移分量
    bool keyMap[6];                                 //运动控制按键的状态（按下还是抬起）0:前 1:后 2:左 3:右 4:跳跃或起飞 5:蹲下或下降
    QTime lastTime;
    float G;                                                    //重力系数G
    float MaxSpeed;                                     //速度上限
};

#endif // CAMERA_H
