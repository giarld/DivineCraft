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
        FRONT=0,BACK=1,LEFT=2,RIGHT=3,TOP=4,DOWN=5,MAX_FACE_SUM=6
    };
    enum{
        SIXBLOCK=0x0,FOURBLOCK=0x1
    };
    Block(QVector3D position);
    Block(QVector3D position,BlockListNode const*mb);                 //创建一个默认的b_id方块。坐标为position，faceType代表6面方块或4面装饰物
    ~Block();

    bool isAir();               //是空气？
    void reSetBlock(BlockListNode const*mb, QVector3D position);                                                  //重设方块
    Face *getFace(int site);                                        //给出面

    bool isCollide();                                                       //返还可碰撞状态
    bool isTrans();                                                             //是否透明
    int faceSum();                                                          //面数
    int getShowFaceSum();                                           //可绘制的面数
    int getId();                                                                    //给出物理id（不是方块的编号）
    int getType();                                                                //给出类型

    QString getBName() const;
    void setBName(const QString &name);

    QVector3D getPosition() const;
    void setPosition(const QVector3D &value);

    QVector3D vicinityPosition(int site) const;                                //计算上下左右前后6个方向的邻近方块坐标

private:
    void createBlock();                                 //创建方块的面纹理等

private:
//    int b_id;                   //方块的所属id（属于那个方块）
    QVector3D position;         //方块坐标,左下角
    QVector<Face *> face;               //面
    QString bName;                                                                  //方块名字（实在的）

    BlockListNode const *mBlock;                                  //当前方块所拥有的物理属性
};

#endif // BLOCK_H
