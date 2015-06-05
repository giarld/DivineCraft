/*
 * Author:Gxin
 */
#ifndef DCVIEW_H
#define DCVIEW_H
#include <QtWidgets>
#include <QGLWidget>
#include <QWidget>
#include "gamescene.h"

class DCView : public QGraphicsView
{
    Q_OBJECT

public:
    DCView();
    ~DCView();
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);


private:
    QGLWidget *widget;
    GameScene *gameScene;
};

#endif // DCVIEW_H
