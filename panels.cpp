#include <panels.h>

//==============================
//      数据面板
//==============================
DataPanel::DataPanel(int x, int y, int w, int z)
{
    rect=QRectF(x,y,w,z);
    fps=0.0f;
    mEyePosition=mPosition=QVector3D(0,0,0);
}

QRectF DataPanel::boundingRect() const
{
    return rect;
}

void DataPanel::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QColor color=QColor(0,0,0,50);
    painter->setPen(color);
    QBrush brush=QBrush(color);
    painter->setBrush(brush);
    painter->drawRect(boundingRect());
    painter->setPen(Qt::white);
    QRectF fpsRect(5,5,boundingRect().width(),20);
    QString fpsStr="FPS : "+QString::number(fps);
    painter->drawText(fpsRect,fpsStr);
}

void DataPanel::setFps(float f)
{
    this->fps=f;
}

void DataPanel::setPosition(QVector3D pos)
{
    this->mPosition=pos;
}

void DataPanel::setEyePosition(QVector3D ePos)
{
    this->mEyePosition=ePos;
}
