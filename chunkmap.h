/*
 * 区块网络
 * 区块和显示区块在主程序中的分配是有限的。
 */
#ifndef CHUNKMAP_H
#define CHUNKMAP_H

#include <QMap>
#include <QSet>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QtOpenGL>

class Block;                                                //block的提前声明


//显示与处理区块,管理Block
class DisplayChunk
{
public:
    DisplayChunk();
    DisplayChunk(int cx,int cy,int cz);
    DisplayChunk(QVector3D dcPos);
    ~DisplayChunk();

    void resetDisplayChunk();
    void resetDisplayChunk(QVector3D dcPos);                                       //清空重置区块
    bool addBlock(Block *block,bool update);                                //增加方块
    bool removeBlock(QVector3D pos,bool update);                     //移除方块

    void update();

    bool isOk();                                                        //区块就绪与否，可以渲染否？

    Block *getBlock(QVector3D bPos);                    //返还指定坐标的方块。

    void draw();                                                            //绘制区块

    GLuint getDisplayListID() const;
    void setDisplayListID(const GLuint &value);

    static QVector3D blockPos2dcPos(QVector3D bPos);                //计算块内坐标(0-15)
    static QVector3D calcChunckPos(QVector3D bPos);                     //计算方块所属显示区块坐标
    static QVector3D calcChunkOriginPos(QVector3D cPos);            //计算区块的原点方块的坐标(cPos是显示区块坐标)

    QVector3D getDcPosition() const;

    bool getHaveChange() const;
    void setHaveChange(bool value);

    QMap<int, Block *> getBlocks() const;                               //返还方块列表

private:
    int calcKey(QVector3D bPos);                                    //通过方块坐标计算其存储的key
    void updateDisplayList();                                       //更新显示列表
    void genDisplayList();                                                  //创建显示列表(无的前提下)
    void deleteDisplayList();                                          //移除显示列表
private:
    QVector3D dcPosition;                     //显示区块坐标（x16=首区块坐标）
    QMap<int,Block*> blocks;               //方块列表16*16*16，key=0 to 4095 ( key=(16*16)*y+16*z+x. (0<=x,y,z<16) ; x,y,z=Block.(x,y,z)/16)
    int blockCount;                                 //方块计数器
    bool haveChange;                            //有修改
//    bool hasBlock;                                                   //含非空气方块？
    GLuint displayListID;                                       //分配的显示列表编号
};



///16×16×256的方块网络，由16个显示区块组成，寻找一个显示区块需要一个方块的三维坐标
//管理DisplayChunk
class ChunkMap
{
public:
    enum{
        MINLOW=0,MAXHIGHT=255
    };
    ChunkMap(QVector2D cPos);
    ChunkMap(int cx,int cz);
    ~ChunkMap();

    bool addBlock(Block *block, bool update);                                //增加方块
    bool removeBlock(QVector3D pos,bool update);                     //移除方块
    Block *getBlock(QVector3D bPos);                                                //返还指定坐标方块

    DisplayChunk *getDisplayChunk(int y);                     //返还指定 显示区块垂直高度的显示区块

    void draw(const QVector3D &pos,int maxLen);                         //绘制区块中的显示区块，pos是camera坐标,当区块到camera原点的距离小于等于maxLen时，绘制

    bool inDraw();                                                                                          //在绘制

    void update(int y);                                         //刷新第y个区块
    void updateLast();                                          //刷新上一个被操作的区块
    void updateAll();                                                   //强制刷新所有
    bool haveChange();                                          //是否对区块进行过修改
    void saveAll();                                                         //已经保存了全部，也就是设置每一个显示区块的haveChange为false
    QVector2D getChunkPosition() const;
//    void setChunkPosition(const QVector2D &value);

    void setShow(bool s);
    bool isShow();

private:
    bool createDisplayChunk(QVector3D dcPos);                                                     //创建一个显示区块

private:
    QVector2D chunkPosition;                                                    //区块坐标，世界中的区块是二维分布的.
    QMap<int,DisplayChunk*> displayChunk;                                       //16个对等的显示区块，每一个显示区块可能为空
    DisplayChunk *lastOPDC;                                                                   //上一个被操作显示区块
    bool show;
    bool drawLock;                                                                              //绘制锁，识别是否在绘制的锁
};

#endif // CHUNKMAP_H
