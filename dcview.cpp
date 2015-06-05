/*
 * Author:Gxin
 */
#include "dcview.h"
#include <QApplication>

DCView::DCView()
{
    this->setWindowTitle(tr("DivineCraft"));
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    this->setWindowIcon(QIcon(":/res/divinecraft/icon.png"));

    int dWidth=QApplication::desktop()->width();
    int dHeight=QApplication::desktop()->height();
    int w=dWidth*0.618;
    int h=dHeight*0.618;
    this->setMinimumSize(w,h);
    this->setGeometry((dWidth-w)/2,(dHeight-h)/2,w,h);              //窗口居中，黄金比例。
    gameScene=new GameScene(w,h,this) ;
    this->setScene(gameScene);
        this->setMouseTracking(false);
}

DCView::~DCView()
{
    if(gameScene)
        delete gameScene;
}

void DCView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
}

void DCView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
}

void DCView::resizeEvent(QResizeEvent *event)
{
    if(scene()){
        scene()->setSceneRect(QRect(QPoint(0,0),event->size()));
    }
    QGraphicsView::resizeEvent(event);
}

void DCView::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_F11){
        if(this->isFullScreen())
            this->showNormal();
        else
            this->showFullScreen();
    }

    QGraphicsView::keyPressEvent(event);
}


