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
    DataPanel(int x,int y,int w,int z);
protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

public:
    void setFps(float f);                       //接收fps数据
    void setPosition(QVector3D pos);        //接收坐标数据
    void setEyePosition(QVector3D ePos);        //接收眼坐标数据

private:
    QRectF rect;
    QVector3D mPosition;                //camera坐标信息
    QVector3D mEyePosition;             //眼坐标信息
    float fps;                                          //帧数
};

#endif // PANELS_H

