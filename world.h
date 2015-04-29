#ifndef WORLD_H
#define WORLD_H

#include <QObject>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
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

    void draw();                                                                                        //绘制

    QVector3D getCameraPosition() const;
    void setCameraPosition(const QVector3D &value);

    int getMaxRenderLen() const;
    void setMaxRenderLen(int value);

signals:
    void upProgress(int p);                     //返还更新世界进度的信号
public slots:
    void updateWorld();                     //更新世界的槽,区块的加载生成（会在附线程中被执行）
    void saveChunk();                           //保存区块修改

private:
    QString getKey(QVector2D chunkPos);                     //返还匹配chunksMap的键值，chunkPos是区块坐标
    QString getKey(int x,int y);

private:
    QMap<QString,ChunkMap*> chunksMap;               //区块列表。key="chunkX-chunkY"
    int maxRenderLen;                                            //额，最大的区块显示距离，以camera所在区块为起点
    QVector3D cameraPosition;                                       //camera的坐标，让区块能更新的前提
};

#endif // WORLD_H
