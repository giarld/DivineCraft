#include "gamescene.h"
#include <QTextStream>
#include <QMessageBox>
#include "ctime"
#include "gmath.h"
//================================================//
//
//================================================//
GameScene::GameScene(int width, int height)
    :maxRenderLen(5)
    ,inSence(false)
{
    setSceneRect(0,0,width,height);
    initGame();
    lastTime=QTime::currentTime();

    QTimer *timer=new QTimer;
    timer->setInterval(20);
    connect(timer,SIGNAL(timeout()),this,SLOT(update()));
    connect(timer,SIGNAL(timeout()),camera,SLOT(cMove()));
    connect(timer,SIGNAL(timeout()),world,SLOT(updateDraw()),Qt::DirectConnection);         //在主线程中执行
    timer->start();
}

GameScene::~GameScene()
{
    delete blockTexture;
    delete blockVertexShader;
    delete blockFragmentShader;
    delete blockProgram;
    delete camera;
    wThread->quit();
    wThread->wait();
    delete wThread;
}

void GameScene::drawBackground(QPainter *painter, const QRectF &)
{
    float width=float(painter->device()->width());
    float height=float(painter->device()->height());

    //        QTime lT=QTime::currentTime();

    painter->beginNativePainting();

    setStates();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    qgluPerspective(60.0,width/height,0.01,500.0);

    glMatrixMode(GL_MODELVIEW);

    QMatrix4x4 view;
    view.translate(-camera->getEyePosition());

    QMatrix4x4 rview;
    QPointF rot=camera->rotation();
    rview.rotate(rot.x(),0,1,0);
    rview.rotate(rot.y(),cos(GMath::radians(rot.x())),0,sin(GMath::radians(rot.x())));

    renderWorld(view,rview);
    defaultStates();
    //        qDebug()<<"draw:"<<lT.msecsTo(QTime::currentTime());
    painter->endNativePainting();
}

bool GameScene::isInScene()
{
    return inSence;
}

void GameScene::setCenterPoint(const QPointF &cp)
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
    QGraphicsScene::mousePressEvent(event);
    if(event->button()==Qt::LeftButton){
        if(!inSence){
            inSence=true;
            camera->bind();
            startGame();
        }
    }
}

void GameScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseReleaseEvent(event);
}

void GameScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseMoveEvent(event);
    //            qDebug()<<event->scenePos();
    if(inSence){
        camera->sightMove(event->screenPos()-centerPoint);
    }
}

void GameScene::keyPressEvent(QKeyEvent *event)
{
    QGraphicsScene::keyPressEvent(event);
    if(event->key()==Qt::Key_Escape && inSence){
        inSence=false;
        camera->unBind();
        pauseGame();
    }
    else if(event->key()==Qt::Key_M){
        if(camera->getGameMode()==Camera::SURVIVAL)
            camera->setGameMode(Camera::GOD);
        else
            camera->setGameMode(Camera::SURVIVAL);
    }
    else{
        camera->keyPress(event->key());
    }
    //    qDebug()<<QTime::currentTime()<<"press:"<<event->key();
}

void GameScene::keyReleaseEvent(QKeyEvent *event)
{
    QGraphicsScene::keyReleaseEvent(event);
    camera->keyRelease(event->key());
    //    qDebug()<<QTime::currentTime()<<"relese:"<<event->key();
}

void GameScene::setStates()
{
    glClearColor(0.0,0.65,1.0,0.5);

    glEnable(GL_DEPTH_TEST);                //启用深度测试
    glEnable(GL_CULL_FACE);                     //正面消隐
    glEnable(GL_LIGHTING);                          //光照
    //    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_TEXTURE_2D);                //2D材质
    glEnable(GL_NORMALIZE);                 //法线

    //        glDepthRange(0.0f,1.0f);
    //        glClearDepth(1.0f);
    //            glDepthFunc(GL_LEQUAL);
    //            glDepthMask(GL_FALSE);
    //反锯齿
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glEnable(GL_POINT_SMOOTH);
//    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
//    glEnable(GL_LINE_SMOOTH);
//    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
//    glEnable(GL_POLYGON_SMOOTH);
//    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

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

    line->draw();
    glLoadMatrixf(rview.constData());
    glMultMatrixf(view.constData());
    //    glRotatef(rot,1.0,1.0,1.0);

    blockProgram->bind();
    blockProgram->setUniformValue("tex",GLint(0));
    //    blockProgram->setUniformValue("view",view);
    world->draw();

    blockProgram->release();

    if(glActiveTexture){
        glActiveTexture(GL_TEXTURE0);
        blockTexture->unbind();
    }
}

void GameScene::firstLoad()
{
    emit updateWorld();
}

void GameScene::initGame()
{
    camera=new Camera(QVector3D(8,4,8),QPointF(180.0,0.0));
    //    camera->setMouseLevel(0.5);
    camera->setGameMode(Camera::GOD);

    blockTexture=new GLTexture2D(":/res/divinecraft/textures/block_texture.png",0,0);

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
    ////////////////////////////
    world=new World;
    wThread=new QThread;
    world->moveToThread(wThread);
    connect(wThread,SIGNAL(finished()),world,SLOT(deleteLater()));              //线程被销毁的同时销毁world
    connect(this,SIGNAL(updateWorld()),world,SLOT(forcedUpdateWorld()));
    connect(camera,SIGNAL(cameraMove(QVector3D)),world,SLOT(changeCameraPosition(QVector3D)));          //连接camera移动与世界相机位移的槽
    wThread->start();
    world->loadBlockIndex();
    world->setMaxRenderLen(maxRenderLen);
    world->setWorldName("Test");
    camera->setWorld(world);                                //传递世界指针
    ///////////////////////////

    //    world->setCameraPosition(camera->position());
    //    world->updateWorld();
    firstLoad();            //强制首次加载

    line=new LineMesh(2);
    float lineLen=0.0004;
    line->addPoint(QVector3D(-lineLen,0,-0.02),QVector3D(lineLen,0,-0.02));
    line->addPoint(QVector3D(0,-lineLen,-0.02),QVector3D(0,lineLen,-0.02));
}

