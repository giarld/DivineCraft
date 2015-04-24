/*
 * 区块网络
 * 区块和显示区块在主程序中的分配是有限的。
 */
#ifndef CHUNKMAP_H
#define CHUNKMAP_H

#include <QMap>
#include <QSet>
#include <QVector>
#include <QVector3D>
#include <QtOpenGL>

class Block;                                                //block的提前声明


//显示与处理区块
class DisplayChunk
{
public:
    DisplayChunk();
    DisplayChunk(QVector3D dcPos);
    ~DisplayChunk();

    void resetDisplayChunk();
    void resetDisplayChunk(QVector3D dcPos);                                       //清空重置区块
    bool addBlock(Block *block,bool update);                                //增加方块
    bool removeBlock(QVector3D pos);                     //移除方块,只是单纯的从显示区块移除，Block的其他相关关系再主场景处理

    void update();

    bool isOk();                                                        //区块就绪与否，可以渲染否？

    Block *getBlock(QVector3D bPos);                    //返还指定坐标的方块。

    GLuint getDisplayListID() const;
    void setDisplayListID(const GLuint &value);

    static QVector3D blockPos2dcPos(QVector3D bPos);                //计算块内坐标,全局
    static QVector3D calcChunckPos(QVector3D bPos);                     //计算方块所属区块坐标

private:
    int calcKey(QVector3D bPos);                                    //通过方块坐标计算其存储的key
    void updateDisplayList();                                       //更新显示列表
    void genDisplayList();                                                  //创建显示列表(无的前提下)
    void deleteDisplayList();                                          //移除显示列表
private:
    QVector3D dcPosition;                     //显示区块坐标（x16=首区块坐标）
    QMap<int,Block*> blocks;               //方块列表16*16*16，key=0 to 4095 ( key=(16*16)*y+16*z+x. (0<=x,y,z<16) ; x,y,z=Block.(x,y,z)/16)
    int blockCount;                                 //方块计数器
//    bool hasBlock;                                                   //含非空气方块？
    GLuint displayListID;                                       //分配的显示列表编号
};



///16×16×256的方块网络，由16个显示区块组成，寻找一个显示区块需要一个方块的三维坐标
class ChunkMap
{
public:
    ChunkMap();
    ~ChunkMap();
private:
    QVector2D chuckPosition;                                                    //区块坐标，世界中的区块是二维分布的。方块坐标/16可以找到对应区块
    QMap<int,DisplayChunk*> displayChunk;                                       //16个对等的显示区块，每一个显示区块可能为空
};

#endif // CHUNKMAP_H
