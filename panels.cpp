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
    QRectF fpsRect(5,0,boundingRect().width()/2,20);
    QString fpsStr="FPS : "+QString::number(fps);
    painter->drawText(fpsRect,fpsStr);

    fpsRect=QRectF(boundingRect().width()/2+5,0,boundingRect().width()/2,20);
    QString rStr=QObject::tr("Radius : %1").arg(displayRadius);
    painter->drawText(fpsRect,rStr);

    QRectF posRect(5,20,100,80);
    QString posStr=QObject::tr("pos: \nX: %1\nY: %2\nZ: %3").arg(mPosition.x()).arg(mPosition.y()).arg(mPosition.z());
    painter->drawText(posRect,posStr);

    QRectF eyeRect(105,20,100,80);
    QString eyeStr=QObject::tr("pos: \nX: %1\nY: %2\nZ: %3").arg(mEyePosition.x()).arg(mEyePosition.y()).arg(mEyePosition.z());
    painter->drawText(eyeRect,eyeStr);
}

void DataPanel::setFps(int f)
{
    this->fps=f;
}

void DataPanel::setPosition(const QVector3D &pos, const QVector3D &ePos)
{
    this->mPosition=pos;
    this->mEyePosition=ePos;
}

void DataPanel::setDisplayRadius(int r)
{
    displayRadius=r;
}

//============================
//物品栏
//============================
ItemBar::ItemBar(QGraphicsScene *scene)
    :QObject(scene)
{
    rect=QRectF(0,0,20,10);
    widgetProxy=new QGraphicsProxyWidget(0);
    widget=new ItemBarWidget();
    widgetProxy->setWidget(widget);
    widget->setGeometry(100,100,100,200);
    widgetProxy->show();
    qobject_cast<QGraphicsScene *>(parent())->addItem(widgetProxy);
}

void ItemBar::show(int x, int y, int w, int h)
{
//    widgetProxy->setVisible(true);
    rect=QRectF(x,y,w,h);
    widgetProxy->show();
}

void ItemBar::hide()
{
//    widgetProxy->setVisible(false);
    widgetProxy->hide();
}


ItemBarWidget::ItemBarWidget()
    :QWidget(0)
{
    QHBoxLayout *layout=new QHBoxLayout;
    this->setLayout(layout);
    QPushButton *button=new QPushButton(tr("button"),this);
    layout->addWidget(button);
}
