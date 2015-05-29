/*
 * Author:Gxin
 */
#include "gamescene.h"
#include <QTextStream>
#include <QMessageBox>
#include "ctime"
#include "gmath.h"

//选定线框的顶点偏移
QVector3D linePoints[][2]={
    {QVector3D(-0.005,1.005,-0.005),QVector3D(-0.005,1.005,1.005)},
    {QVector3D(1.005,1.005,-0.005),QVector3D(1.005,1.005,1.005)},
    {QVector3D(-0.005,1.005,-0.005),QVector3D(1.005,1.005,-0.005)},
    {QVector3D(-0.005,1.005,1.005),QVector3D(1.005,1.005,1.005)},

    {QVector3D(-0.005,-0.005,-0.005),QVector3D(-0.005,-0.005,1.005)},
    {QVector3D(1.005,-0.005,-0.005),QVector3D(1.005,-0.005,1.005)},
    {QVector3D(-0.005,-0.005,-0.005),QVector3D(1.005,-0.005,-0.005)},
    {QVector3D(-0.005,-0.005,1.005),QVector3D(1.005,-0.005,1.005)},

    {QVector3D(-0.005,1.005,-0.005),QVector3D(-0.005,-0.005,-0.005)},
    {QVector3D(-0.005,1.005,1.005),QVector3D(-0.005,-0.005,1.005)},
    {QVector3D(1.005,1.005,-0.005),QVector3D(1.005,-0.005,-0.005)},
    {QVector3D(1.005,1.005,1.005),QVector3D(1.005,-0.005,1.005)}
};

//================================================//
//
//================================================//
GameScene::GameScene(int width, int height, QGraphicsView *parent)
    :QGraphicsScene(parent)
    ,maxRenderLen(15)
    ,GView(parent)
    ,inSence(false)
{
    setSceneRect(0,0,width,height);
    initGame();
    lastTime=QTime::currentTime();

    QTimer *timer=new QTimer;
    timer->setInterval(10);
    connect(timer,SIGNAL(timeout()),this,SLOT(update()));
    connect(timer,SIGNAL(timeout()),camera,SLOT(cMove()));
    connect(timer,SIGNAL(timeout()),world,SLOT(updateDraw()),Qt::DirectConnection);         //在主线程中执行
    connect(timer,SIGNAL(timeout()),this,SLOT(mouseMove()));
    connect(timer,SIGNAL(timeout()),this,SLOT(handleGameMessage()));
    timer->start();
}

GameScene::~GameScene()
{
    saveOption();           //临时的
    world->autoSave();
    delete blockTexture;
    delete blockVertexShader;
    delete blockFragmentShader;
    delete blockProgram;

    delete lineVertexShader;
    delete lineFragmentShader;
    delete lineProgram;

    delete camera;
    wThread->quit();
    wThread->wait();
    delete wThread;

    delete line;
    delete lineQua;
    delete dataPanel;
    delete backPackBar;

    delete messagePanel;
    while(!gameMessages.isEmpty())
        gameMessages.pop_front();
}

void GameScene::drawBackground(QPainter *painter, const QRectF &)
{
    float width=float(painter->device()->width());
    float height=float(painter->device()->height());

    painter->beginNativePainting();

    setStates();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);

    qgluPerspective(60.0,width/height,0.01f,500.0f);

    glMatrixMode(GL_MODELVIEW);

    QMatrix4x4 view;
    view.translate(-camera->getEyePosition());

    QMatrix4x4 rview;
    QPointF rot=camera->rotation();
    rview.rotate(rot.x(),0,1,0);
    rview.rotate(rot.y(),cos(GMath::radians(rot.x())),0,sin(GMath::radians(rot.x())));

    renderWorld(view,rview);
    defaultStates();
    painter->endNativePainting();

    //计算fps
    drawCount++;
    QTime currT=QTime::currentTime();
    int mss=lastTime.msecsTo(currT);
    if(mss>=1000){
        glFps=(drawCount/(mss*1.0f))*1000;
        drawCount=0;
        lastTime=currT;
        dataPanel->setFps(glFps);
    }

    //控制物品栏布局
    if(backPackBar->isShow()){
        int h=this->height()*0.7;
        int w=h*1.35;
        backPackBar->setGeometry((this->width()-w)/2,(this->height()-h)/2,w,h);
        backPackBar->setViewPos(QPoint(GView->pos().x()
                                       ,GView->pos().y()+(GView->frameSize().height()-GView->height())));
    }
    if(itemBar){
        itemBar->resetSIze(this->width(),this->height(),40);
    }
}

