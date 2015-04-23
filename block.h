#ifndef BLOCK_H
#define BLOCK_H

#include <QVector2D>
#include <QVector3D>
#include <QVector>
#include <QMap>
#include <QtGui>

//方块材质及属性列表节点
struct BlockListNode{
    int id;                                             //物理id
    int type;                                           //物理类型
    bool collide;                                   //是实体碰撞方块？
    bool trans;                                         //是否透明
    QString name;                                //方块名称
    QVector<QVector2D> tex;             //材质坐标列表(起点)
    float texWidth,texHeight;      //纹理的逻辑宽和高(texture_index.list第一行)
};
//////////////////////////////////

//顶点
struct Vertex{
    QVector3D position;
    QVector2D texCoord;
    QVector3D normal;
};

//面,由顶点组成
struct Face
{
//    int site;                   //哪个面，详见Block的Face Enum
    bool ishide;                    //是否隐藏
    Vertex vert[4];             //四个顶点，以逆时针排序，正对着观察从左上角开始。
};

/*方块，由面组成。标准方块有6个面：FRONT,BACK,TOP,DOWN,LEFT,RIGHT。
 * 花卉一类的方块有4个面：FRONT,BACK,TOP,DOWN，且成对角绘制
*/

class Block
{
public:
    enum{//face Enum
        FRONT=0,BACK=1,LEFT=2,RIGHT=3,TOP=4,DOWN=5
    };
    enum{
        SIXBLOCK=0x0,FOURBLOCK=0x1
    };
    Block(QVector3D position);
    Block(BlockListNode const*mb, QVector3D position);                 //创建一个默认的b_id方块。坐标为position，faceType代表6面方块或4面装饰物
    ~Block();

    void reSetBlock(BlockListNode const*mb, QVector3D position);                                                  //重设方块
    Face *getFace(int site);                                        //给出面
    bool setBrother(int site,Block *b);                     //设置兄弟
    bool removeBrother(int site);                           //移除兄弟记录
    void updateFace();                                                          //更新面的显示，可再增加兄弟和去除兄弟后执行。
    void hideFace(int site,bool hide=true);                      //设置面site的隐藏状态，hide为是否隐藏
    void hideAll();                                                                 //隐藏所有
    void showAll();                                                                     //显示所有
    void showFace(int site);                                                    //显示面，功能同hideFace(site,false);
    bool isFaceHide(int site);                                                      //面site是否隐藏
    bool isCollide();                                                       //返还可碰撞状态
    bool isTrans();                                                             //是否透明
    int faceSum();                                                          //面数
    int getShowFaceSum();                                           //可绘制的面数
    int getId();                                                                    //给出物理id（不是方块的编号）
    int getType();                                                                //给出类型

private:
    void createBlock();                                 //创建方块的面纹理等

private:
//    int b_id;                   //方块的所属id（属于那个方块）
    QVector3D position;         //方块坐标,左下角
    QVector<Face *> face;               //面

    BlockListNode const *mBlock;                                  //当前方块所拥有的物理属性
    QVector<Block *>brothers;                               //兄弟方块，周围6个
};

#endif // BLOCK_H
