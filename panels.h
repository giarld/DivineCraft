/*
 * 各种服务于场景的面板
 */
#ifndef PANELS_H
#define PANELS_H

#include <QtWidgets>
#include <QGraphicsItem>

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

//===============================================//
class ItemBarWidget : public QWidget
{
    Q_OBJECT
public:
    ItemBarWidget();
};

class ItemBar : public QObject
{
    Q_OBJECT
public:
    ItemBar(QGraphicsScene *scene);

    void show();
    void hide();
protected:

private:
    QRectF rect;
    QGraphicsProxyWidget *widgetProxy;
};

#endif // PANELS_H

