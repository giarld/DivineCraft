/*
 * 各种服务于场景的面板
 */
#ifndef PANELS_H
#define PANELS_H

#include <QtWidgets>
#include <QGraphicsItem>
#include <QVector>

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
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
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
    void setAmount(int s);                                  //设置数量

    void setSize(int size);
protected:
    void paintEvent(QPaintEvent *);
private:
    int tSize;
    ThingItem *item;
};

class BackPackBarWidget : public QWidget
{
    Q_OBJECT
public:
    BackPackBarWidget();
    ~BackPackBarWidget();
    void setWorld(World *world);

protected:
    void paintEvent(QPaintEvent *);

private:
    void initBar();                         //初始化物品栏

private:
    QVector<ThingItemPanel*>pocketThing;              //口袋物品
    QVector<ThingItemPanel*>barThing;                    //背包栏里的物品
    World *myWorld;
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
    bool isShow();           //是否呈现
    void setWorld(World *world);
protected:

private:
    QRect rect;         
    QGraphicsProxyWidget *widgetProxy;
    BackPackBarWidget *widget;
};
//======================================
//物品栏
class ItemBar : public QObject
{
    Q_OBJECT
public:
    ItemBar(QGraphicsScene *scene);
}

#endif // PANELS_H

