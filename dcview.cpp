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
    int maxTextureSize = 1024;
    s=new GameScene(w,h,maxTextureSize) ;
    this->setScene(s);
//    this->setMouseTracking(true);
}

DCView::~DCView()
{

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


