#ifndef WORLD_H
#define WORLD_H

#include <QObject>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QQueue>
#include <QMap>
#include "chunkmap.h"
#include "block.h"

//世界类，管理区块的生成，保存与加载。进行方块的检索，更新。
class World : public QObject
{
    Q_OBJECT
public:
    explicit World(QObject *parent = 0);
    ~World();

    bool addBlock(Block *block, bool update);                                //增加方块
    bool removeBlock(QVector3D pos,bool update);                     //移除方块
    Block *getBlock(QVector3D bPos);                                                //返回制定坐标处的方块
    bool collision(QVector3D bPos);                                                 //实体碰撞检测

    void draw();                                                                                        //绘制

    QVector3D getCameraPosition() const;

    int getMaxRenderLen() const;
    void setMaxRenderLen(int value);

    BlockListNode *getBlockIndex(int index);

    QString getWorldName() const;
    void setWorldName(const QString &value);

signals:
    void upProgress(int p);                     //返还更新世界进度的信号

public slots:
    void updateWorld();                     //更新世界的槽,区块的加载生成和保存（会在附线程中被执行）
    void forcedUpdateWorld();           //强制更新世界（会清除chunksMap并重新加载）
    void loadBlockIndex();                  //加载方块索引
    void autoSave();                                  //自动保存？每个一个特定时钟周期进行一次保存操作
    void updateDraw();                          //处理显示更新等待队列里的请求
    void fulfilRemoveQueue();
    void changeCameraPosition(const QVector3D &cPos);

private:
    QString getKey(QVector2D chunkPos);                     //返还匹配chunksMap的键值，chunkPos是区块坐标
    QString getKey(int x,int y);
    ChunkMap *loadChunk(QVector2D chunkPos);                                         //读取并加载区块
    ChunkMap *createChunk(QVector2D chunkPos);                                                     //当区块不存在于文件中时创建区块(当前只是简单的超平坦世界)
    void bfs2World(const QVector2D &start);                                                                                            //对当前的world可视化区域进行广搜以完成区块的加载
    bool saveChunk(QString key);                                                              //保存区块修改
    void setfilePath();                                                                                                        //设置文件路径

private:
    QMap<QString,ChunkMap*> chunksMap;               //区块列表。key="chunkX-chunkY"
    int maxRenderLen;                                            //额，最大的区块显示距离，以camera所在区块为起点(最大加载距离比最大可视距离要大1)
    QVector3D cameraPosition;                                       //camera的坐标，让区块能更新的前提
    QVector2D lastCameraChunk;                                  //camera所在区块的坐标记录
    QString worldName;
    QString filePath;                                                               //地图文件所在目录地址
    QVector<BlockListNode *> mBlockIndex;                                                            //存储所有类型方块的物理属性
    QQueue<QString> updateQueue;                                    //显示更新等待队列，保存等待刷新显示的区块
    bool upLock;                                                                //update单操作锁
};

#endif // WORLD_H
