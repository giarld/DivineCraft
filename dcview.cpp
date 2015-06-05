/*
 * Author:Gxin
 */
#include "dcview.h"
#include <QApplication>


inline bool matchString(const char *extensionString, const char *subString)
{
    int subStringLength = strlen(subString);
    return (strncmp(extensionString, subString, subStringLength) == 0)
            && ((extensionString[subStringLength] == ' ') || (extensionString[subStringLength] == '\0'));
}

bool necessaryExtensionsSupported()
{
    const char *extensionString = reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS));
    const char *p = extensionString;

    const int GL_EXT_FBO = 1;
    const int GL_ARB_VS = 2;
    const int GL_ARB_FS = 4;
    const int GL_ARB_SO = 8;
    int extensions = 0;

    while (*p) {
        if (matchString(p, "GL_EXT_framebuffer_object"))
            extensions |= GL_EXT_FBO;
        else if (matchString(p, "GL_ARB_vertex_shader"))
            extensions |= GL_ARB_VS;
        else if (matchString(p, "GL_ARB_fragment_shader"))
            extensions |= GL_ARB_FS;
        else if (matchString(p, "GL_ARB_shader_objects"))
            extensions |= GL_ARB_SO;
        while ((*p != ' ') && (*p != '\0'))
            ++p;
        if (*p == ' ')
            ++p;
    }
    return (extensions == 15);
}

//////////////////////////////////////

DCView::DCView()
{
    //OpenGL支持检测
    if((QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_5)==0){
        QMessageBox::critical(0, "OpenGL features missing",
                              "OpenGL version 1.5 or higher is required to run this game.\n"
                              "The program will now exit.");
        exit(-1);
    }

    widget = new QGLWidget(QGLFormat(QGL::SampleBuffers));
    widget->makeCurrent();

    if (!necessaryExtensionsSupported()) {
        QMessageBox::critical(0, "OpenGL features missing",
                              "The OpenGL extensions required to run this game are missing.\n"
                              "The program will now exit.");
        delete widget;
        exit(-2);
    }

    // 检查所有需要的功能
    if (!getGLExtensionFunctions().resolve(widget->context())) {
        QMessageBox::critical(0, "OpenGL features missing",
                              "Failed to resolve OpenGL functions required to run this game.\n"
                              "The program will now exit.");
        delete widget;
        exit(-3);
    }

    widget->makeCurrent();
    this->setViewport(widget);
    ///////////////////////////////////////////////////

    QApplication::setApplicationName("DivineCraft");
    QApplication::setApplicationVersion("0.0010.0(demo_1)");
    this->setWindowTitle(tr("DivineCraft %1").arg(QApplication::applicationVersion()));
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    this->setWindowIcon(QIcon(":/res/divinecraft/image/logo_128x128.png"));

    int dWidth=QApplication::desktop()->width();
    int dHeight=QApplication::desktop()->height();
    int w=dWidth*0.618;
    int h=dHeight*0.618;
    this->setMinimumSize(w,h);
    this->setGeometry((dWidth-w)/2,(dHeight-h)/2,w,h);              //窗口居中，黄金比例。
    gameScene=new GameScene(w,h,this) ;
    this->setScene(gameScene);
    //        this->setMouseTracking(false);
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