bool GameScene::isInScene()
{
    return inSence;
}

void GameScene::setCenterPoint(const QPoint &cp)
{
    this->centerPoint=cp;
}

void GameScene::startGame()
{
    camera->setPause(false);
}

void GameScene::pauseGame()
{
    camera->setPause(true);
}


void GameScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button()==Qt::LeftButton){
        if(!backPackBar->isShow()){
            if(!inSence){
                inSence=true;
                mouseLock();
                camera->bind();
                startGame();
            }
            else{
                emit removeBlock();
            }
        }
    }
    else if(event->button()==Qt::RightButton){
        if(!backPackBar->isShow()){
            if(inSence){
                emit addBlock();
            }
        }
    }
    QGraphicsScene::mousePressEvent(event);
}

void GameScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseReleaseEvent(event);
}

void GameScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseMoveEvent(event);
}

void GameScene::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    QGraphicsScene::wheelEvent(event);
    if (!event->isAccepted()) {
        int fx= event->delta();
        if(fx<0){
            itemBar->nextIndex();
        }
        else if(fx>0){
            itemBar->lastIndex();
        }
        event->accept();
    }
}

void GameScene::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Escape){
        if(backPackBar->isShow()){
            hideBackPackBar();
            inSence=true;
            mouseLock();
            camera->bind();
            startGame();
        }
        else{
            inSence=false;
            camera->unBind();
            pauseGame();
            mouseUnLock();
        }
    }
    else if(event->key()==Qt::Key_M){
        if(camera->getGameMode()==Camera::SURVIVAL)
            camera->setGameMode(Camera::GOD);
        else
            camera->setGameMode(Camera::SURVIVAL);
    }
    else if(event->key()==Qt::Key_E){
        if(backPackBar->isShow()){
            hideBackPackBar();
            inSence=true;
            mouseLock();
            camera->bind();
            startGame();
        }
        else{
            showBackPackBar();
            inSence=false;
            camera->unBind();
            pauseGame();
            mouseUnLock();
            showMessage(tr("打开了物品栏"),1);
        }
    }
    else{
        switch (event->key()) {
        case Qt::Key_1:
            itemBar->setIndex(0);
            break;
        case Qt::Key_2:
            itemBar->setIndex(1);
            break;
        case Qt::Key_3:
            itemBar->setIndex(2);
            break;
        case Qt::Key_4:
            itemBar->setIndex(3);
            break;
        case Qt::Key_5:
            itemBar->setIndex(4);
            break;
        case Qt::Key_6:
            itemBar->setIndex(5);
            break;
        case Qt::Key_7:
            itemBar->setIndex(6);
            break;
        case Qt::Key_8:
            itemBar->setIndex(7);
            break;
        case Qt::Key_9:
            itemBar->setIndex(8);
            break;
        default:
            camera->keyPress(event->key());
            break;
        }
    }
    QGraphicsScene::keyPressEvent(event);
    //    qDebug()<<QTime::currentTime()<<"press:"<<event->key();
}

void GameScene::keyReleaseEvent(QKeyEvent *event)
{
    //    QGraphicsScene::keyReleaseEvent(event);
    camera->keyRelease(event->key());
    //    qDebug()<<QTime::currentTime()<<"relese:"<<event->key();
}

