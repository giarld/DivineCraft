#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QtWidgets>
#include <QtOpenGL>
#include "glkernel/chunkmesh.h"
#include "glkernel/glextensions.h"
#include "glkernel/glmeshs.h"
#include "glkernel/glbuffers.h"
#include "block.h"

#define PI 3.14159265358979


class GameScene : public QGraphicsScene
{
    Q_OBJECT

public:
    GameScene(int width,int height,int maxTextureSize);
    ~GameScene();
    virtual void drawBackground(QPainter *painter, const QRectF &);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

    void setStates();               //设置状态，初始化GL绘图
    void setLights();               //光照
    void defaultStates();           //还原GL设置
    void renderBlocks(const QMatrix4x4 &view);                                          //绘制

signals:

private:
    void initGame();
    void loadmBlockList();
    void makeBuildList();

private:
    int g_maxTextureSize;
    GLTexture2D *blockTexture;                                  //方块材质
    QGLShader *blockVertexShader;                           //方块顶点着色器
    QGLShader *blockFragmentShader;                     //方块片段着色器
    QGLShaderProgram *blockProgram;                     //方块着色器程序
    Block *block;
    Block *block2;

    GLuint buildList;

    QVector<BlockListNode *> mBlockList;                                                            //存储所有类型方块的物理属性
};

class WorldThread : public QThread                                                          //负责加载、保存、重建地图和区块的线程
{
    Q_OBJECT

public:
    WorldThread(QObject *parent=0);
};

#endif // GAMESCENE_H