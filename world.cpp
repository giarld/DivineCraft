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
    lockDQueue=false;
    drawID=glGenLists(1);
    qsrand(time(0));            //初始化随机种子
}

World::~World()
{
    foreach (ChunkMap *c, chunksMap) {
        if(c)
            delete c;
    }
    chunksMap.clear();
    foreach (BlockListNode *t, mBlockIndex) {
        if(t)
            delete t;
    }
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

    if(tempc->addBlock(block,update)){                               //增加成功则刷新显示列表
        updateDisplayChunkQueue.push_back(block->getPosition());            //增加到刷新等待队列
        return true;
    }
    return false;
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

    if(tempc->removeBlock(pos,update)){                         //删除成功则刷新显示列表
        updateDisplayChunkQueue.push_back(pos);                 //增加到刷新等待队列
        return true;
    }
    return false;
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
    //    foreach (ChunkMap *cm, chunksMap) {
    //        if(cm){
    //            cm->draw(this->cameraPosition,this->maxRenderLen);
    //        }
    //    }
    glCallList(drawID);
}

void World::updateWorld()
{
    if(upLock)
        return;
    upLock=true;
    qDebug()<<QTime::currentTime()<<"update world : "<<this->cameraPosition;
    QVector3D cdPos=DisplayChunk::calcChunckPos(this->cameraPosition);          //给出当前所在的区块
    QVector2D startCPos=GMath::v3d2v2d(cdPos);                                                  //将所在区块定义为起始区块。

    foreach (ChunkMap *cm, chunksMap) {                         //移除视野外方块
        if(cm){                                        //有效区
            if(GMath::gAbs(int(cm->getChunkPosition().distanceToPoint(startCPos)))>=maxRenderLen+2){
                //                cm->setShow(false);
                QString key=getKey(cm->getChunkPosition());
                if(cm->haveChange()){                                                               //未保存先保存
                    saveChunk(key);
                }
                chunksMap.remove(key);
                deleteChunckQueue.push_back(cm);                        //推送到队列准备销毁
            }
        }
    }

    bfs2World(startCPos);               //遍历可见区

    qDebug()<<"load end";
    upLock=false;
}

void World::forcedUpdateWorld()
{
    updateQueue.clear();
    updateDisplayChunkQueue.clear();

    foreach (ChunkMap *ch, chunksMap) {
        deleteChunckQueue.push_back(ch);
    }
    chunksMap.clear();
    forcedUpdate=true;
    updateWorld();
}

