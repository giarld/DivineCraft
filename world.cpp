/*
 * Author:Gxin
 */
#include "world.h"
#include <cmath>
#include <QDataStream>
#include <QTextStream>
#include <QDir>
#include <QFile>
#include "gmath.h"
#include <ctime>

World::World(QObject *parent) : QObject(parent)
{
    setMaxRenderLen(16);
    this->lastCameraChunk=QVector2D(0,0);
    upLock=false;
    qsrand(time(0));            //初始化随机种子
}

World::~World()
{
    foreach (BlockListNode *t, mBlockIndex) {
        if(t)
            delete t;
    }
    foreach (ChunkMap *c, chunksMap) {
        if(c)
            delete c;
    }
    chunksMap.clear();
}

bool World::addBlock(Block *block, bool update)
{
    QVector3D dPos=DisplayChunk::calcChunckPos(block->getPosition());       //方块所属区块
    QVector3D cdPos=DisplayChunk::calcChunckPos(getCameraPosition());       //camera所在区块
    if(GMath::gAbs(int(dPos.distanceToPoint(cdPos)))>maxRenderLen)           //方块超出world的管理范围
        return false;
    if(block->getPosition().y()<ChunkMap::MINLOW  || block->getPosition().y()>ChunkMap::MAXHIGHT){                 //方块超出上下限，pass
        return false;
    }
    QString sKey=getKey(GMath::v3d2v2d(dPos));
    ChunkMap *tempc=chunksMap.value(sKey);
    if(!tempc){                                 //区块错误返还false
        return false;
    }
    return tempc->addBlock(block,update);
}

bool World::removeBlock(QVector3D pos, bool update)
{
    QVector3D dPos=DisplayChunk::calcChunckPos(pos);       //方块所属区块
    QVector3D cdPos=DisplayChunk::calcChunckPos(getCameraPosition());       //camera所在区块
    if(GMath::gAbs(int(dPos.distanceToPoint(cdPos)))>maxRenderLen)           //方块超出world的管理范围
        return false;
    if(pos.y()<ChunkMap::MINLOW  || pos.y()>ChunkMap::MAXHIGHT){                 //方块超出上下限，pass
        return false;
    }
    QString sKey=getKey(GMath::v3d2v2d(dPos));
    ChunkMap *tempc=chunksMap.value(sKey);
    if(!tempc){                                 //区块错误返还false
        return false;
    }
    return tempc->removeBlock(pos,update);
}

Block *World::getBlock(QVector3D bPos)
{
    QVector3D dp=DisplayChunk::calcChunckPos(bPos);

    ChunkMap *chunk=chunksMap.value(getKey(GMath::v3d2v2d(dp)));
    if(chunk==NULL)                                                 //空区块返还空方块
        return NULL;
    return chunk->getBlock(bPos);                       //向下查找方块
}

bool World::collision(QVector3D bPos)
{
    Block *tb=getBlock(GMath::v3toInt(bPos));           //先将坐标取整再搜索方块
    if(tb==NULL)
        return false;
    if(tb->isAir())
        return false;                   //空气没有碰撞箱
    return tb->isCollide();
}

void World::draw()
{
    foreach (ChunkMap *cm, chunksMap) {
        if(cm){
            cm->draw(this->cameraPosition,this->maxRenderLen);
        }
    }
}

void World::updateWorld()
{
    if(upLock)
        return;
    upLock=true;
    qDebug()<<QTime::currentTime()<<"update world";
    QVector3D cdPos=DisplayChunk::calcChunckPos(this->cameraPosition);          //给出当前所在的区块
    QVector2D startCPos=GMath::v3d2v2d(cdPos);                                                  //将所在区块定义为起始区块。

    bfs2World(startCPos);

    foreach (ChunkMap *cm, chunksMap) {
        if(cm && !cm->inDraw()){                                        //有效区块且区块不在绘图状态
            if(GMath::gAbs(int(cm->getChunkPosition().distanceToPoint(startCPos)))>maxRenderLen+2){
                cm->setShow(false);
                QString key=getKey(cm->getChunkPosition());
                if(cm->haveChange()){                                                               //未保存先保存
                    saveChunk(key);
                }
                chunksMap.remove(key);
                delete cm;
            }
        }
    }

    qDebug()<<"load end";
    upLock=false;
}

void World::forcedUpdateWorld()
{
    chunksMap.clear();
    updateWorld();
}

