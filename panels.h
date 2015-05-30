/*
 * 各种服务于场景的面板
 */
#ifndef PANELS_H
#define PANELS_H

#include <QtWidgets>
#include <QGraphicsItem>
#include <QVector>
#include "block.h"

class World;                //world类的引用

class DataPanel : public QGraphicsItem
{
public:
    DataPanel(int x,int y,int w,int h);
protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

public:
    void setFps(int f);                       //接收fps数据
    void setPosition(const QVector3D &pos,const QVector3D &ePos);        //接收坐标数据
    void setDisplayRadius(int r);

private:
    QRectF rect;
    QVector3D mPosition;                //camera坐标信息
    QVector3D mEyePosition;             //眼坐标信息
    int fps;                                          //帧数
    int displayRadius;                      //绘制半径信息
};
//=================================================//
//消息组件
//游戏提示消息结构
struct GameMessage{
public:
    GameMessage(QString message, QColor textColor, int textSize,  int showTime){
        text=message;
        this->textSize=textSize;
        this->textColor=textColor;
        this->showTime=showTime;
    }
    QString text;
    int textSize;
    QColor textColor;
    int showTime;
};
//提示消息面板
class MessagePanel : public QGraphicsItem
{
public:
    MessagePanel();
    //显示提示面板，坐标，长宽，信息内容，字体颜色，字体大小，以及显示的时间（showTime>0）
    void showMessage(GameMessage *message,QGraphicsScene *s);
protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
private:
    void deleteMessage();           //清理消息内存
private :
    GameMessage *gMessage;
    QRectF rect;
    int alpha;                      //透明度
    QTime beginTime;
    QFont font;
};

//===============================================//
//物品单元结构
struct ThingItem
{
public:
    ThingItem(int id,QString name,QString texName,int amount){                  //方块id,方块名称，材质文件名，数量
        setItem(id,name,texName,amount);
    }
    void setItem(int id,QString name,QString texName,int amount){
        this->id=id;
        this->name=name;
        this->texName=texName;
        this->amount=amount;
    }

    int id;
    QString name;
    QString texName;
    int amount;
};

class ThingItemPanel : public QWidget
{
    Q_OBJECT
public:
    ThingItemPanel(int size,QWidget *parent);
    ~ThingItemPanel();
    void setItem(int id,QString table,QString texName,int amount);              //设置item属性
    void setItem(ThingItem *i);
    void copyItem(ThingItem *i);                            //复制Item，而不是传递指针
    void setNULLItem();                                         //设置为空气物品(即空的)
    void setAmount(int s);                                  //设置数量(-1<=amount<=64,当等于-1时,为无限拾取，当为0时会被自动隐藏)
    int getAmount();
    ThingItem *getItem();
    void setSize(int size);

    bool isNULL();                                      //是否为空
    bool isPick();                                          //能否被拾取（amount>0）
protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *event);

signals:
    void mouseChoose();

private:
    int tSize;
    ThingItem *item;
};

//======================================
//物品栏组件
class ItemBarWidget : public QWidget
{
    Q_OBJECT
public:
    ItemBarWidget();
    ~ItemBarWidget();

    void setThingItem(ThingItem *t,int index);                      //设置index位置上的物品为t
    void setThingItem(int id, QString table, QString texName, int amount, int index);
    ThingItem *currThingItem();                                             //返还当前位子上的物品
    int getCurrThingID();                                                           //返还当前所持物品ID
    ThingItem *getThingItem(int i);                                         //返还i位置上的ThingItem
    void setIndex(int i);                                               //设置物品选择位置
    void nextIndex();                                                   //下一个选择位置
    void lastIndex();                                                   //上一个选择位置

    void midBlock(BlockListNode *blockIndex);                   //中间拾取方块
protected:
    void paintEvent(QPaintEvent *);

signals:
    void thingIndexChange(int blockId);

private:
    QVector<ThingItemPanel *>pocketThing;               //物品
    int currentIndex;                                                           //当前选中的位置
};

//物品栏
class ItemBar : public QObject
{
    Q_OBJECT
public:
    ItemBar(QGraphicsScene *scene);
    ThingItem *getThingItem(int i);                                         //返还i位置上的ThingItem
    void setThingItem(int id, QString table, QString texName, int amount, int index);
    void resetSIze(int sceneW,int sceneH,int h);                //设置物品栏大小，sceneW是场景宽，sceneH是场景高，h是物品栏高度
    int getCurrThingID();                                                           //返还当前所持物品ID

    void setIndex(int i);                                               //设置物品选择位置
    void nextIndex();                                                   //下一个选择位置
    void lastIndex();                                                   //上一个选择位置

    void midBlock(BlockListNode *blockIndex);                   //中间拾取方块
signals:
    void thingIndexChange(int blockId);                 //传递物品选择变换，传递的是方块id

private:
    ItemBarWidget *widget;
    QGraphicsProxyWidget *proxyWidget;
};

//=======================================
class BackPackBarWidget : public QWidget
{
    Q_OBJECT
public:
    BackPackBarWidget();
    ~BackPackBarWidget();
    void setWorld(World *world);                            //引入世界指针
    void setPocket(ItemBar *itemBar);                           //使用该函数设置物品栏物品

    void setPocketThingItem(ThingItem *item, int amount, int index);
    void setViewPos(QPoint pos);

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *event);

private:
    void initBar();                         //初始化物品栏

private slots:
    void chooseItem();

private:
    QVector<ThingItemPanel*>pocketThing;              //口袋物品  !!其中ThingItem的指针要求保持不变。
    QVector<ThingItemPanel*>barThing;                    //背包栏里的物品
    ThingItemPanel *flowItem;                                       //漂浮的Item
    ItemBar *pocketBar;
    World *myWorld;
    QPoint viewPos;                                                         //全局窗体的坐标
};

class BackPackBar : public QObject
{
    Q_OBJECT
public:
    BackPackBar(QGraphicsScene *scene);
    ~BackPackBar();

    void show();
    void hide();
    void setGeometry(int x,int y,int w,int h);
    void setViewPos(QPoint pos);
    bool isShow();           //是否呈现
    void setWorld(World *world);
    void setPocket(ItemBar *itemBar);
protected:

private:
    QRect rect;
    QGraphicsProxyWidget *widgetProxy;
    BackPackBarWidget *widget;
};


#endif // PANELS_H