void GameScene::setStates()
{
    //    glShadeModel(GL_SMOOTH);
    glClearColor(0.0,0.65,1.0,0.5);
    //    glClearColor(0.0,0.0,0.0,0.2);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);                //启用深度测试
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

    glEnable(GL_CULL_FACE);                     //正面消隐
    glEnable(GL_LIGHTING);                          //光照
    //    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_TEXTURE_2D);                //2D材质
    glEnable(GL_NORMALIZE);                 //法线

    //        glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    //            glDepthRange(0.0f,1.0f);
    //            glClearDepth(1.0f);
    //                glDepthFunc(GL_LEQUAL);
    //                glDepthMask(GL_FALSE);
    //反锯齿
    //    glEnable(GL_BLEND);
    //    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //    glEnable(GL_POINT_SMOOTH);
    //    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    //    glEnable(GL_LINE_SMOOTH);
    //    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    //    glEnable(GL_POLYGON_SMOOTH);
    //    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    //    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_DONT_CARE);
    //    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_FASTEST);

    //png透明
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER,0.0);

    glMatrixMode(GL_PROJECTION);            //设置矩阵模式
    glPushMatrix();
    glLoadIdentity();;

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    setLights();

    float materialSpecular[]={0.5f,0.5f,0.5f,1.0f};
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,materialSpecular);           //设置有质感的光照效果
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,32.0f);
}

void GameScene::setLights()
{
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    float lightDir[]={0.0,0.0,1.0,0.0};
    float lightAmbient[]={0.2,0.2,0.2,1.0};      //环境光
    //    float lightDiffuse[]={1.0,1.0,1.0,1.0};      //漫射光
    glLightfv(GL_LIGHT0,GL_POSITION,lightDir);
    glLightfv(GL_LIGHT0,GL_AMBIENT,lightAmbient);
    //    glLightfv(GL_LIGHT0,GL_DIFFUSE,lightDiffuse);
    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER,1.0f);
    glEnable(GL_LIGHT0);
}

void GameScene::defaultStates()
{
    glClearColor(0.0,0.65,1.0,0.5);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    //glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHT0);
    glDisable(GL_NORMALIZE);
    glDisable(GL_ALPHA_TEST);

    //
    //    glDisable(GL_BLEND);
    //    glDisable(GL_POINT_SMOOTH);
    //    glDisable(GL_LINE_SMOOTH);
    //    glDisable(GL_POLYGON_SMOOTH);
    //

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 0.0f);
    float defaultMaterialSpecular[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, defaultMaterialSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
}

void GameScene::renderWorld(const QMatrix4x4 &view,const QMatrix4x4 &rview)
{
    if(glActiveTexture){
        glActiveTexture(GL_TEXTURE0);
        blockTexture->bind();
    }

    glLineWidth(2.0f);
    lineProgram->bind();
    line->draw();                           //画十字准心
    lineProgram->release();

    glLoadMatrixf(rview.constData());
    glMultMatrixf(view.constData());

    blockProgram->bind();
    blockProgram->setUniformValue("tex",GLint(0));
    world->draw();
    blockProgram->release();

    QVector3D keyPosition=camera->getKeyPosition();
    if(keyPosition.y()>=0){
        lineQua->clear();
        for(int i=0;i<12;i++){
            lineQua->addLine(linePoints[i][0]+keyPosition,linePoints[i][1]+keyPosition);
        }
        lineProgram->bind();
        lineQua->draw();
        lineProgram->release();
    }

    if(glActiveTexture){
        glActiveTexture(GL_TEXTURE0);
        blockTexture->unbind();
    }
}

void GameScene::firstLoad()
{
    emit updateWorld();
    showMessage(tr("世界正在加载中\n"
                   "请稍等"),3,10,Qt::white);
}

void GameScene::saveOption()
{
    camera->savePosRot();                   //保存camera的坐标和视角
}

void GameScene::dataShowPosition(const QVector3D &pos,const QVector3D &ePos)
{
    dataPanel->setPosition(pos,ePos);
}