void World::loadBlockIndex()
{
    mBlockIndex.clear();
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
            mBlockIndex<<node;
        }
    }
    file.close();

    file.setFileName(":/res/divinecraft/textures/blocks/texture_index.list");
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
                foreach (BlockListNode *node, mBlockIndex) {
                    node->texWidth=tWidth;
                    node->texHeight=tHeight;
                }
            }
            else{
                QStringList temp=line.split(" ",QString::SkipEmptyParts);
                int i=0;
                int id=temp[i++].toInt();
                int type=temp[i++].toInt();
                if(id>=mBlockIndex.length()) continue;
                BlockListNode *bl=mBlockIndex[id];
                int j=0;
                int nc;
                if(type==0)
                    nc=6;
                else if(type==1)
                    nc=4;
                while(i<temp.length() && j<nc){
                    QString u=temp[i++];
                    bl->texName<<u;
                    j++;
                }
            }
            index++;
        }
    }
    //    foreach (BlockListNode *a, mBlockIndex) {
    //        qDebug()<<a->id<<" "<<a->type<<" "<<a->name<<" "<<a->tex;
    //    }
}

void World::setBlockListLength(int len)
{
    foreach (BlockListNode *node, mBlockIndex) {
        if(node)
            node->texLength=len;
    }
}

void World::calcBlockListNodeTexId(const QMap<QString, int> &texMap)
{
    foreach (BlockListNode *node, mBlockIndex) {
        if(node){
            node->tex.clear();
            foreach (QString name, node->texName) {
                node->tex.append(texMap.value(name));
            }
        }
    }
}

void World::setFirstCameraPosition(const QVector3D &cPos)
{
    cameraPosition = cPos;
    lastCameraChunk=GMath::v3d2v2d(DisplayChunk::calcChunckPos(cameraPosition));
}

void World::autoSave()
{
    foreach (ChunkMap *cm, chunksMap) {
        if(cm && cm->haveChange()){
            saveChunk(getKey(cm->getChunkPosition()));
        }
    }
}

void World::updateDraw()
{
    QTime fTime=QTime::currentTime();
    while(!updateQueue.isEmpty() && fTime.msecsTo(QTime::currentTime())<=5){                      //没次刷新最多只进行5ms
        QString key=updateQueue.front();
        updateQueue.pop_front();
        ChunkMap *ch=chunksMap.value(key);
        if(ch)
            ch->updateAll();
    }
}


QString World::getKey(QVector2D chunkPos)
{
    QString sKey=tr("%1x%2").arg((int)chunkPos.x()).arg((int)chunkPos.y());
    return sKey;
}

QString World::getKey(int x, int y)
{
    return getKey(QVector2D(x,y));
}

ChunkMap *World::loadChunk(QVector2D chunkPos)
{
    ChunkMap *newChunk;
    QString key=getKey(chunkPos);

    //    qDebug()<<"Load:"<<key;

    QDir idir(getFilePath());
    if(idir.absolutePath()!=filePath){
        QMessageBox::warning(0,tr("错误"),tr("游戏无法正确的创建文件和目录，即将被关闭\n此错误可能与文件读写权限有关"));
        exit(1);
    }
    if(!idir.cd("chunks/")){
        idir.mkdir("chunks/");
        idir.cd("chunks/");
    }
    bool ok=false;      //文件成功并完成读取打开的标志
    QString chunkFile=tr("%1/%2.gck").arg(idir.absolutePath()).arg(key);
    QFile file(chunkFile);
    if(file.open(QIODevice::ReadOnly)){
        QDataStream in(&file);
        int za,zb;
        in>>za>>zb;
        if(za==0x9394ef && zb==0x7b2f5c){                               //如果文件达标，创建并读取区块
            newChunk=new ChunkMap(chunkPos);
            while(!in.atEnd()){
                int b;
                QVector3D blockPos;
                int blockIndex;
                in>>b>>blockPos>>blockIndex;
                if(b!=0x01){
                    qWarning()<<tr("文件读取出错!");
                    break;
                }
                newChunk->addBlock(new Block(blockPos,getBlockIndex(blockIndex)),false);
            }
            ok=true;
        }
    }
    if(!ok){           //文件无法打开或读取错误,新建区块
        newChunk=createChunk(chunkPos);
    }
    else{
        newChunk->saveAll();                            //因为是从文件读取的，将区块标为已保存，防止重复写入
        //        qDebug()<<"load on file";
    }
    //        newChunk->updateAll();
    file.close();
    return newChunk;
}

ChunkMap *World::createChunk(QVector2D chunkPos)
{
    QVector3D nPos=QVector3D(chunkPos.x(),0,chunkPos.y());
    QVector3D oPos=DisplayChunk::calcChunkOriginPos(nPos);                  //计算出区块的偏移坐标
    ChunkMap *newChunk=new ChunkMap(chunkPos);
    int i,j,k;
    int cb[]={4,1,2};
    for(k=0;k<3;k++){
        for(i=0;i<16;i++){
            for(j=0;j<16;j++){
                QVector3D bPos=oPos+QVector3D(i,k,j);
                newChunk->addBlock(new Block(bPos,getBlockIndex(cb[k])),false);
            }
        }
    }

    int x=qrand()%16;
    int z=qrand()%16;
    int b=qrand()%getBlockIndex(0)->texLength;
    newChunk->addBlock(new Block(oPos+QVector3D(x,3,z),getBlockIndex(b)),false);

    return newChunk;
}

