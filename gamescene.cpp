#include "gamescene.h"
#include <QTextStream>
#include <QMessageBox>
#include "ctime"
//================================================//
//
//================================================//
float rot=0;
GameScene::GameScene(int width, int height)
{
    setSceneRect(0,0,width,height);
    initGame();
    lastTime=QTime::currentTime();
    QTimer *timer=new QTimer;
    timer->setInterval(20);
    connect(timer,SIGNAL(timeout()),this,SLOT(update()));
    timer->start();
}

GameScene::~GameScene()
{
    foreach (BlockListNode *t, mBlockList) {
        delete t;
    }
    delete blockTexture;
    delete blockVertexShader;
    delete blockFragmentShader;
    delete blockProgram;
}

void GameScene::drawBackground(QPainter *painter, const QRectF &)
{
    float width=float(painter->device()->width());
    float height=float(painter->device()->height());

    QTime lT=QTime::currentTime();

    painter->beginNativePainting();
    setStates();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    qgluPerspective(60.0,width/height,0.01,300.0);

    glMatrixMode(GL_MODELVIEW);
    QMatrix4x4 view;
    //    float angle = 00.0;
    //    QQuaternion q=QQuaternion::fromAxisAndAngle(QVector3D(1.0,0.0,0.0), angle) * QQuaternion();
    //    view.rotate(q );
    view(2, 3) -= 30.0;
    view.translate(-8,-8,0);
    renderBlocks(view);
    defaultStates();
//    qDebug()<<"draw:"<<lT.msecsTo(QTime::currentTime());
    painter->endNativePainting();
}

void GameScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mousePressEvent(event);
}

void GameScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseReleaseEvent(event);
}

void GameScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseMoveEvent(event);
    //        qDebug()<<event->scenePos();
}

void GameScene::keyPressEvent(QKeyEvent *event)
{
    QGraphicsScene::keyPressEvent(event);
}

void GameScene::setStates()
{
    glClearColor(0.0,0.65,1.0,0.5);
    glEnable(GL_DEPTH_TEST);                //启用深度测试
    glEnable(GL_CULL_FACE);                     //正面消隐
    glEnable(GL_LIGHTING);                          //光照
    glEnable(GL_TEXTURE_2D);                //2D材质
    glEnable(GL_NORMALIZE);                 //法线

    //    glClearDepth(1.0);
    //    glDepthFunc(GL_LEQUAL);
    //    glDepthRange(0.0f,1.0f);
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

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 0.0f);
    float defaultMaterialSpecular[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, defaultMaterialSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
}

void GameScene::renderBlocks(const QMatrix4x4 &view)
{
    if(glActiveTexture){
        glActiveTexture(GL_TEXTURE0);
        blockTexture->bind();
    }
    glLoadMatrixf(view.constData());


    rot+=lastTime.msecsTo(QTime::currentTime())*0.01;
    lastTime=QTime::currentTime();

    glRotatef(rot,1.0,1.0,0.5);
    blockProgram->bind();
    blockProgram->setUniformValue("tex",GLint(0));
    blockProgram->setUniformValue("view",view);

    glCallList(disChunk->getDisplayListID());
    glCallList(disChunk2->getDisplayListID());

    blockProgram->release();

    if(glActiveTexture){
        glActiveTexture(GL_TEXTURE0);
        blockTexture->unbind();
    }
}

void GameScene::initGame()
{
    loadmBlockList();
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

    block=new Block(QVector3D(0.0,1.0,0.0),mBlockList[2]);
    block2=new Block(QVector3D(0.0,2.0,0.0),mBlockList[16]);

    disChunk=new DisplayChunk(QVector3D(0,0,0));
//    disChunk->setDisplayListID(buildList);
    disChunk->addBlock(block,true);
    disChunk->addBlock(block2,true);
    qsrand(time(0));

    for(int i=0;i<16;i++)
        for(int j=0;j<16;j++){
            for(int k=0;k<16;k++){
                disChunk->addBlock(new Block(QVector3D(i,k,j),mBlockList[qrand()%34]),false);
            }
        }
    disChunk->update();

    disChunk2=new DisplayChunk(QVector3D(-1,0,0));
    for(int i=0;i>=-20;i--){
        disChunk2->addBlock(new Block(QVector3D(i,0,0),mBlockList[qrand()%34]),false);
    }
    disChunk2->update();
}

void GameScene::loadmBlockList()
{
    QFile file(":/res/divinecraft/block.list");
    if(file.open(QIODevice::ReadOnly)){
        QTextStream in(&file);
        while(!in.atEnd()){
            QString line=in.readLine();
            if(line=="" || line==NULL) continue;
            QStringList temp=line.split(" ",QString::SkipEmptyParts);
            int id=temp[0].toInt();
            int type=temp[1].toInt();
            bool collide=temp[2].toInt();
            bool trans=temp[3].toInt();
            QString name=temp[4];
            BlockListNode *node=new BlockListNode;
            node->id=id;
            node->type=type;
            node->name=name;
            node->collide=collide;
            node->trans=trans;
            mBlockList<<node;
        }
    }
    file.close();

    file.setFileName(":/res/divinecraft/textures/texture_index.list");
    if(file.open(QIODevice::ReadOnly)){
        QTextStream in(&file);
        int index=0;
        while(!in.atEnd()){
            QString line=in.readLine();
            if(line=="" || line==NULL) continue;
            if(index==0){
                QStringList temp=line.split(" ",QString::SkipEmptyParts);
                float tWidth=temp[0].toFloat();
                float tHeight=temp[1].toFloat();
                foreach (BlockListNode *node, mBlockList) {
                    node->texWidth=tWidth;
                    node->texHeight=tHeight;
                }
            }
            else{
                QStringList temp=line.split(" ",QString::SkipEmptyParts);
                int i=0;
                int id=temp[i++].toInt();
                int type=temp[i++].toInt();
                if(id>=mBlockList.length()) continue;
                BlockListNode *bl=mBlockList[id];
                while(i<temp.length()){
                    int u=temp[i++].toFloat();
                    int v=temp[i++].toFloat();
                    bl->tex<<QVector2D(u,v);
                }
                if(type==0 && i<=10){
                    bl->tex<<QVector2D(temp[temp.length()-2].toFloat(),temp[temp.length()-1].toFloat());
                }
            }
            index++;
        }
    }
    //    foreach (BlockListNode *a, mBlockList) {
    //        qDebug()<<a->id<<" "<<a->type<<" "<<a->name<<" "<<a->tex;
    //    }
}

//==========================================================================//

WorldThread::WorldThread(QObject *parent)
    :QThread(parent)
    ,isRun(false)
{
}

void WorldThread::run()
{
    isRun=true;
    count=0;
    while(isRun){
        qWarning()<<count++;
    }
}

void WorldThread::stop()
{
    isRun=false;
    this->wait();
}