void GameScene::showBackPackBar()
{
    backPackBar->show();
}

void GameScene::hideBackPackBar()
{
    backPackBar->hide();
}

void GameScene::mouseMove()
{
    if(inSence){
        QPoint dtPoint=GView->cursor().pos()-centerPoint;
        camera->sightMove(QPointF(dtPoint));
        mouseLock();
    }
}

void GameScene::loadOverSlot()
{
    showMessage(tr("世界加载完成"),2);
    if(!backPackBar->isShow()){
        if(!inSence){
            inSence=true;
            mouseLock();
            camera->bind();
            startGame();
        }
    }
}

void GameScene::showMessage(QString message, int showTime, int textSize, QColor textColor)
{
    gameMessages.push_back(new GameMessage(message,textColor,textSize,showTime));
}

void GameScene::handleGameMessage()
{
    if(messagePanel->isVisible()==false){               //messagePanel处于空闲且队列中有消息，则显示消息
        if(!gameMessages.isEmpty()){
            messagePanel->showMessage(gameMessages.front(),this);
            gameMessages.pop_front();
        }
    }
}

void GameScene::initGame()
{
    blockVertexShader=new QGLShader(QGLShader::Vertex);
    blockVertexShader->compileSourceFile(QLatin1String(":/res/divinecraft/shader/block.vsh"));
    blockFragmentShader=new QGLShader(QGLShader::Fragment);
    blockFragmentShader->compileSourceFile(QLatin1String(":/res/divinecraft/shader/block.fsh"));
    blockProgram=new QGLShaderProgram;
    blockProgram->addShader(blockVertexShader);
    blockProgram->addShader(blockFragmentShader);
    if(!blockProgram->link()){
        qWarning("Failed to compile and link shader program");
        qWarning("Vertex shader log:");
        qWarning() << blockVertexShader->log();
        qWarning() << blockFragmentShader->log();
        qWarning("Shader program log:");
        qWarning() << blockProgram->log();
        QMessageBox::warning(0,tr("错误"),tr("着色器程序加载失败造成游戏无法正常启动\n"
                                           "请联系开发者寻求解决方案"),QMessageBox::Ok);
        exit(1);
    }

    lineVertexShader=new QGLShader(QGLShader::Vertex);
    lineVertexShader->compileSourceFile(QLatin1String(":/res/divinecraft/shader/line.vsh"));
    lineFragmentShader=new QGLShader(QGLShader::Fragment);
    lineFragmentShader->compileSourceFile(QLatin1String(":/res/divinecraft/shader/line.fsh"));
    lineProgram=new QGLShaderProgram;
    lineProgram->addShader(lineVertexShader);
    lineProgram->addShader(lineFragmentShader);
    if(!lineProgram->link()){
        qWarning("Failed to compile and link shader program");
        qWarning("Vertex shader log:");
        qWarning() << lineVertexShader->log();
        qWarning() << lineFragmentShader->log();
        qWarning("Shader program log:");
        qWarning() << lineProgram->log();
        QMessageBox::warning(0,tr("错误"),tr("着色器程序加载失败造成游戏无法正常启动\n"
                                           "请联系开发者寻求解决方案"),QMessageBox::Ok);
        exit(1);
    }
    ////////////////////////////
    camera=new Camera(QVector3D(0,4,0),QPointF(180.0,0.0));
    //        camera->setMouseLevel(0.5);
    //    camera->setGameMode(Camera::GOD);

    world=new World;
    wThread=new QThread;
    world->moveToThread(wThread);
    connect(wThread,SIGNAL(finished()),world,SLOT(deleteLater()));              //线程被销毁的同时销毁world
    connect(this,SIGNAL(updateWorld()),world,SLOT(forcedUpdateWorld()));                //强制进行世界刷新
    connect(camera,SIGNAL(cameraMove(QVector3D)),world,SLOT(changeCameraPosition(QVector3D)));          //连接camera移动与世界相机位移的槽
    connect(this,SIGNAL(addBlock()),camera,SLOT(addBlock()));
    connect(this,SIGNAL(removeBlock()),camera,SLOT(removeBlock()));
    connect(world,SIGNAL(loadOver()),this,SLOT(loadOverSlot()));
    wThread->start();

    world->setMaxRenderLen(maxRenderLen);
    world->setWorldName("Test");
    camera->setWorld(world);                                //传递世界指针
    ///////////////////////////
    //这里是一个规定的加载顺序，后步骤会依赖于前步骤
    world->loadBlockIndex();            //加载方块属性列表

    loadTexture();                      //加载纹理
    //======================
    line=new LineMesh(2);           //十字准心
    float lineLen=0.0004;
    line->addLine(QVector3D(-lineLen,0,-0.02),QVector3D(lineLen,0,-0.02));
    line->addLine(QVector3D(0,-lineLen,-0.02),QVector3D(0,lineLen,-0.02));

    lineQua=new LineMesh(12);           //被选方块的包围线框

    //=======================
    //数据面板
    dataPanel=new DataPanel(0,0,200,100);
    addItem(dataPanel);
    glFps=0;
    drawCount=0;
    dataPanel->setDisplayRadius(maxRenderLen);
    connect(camera,SIGNAL(getPositions(QVector3D,QVector3D)),this,SLOT(dataShowPosition(QVector3D,QVector3D)));
    //    dataPanel->hide();
    //背包物品栏
    backPackBar=new BackPackBar(this);
    hideBackPackBar();
    backPackBar->setWorld(world);               //传递world指针

    itemBar=new ItemBar(this);
    backPackBar->setPocket(itemBar);
    connect(itemBar,SIGNAL(thingIndexChange(int)),camera,SLOT(setBlockId(int)));
    //=======================
    gameMessages.clear();
    messagePanel=new MessagePanel;
    addItem(messagePanel);
    //===========================
    camera->loadPosRot();                                   //加载位置视角信息
    firstLoad();            //强制首次加载
}

