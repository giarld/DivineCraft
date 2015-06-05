/*
 * Author:Gxin
 * main
 */
#include "dcview.h"
#include "glkernel/glextensions.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    DCView view;
    view.setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    view.show();

    return app.exec();
}
