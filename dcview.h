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
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
     GameScene *s;
};

#endif // DCVIEW_H