void GameScene:: loadTexture()
{
    BlockListNode *bn=world->getBlockIndex(0);
    int tw=bn->texWidth;
    int th=bn->texHeight;

    QStringList filter;
    QList<QFileInfo> files;
    QMap<QString,int>texMap;

    filter = QStringList("*.png");                  //索引材质的数量
    files = QDir(":/res/divinecraft/textures/blocks/").entryInfoList(filter, QDir::Files | QDir::Readable);

    int tc=files.length();
    //        qWarning()<<tc;
    world->setBlockListLength(tc);                                  //更新方块列表的材质数量记录
    blockTexture=new GLTexture3D(tw,th,tc+1);               //为了使材质列表的最后一张材质能够使用，需要再增加一张多余的材质来垫底
    QRgb *data=new QRgb[tw*th*(tc+1)];
    //    memset(data,0,tw*th*tc*sizeof(QRgb));
    QRgb *p=data;
    for(int k=0;k<=tc;k++){
        QImage img(files[k==tc?0:k].absoluteFilePath());
        if(k<tc){
            texMap.insert(files[k].baseName(),k);
        }
        for(int i=0;i<tw;i++){
            for(int j=0;j<th;j++){
                *p=img.pixel(j,i);
                p++;
            }
        }
    }
    blockTexture->load(tw,th,tc+1,data);
    delete [] data;

    world->calcBlockListNodeTexId(texMap);          //进行名称->ID映射
}

void GameScene::mouseLock()
{
    QPoint po(GView->pos().x()+GView->width()/2,GView->pos().y()+GView->height()/2);
    GView->cursor().setPos(po);
    setCenterPoint(po);
    //    GView->setCursor(Qt::BlankCursor);
    GView->viewport()->setCursor(Qt::BlankCursor);                  //使用父容器对鼠标进行改变
}

void GameScene::mouseUnLock()
{
    //    GView->setCursor(Qt::ArrowCursor);
    GView->viewport()->setCursor(Qt::ArrowCursor);
}

