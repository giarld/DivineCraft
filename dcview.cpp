/*
 * Author:Gxin
 */
#include "dcview.h"
#include <QApplication>

DCView::DCView()
{
    this->setWindowTitle(tr("DivineCraft"));
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    int dWidth=QApplication::desktop()->width();
    int dHeight=QApplication::desktop()->height();
    int w=dWidth*0.618;
    int h=dHeight*0.618;
    this->setGeometry((dWidth-w)/2,(dHeight-h)/2,w,h);              //窗口居中，黄金比例。
    gameScene=new GameScene(w,h) ;
    this->setScene(gameScene);
    //    this->setMouseTracking(true);
}

DCView::~DCView()
{
    if(gameScene)
        delete gameScene;
}

void DCView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
    if(event->button()==Qt::LeftButton){
        if(gameScene->isInScene()){
            this->cursor().setPos(this->pos().x()+this->width()/2,this->pos().y()+this->height()/2);
            gameScene->setCenterPoint(QPointF(this->pos().x()+this->width()/2,this->pos().y()+this->height()/2));
            this->setCursor(Qt::BlankCursor);
        }
    }
}

void DCView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
    if(gameScene->isInScene()){
        this->cursor().setPos(this->pos().x()+this->width()/2,this->pos().y()+this->height()/2);
        gameScene->setCenterPoint(QPointF(this->pos().x()+this->width()/2,this->pos().y()+this->height()/2));
    }
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
    else if(event->key()==Qt::Key_Escape){
        if(gameScene->isInScene()){
            this->setCursor(Qt::ArrowCursor);
        }
    }
    else if(event->key()==Qt::Key_Q)
        exit(1);
    QGraphicsView::keyPressEvent(event);
}