void World::loadBlockIndex()
{
    mBlockIndex.clear();
    QFile file(":/res/divinecraft/block.list");
    if(file.open(QIODevice::ReadOnly)){
        QTextStream in(&file);
        in.setCodec("utf-8");
        while(!in.atEnd()){
            QString line=in.readLine();
            if(line=="" || line==NULL) continue;
            QStringList temp=line.split(" ",QString::SkipEmptyParts);
            int id=temp[0].toInt();
            int type=temp[1].toInt();
            bool collide=temp[2].toInt();
            bool trans=temp[3].toInt();
            bool notHide=temp[4].toInt();
            QString name=temp[5];
            BlockListNode *node=new BlockListNode;
            node->id=id;
            node->type=type;
            node->name=name;
            node->collide=collide;
            node->trans=trans;
            node->dnotHideFace=notHide;
            mBlockIndex<<node;
        }
    }
    file.close();

    file.setFileName(":/res/divinecraft/textures/blocks/texture_index.list");
    if(file.open(QIODevice::ReadOnly)){
        QTextStream in(&file);
        in.setCodec("utf-8");
        int index=0;
        while(!in.atEnd() && index<=mBlockIndex.length()){
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
                int nc=0;
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
    QString unknown="unknown";
    for(int ii=0;ii < mBlockIndex.length();ii++){                 //未找到材质的用未知材质覆盖
        int type=mBlockIndex[ii]->type;
        int x=6;
        if(type==1)
            x=4;
        int k;
        for(k=0;k<mBlockIndex[ii]->texName.length();k++){       //空白覆盖
            if(mBlockIndex[ii]->texName[k]=="")
                mBlockIndex[ii]->texName[k]=unknown;
        }
        for(;k<x;k++){              //缺失覆盖
            mBlockIndex[ii]->texName<<unknown;
        }
    }
    //    foreach (BlockListNode *a, mBlockIndex) {
    //        qDebug()<<a->id<<" "<<a->type<<" "<<a->name<<" "<<a->texName;
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
            for(int i=0;i<node->texName.length();i++){
                QString name = node->texName[i];
                int dep=texMap.value(name);
                if(dep==0 && node->id!=0){
                    dep=texMap.value("unknown");
                    node->texName[i]="unknown";
                }
                node->tex.append(dep);
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
        if(upLock){                     //这在里更新互斥锁的作用是仿制更新线程对chunksMap进行insert时主线程对其进行value的互斥操作
            break;
        }
        QString key=updateQueue.front();
        updateQueue.pop_front();
        ChunkMap *ch=chunksMap.value(key);
        if(ch)
            ch->updateAll();
        //
        updateDisplay();
        if(forcedUpdate){
            emit loadOver();
            forcedUpdate=false;
        }
    }

    fTime=QTime::currentTime();
    lockDQueue=true;
    while(!updateDisplayChunkQueue.isEmpty() && fTime.msecsTo(QTime::currentTime())<=5){        //增加删除方块后的区块更新处理
        QVector3D cPos=updateDisplayChunkQueue.front();
        QVector3D dPos=DisplayChunk::calcChunckPos(cPos);
        updateDisplayChunkQueue.pop_front();
        for(int i=0;i<Block::MAX_FACE_SUM;i++){
            QVector3D tPos=Block::vicinityPosition(cPos,i);
            QVector3D tdPos=DisplayChunk::calcChunckPos(tPos);
            if(tdPos!=dPos){
                chunksMap.value(getKey(GMath::v3d2v2d(tdPos)))->update(tdPos.y());
            }
        }
        updateDisplay();
    }
    lockDQueue=false;
    fTime=QTime::currentTime();
    while(!deleteChunckQueue.isEmpty() && fTime.msecsTo(QTime::currentTime())<=5){                      //释放内存的操作
        ChunkMap *cm=deleteChunckQueue.front();
        deleteChunckQueue.pop_front();
        if(cm)
            delete cm;
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
            newChunk=new ChunkMap(chunkPos,this);
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
    ChunkMap *newChunk=new ChunkMap(chunkPos,this);
    int i,j,k;
    int cb[]={3,5,1,2};
    for(k=0;k<4;k++){
        for(i=0;i<16;i++){
            for(j=0;j<16;j++){
                QVector3D bPos=oPos+QVector3D(i,k,j);
                //                if((i==0 || j==0) && k==2)
                //                    newChunk->addBlock(new Block(bPos,getBlockIndex(10)),false);
                //                else
                newChunk->addBlock(new Block(bPos,getBlockIndex(cb[k])),false);
            }
        }
    }

    //生成树木
    bool cT=qrand()%10>=5?1:0;
    int tree[][2]={{35,41},{36,42},{37,43}};
    int tn=qrand()%3;
    if(cT){
        int treeH=qrand()%4+5;
        int sx=7+oPos.x();
        int sz=7+oPos.z();
        for(int i=4;i<=treeH+4;i++){
            newChunk->addBlock(new Block(QVector3D(sx,i,sz),getBlockIndex(tree[tn][0])),false);
        }
        newChunk->addBlock(new Block(QVector3D(sx,treeH+4,sz),getBlockIndex(tree[tn][1])),false);

        for(int h=treeH/2+4;h<=treeH+4;h++){
            int r=qrand()%3+2;
            if(h>treeH/2+4){
                r-=1;
            }
            if(r<2)
                r=2;

            for(int x=sx-r;x<=sx+r;x++){
                for(int z=sz-r;z<=sz+r;z++){
                    if(!(QVector2D(x,z).distanceToPoint(QVector2D(sx,sz))>r || (x==sx&& z==sz))){
                        newChunk->addBlock(new Block(QVector3D(x,h,z),getBlockIndex(tree[tn][1])),false);
                    }
                }
            }
        }
    }

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
        for(int i=0;i<4;i++){                                                       //遍历是个方向将没有被加载的区块入队等待加载
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

void World::updateDisplay()
{
    glNewList(drawID,GL_COMPILE);
    foreach (ChunkMap *cm, chunksMap) {
        if(cm){
            cm->draw(this->cameraPosition,this->maxRenderLen);
        }
    }
    glEndList();
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
    maxRenderLen = std::min(value/2,16);                        //最大渲染距离是渲染宽度的一半（人在中间嘛）
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
    QVector3D bPos=DisplayChunk::calcChunckPos(cPos);
    QVector2D nowC=GMath::v3d2v2d(bPos);
    if(nowC!=lastCameraChunk){                                              //跨越了区块要进行区块更新
        lastCameraChunk=nowC;
        updateWorld();
    }
    else if(!lockDQueue && lastCameraHight!=bPos.y()){                       //随着高度的变化，要刷掉视距以外的物体（考虑队列互斥）
        lastCameraHight=bPos.y();
        updateDisplayChunkQueue.push_back(cPos);                //由于当前函数不再主线程中，只能寻求主线程进行显示刷新
    }

}
