/*
 * Author:Gxin
 */
#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QtWidgets>
#include <QtOpenGL>
#include <QThread>
#include "glkernel/chunkmesh.h"
#include "glkernel/glextensions.h"
#include "glkernel/glmeshs.h"
#include "glkernel/glbuffers.h"
#include "block.h"
#include "chunkmap.h"
#include "camera.h"
#include "world.h"
#include "panels.h"

class GameScene : public QGraphicsScene
{
    Q_OBJECT

public:
    GameScene(int width, int height,QGraphicsView *parent);
    ~GameScene();
    virtual void drawBackground(QPainter *painter, const QRectF &);

    bool isInScene();                   //返还是否进入场景控制模式
    void setCenterPoint(const QPoint &cp);

    void startGame();
    void pauseGame();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void wheelEvent(QGraphicsSceneWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void setStates();               //设置状态，初始化GL绘图
    void setLights();               //光照
    void defaultStates();           //还原GL设置
    void renderWorld(const QMatrix4x4 &view,const QMatrix4x4 &rview);                                          //变换与绘制，view是位移矩阵，rview是旋转矩阵

    void firstLoad();                                                                                                                                   //第一次加载世界

signals:
    void updateWorld();                                     //从场景要求更新世界的信号
    void addBlock();
    void removeBlock();

public slots:
    void saveOption();                                      //游戏的配置读取与保存
    void dataShowPosition(const QVector3D & pos, const QVector3D &ePos);
    void showBackPackBar();                                 //显示物品栏
    void hideBackPackBar();                                 //隐藏物品栏
    void mouseMove();                                   //鼠标的移动槽（仅当进入场景）
    void loadOverSlot();                                    //预加载完毕接收槽
    void showMessage(QString message,int showTime=3, int textSize=10,QColor textColor=Qt::white);                                     //显示一个MessagePanel(含默认值)
private slots:
    void handleGameMessage();                      //处理消息等待队列

private:
    void initGame();                                                                    //初始化游戏场景
    void loadTexture();                                                 //加载材质纹理,w:材质长度，h材质高度，s材质数量

    void mouseLock();                                                   //将鼠标锁定入场景
    void mouseUnLock();

private:
    int maxRenderLen;                                                   //最大渲染距离
    QGraphicsView *GView;                                           //主窗口的指针（为获得光标的控制权）
//    GLTexture2D *blockTexture;                                  //方块材质
    GLTexture3D *blockTexture;
    QGLShader *blockVertexShader;                           //方块顶点着色器
    QGLShader *blockFragmentShader;                     //方块片段着色器
    QGLShaderProgram *blockProgram;                     //方块着色器程序

    QGLShader *lineVertexShader;
    QGLShader *lineFragmentShader;
    QGLShaderProgram *lineProgram;

    QTime lastTime;

    World *world;
    QThread *wThread;

    Camera *camera;
    bool inSence;                                                    //鼠标场景中？
    QPoint centerPoint;

    LineMesh *line;
    LineMesh *lineQua;

    DataPanel *dataPanel;
    int glFps;
    int drawCount;

    BackPackBar *backPackBar;                   //背包栏
    MessagePanel *messagePanel;
    QQueue<GameMessage *> gameMessages;
    ItemBar *itemBar;                                       //物品栏
};


#endif // GAMESCENE_H