void World::bfs2World(const QVector2D &start)
{
    QMap<QString,bool> flag;                                            //路径标记
    int rr[4][2]={
        {-1,0},{1,0},{0,-1},{0,1}
    };                                                              //四个遍历方向
    QQueue<QVector2D> Q;
    Q.push_back(start);
    flag.insert(getKey(start),true);
    while(!Q.isEmpty()){
        QVector2D nPos=Q.front();
        Q.pop_front();
        QString key=getKey(nPos);
        if(chunksMap.value(key)==NULL){
            chunksMap.insert(key,loadChunk(nPos));                  //所到之处是空的，加载之
            saveChunk(key);                                                                 //保存一个
            updateQueue.push_back(key);                             //增加到刷新等待队列
        }
        QThread::msleep(1);                             //每一个区块加载完休息1ms
        for(int i=0;i<4;i++){                                                       //遍历是个方向
            int xx=nPos.x()+rr[i][0];
            int yy=nPos.y()+rr[i][1];
            QVector2D temp(xx,yy);

            if(flag.value(getKey(temp))==false
                    && GMath::gAbs(temp.distanceToPoint(start))<=maxRenderLen+1){                            //未涉及且在可视渲染距离内的区块，入队
                Q.push_back(temp);
                flag.insert(getKey(temp),true);
            }
        }
    }

    //以下两句其实多余
    flag.clear();
    Q.clear();
}

bool World::saveChunk(QString key)
{
    ChunkMap *sc=chunksMap.value(key);
    if(!sc)                 //未创建的区块，pass
        return false;
    if(sc->haveChange()==false){            //无修改，直接返还完成
        return true;
    }

    QDir sdir(getFilePath());
    if(sdir.absolutePath()!=filePath){
        QMessageBox::warning(0,tr("错误"),tr("游戏无法正确的创建文件和目录，即将被关闭\n此错误可能与文件读写权限有关"));
        exit(1);
    }
    if(!sdir.cd("chunks/")){
        sdir.mkdir("chunks/");
        sdir.cd("chunks/");
    }
    QString chunkFile=tr("%1/%2.gck").arg(sdir.absolutePath()).arg(key);
    QFile file(chunkFile);
    if(file.open(QIODevice::WriteOnly)){
        QDataStream out(&file);
        out<<0x9394ef<<0x7b2f5c;                                                                        //插入标识符
        for(int i=0;i<16;i++){                                                                                          //遍历并将所有方块信息写入到文件
            DisplayChunk *sdc=sc->getDisplayChunk(i);
            if(sdc && sdc->isOk()){
                foreach (Block *b, sdc->getBlocks()) {
                    if(b && b->isAir()==false){                                                             //只保存非空气方块
                        out<<0x01<<b->getPosition()<<b->getId();
                    }
                }
            }
        }
    }
    else{
        return false;
    }
    file.close();

    sc->saveAll();                                                                              //标识为已保存
    qDebug()<<QTime::currentTime()<<"save ok";
    return true;
}

void World::setfilePath()
{
    if(worldName==NULL || worldName=="")
    {
        qWarning("警告:在World::setfilePath中可能存在错误，因为worldName为空!");
        return ;
    }
    filePath.clear();
    QDir dir(".");
    if(!dir.cd("./.divineCraft/")){
        dir.mkdir("./.divineCraft/");
        dir.cd("./.divineCraft/");
    }
    if(!dir.cd("save/")){
        dir.mkdir("save/");
        dir.cd("save/");
    }
    if(!dir.cd(tr("%1/").arg(worldName))){
        dir.mkdir(tr("%1/").arg(worldName));
        dir.cd(tr("%1/").arg(worldName));
    }
    filePath=dir.absolutePath();
}
QString World::getWorldName() const
{
    return worldName;
}

void World::setWorldName(const QString &value)
{
    worldName = value;
}

QString World::getFilePath()
{
    if(filePath==NULL || filePath=="")
        setfilePath();
    return filePath;
}


int World::getMaxRenderLen() const
{
    return maxRenderLen;
}

void World::setMaxRenderLen(int value)
{
    maxRenderLen = std::min(value,30);
}

BlockListNode *World::getBlockIndex(int index)
{
    if(mBlockIndex.length()<=0)
        loadBlockIndex();

    if(index>=mBlockIndex.length() || index<0)
        return mBlockIndex[0];
    return mBlockIndex[index];
}


QVector3D World::getCameraPosition() const
{
    return cameraPosition;
}

void World::changeCameraPosition(const QVector3D &cPos)
{
    cameraPosition = cPos;
    QVector2D nowC=GMath::v3d2v2d(DisplayChunk::calcChunckPos(cameraPosition));
    if(nowC!=lastCameraChunk){
        lastCameraChunk=nowC;
        updateWorld();
    }
}
