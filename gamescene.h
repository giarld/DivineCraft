#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QtWidgets>
#include <QtOpenGL>
#include "glkernel/chunkmesh.h"
#include "glkernel/glextensions.h"
#include "glkernel/glmeshs.h"
#include "glkernel/glbuffers.h"
#include "block.h"
#include "chunkmap.h"
#include "camera.h"

#define PI 3.14159265358979

class WorldThread;

class GameScene : public QGraphicsScene
{
    Q_OBJECT

public:
    GameScene(int width, int height);
    ~GameScene();
    virtual void drawBackground(QPainter *painter, const QRectF &);

    bool isInScene();                   //返还是否进入场景控制模式
    void setCenterPoint(const QPointF &cp);
protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void setStates();               //设置状态，初始化GL绘图
    void setLights();               //光照
    void defaultStates();           //还原GL设置
    void renderBlocks(const QMatrix4x4 &view,const QMatrix4x4 &rview);                                          //绘制

signals:

public slots:
    void mouseMove(const QPointF &dp);                                  //鼠标发生移动的槽，用于主窗口给予鼠标偏移量

private:
    void initGame();                                                                    //初始化游戏场景
    void loadmBlockList();                                                          //加载方块列表和纹理描述文件

private:
    int maxRenderLen;                                                   //最大渲染距离
    GLTexture2D *blockTexture;                                  //方块材质
    QGLShader *blockVertexShader;                           //方块顶点着色器
    QGLShader *blockFragmentShader;                     //方块片段着色器
    QGLShaderProgram *blockProgram;                     //方块着色器程序
    QTime lastTime;
    DisplayChunk *disChunk;
    ChunkMap *chunk1;

    Camera *camera;
    bool inSence;                                                    //鼠标场景中？
    QPointF centerPoint;

    QVector<BlockListNode *> mBlockList;                                                            //存储所有类型方块的物理属性
};

//--负责加载、保存、重建地图和区块的线程--//
class WorldThread : public QThread
{
    Q_OBJECT

public:
    WorldThread(QObject *parent=0);

protected:
    virtual void run();

public slots:
    void stop();

private:
    bool isRun;
    int count;
};

#endif // GAMESCENE_H
