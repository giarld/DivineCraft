#include "world.h"
#include <QDataStream>
#include <QTextStream>
#include <QDir>
#include <QFile>
#include "gmath.h"

World::World(QObject *parent) : QObject(parent)
{
    setMaxRenderLen(16);
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
    QVector3D cdPos=DisplayChunk::calcChunckPos(this->cameraPosition);          //给出当前所在的区块
    QString key=getKey(GMath::v3d2v2d(cdPos));
    //test
    ChunkMap*newCm=loadChunk(GMath::v3d2v2d(cdPos));
    newCm->updateAll();
    chunksMap.insert(key,newCm);
}

void World::saveChunk()
{

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
    //    foreach (BlockListNode *a, mBlockIndex) {
    //        qDebug()<<a->id<<" "<<a->type<<" "<<a->name<<" "<<a->tex;
    //    }
}

QString World::getKey(QVector2D chunkPos)
{
    QString sKey=tr("%1-%2").arg((int)chunkPos.x()).arg((int)chunkPos.y());
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

    if(filePath==NULL || filePath=="")
        setfilePath();
    QDir idir(filePath);
    if(idir.absolutePath()!=filePath){
        QMessageBox::warning(0,tr("错误"),tr("游戏无法正确的创建文件和目录，即将被关闭\n此错误可能与文件读写权限有关"));
        exit(0);
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
                int b,e;
                QVector3D blockPos;
                int blockIndex;
                in>>b>>blockPos>>blockIndex>>e;
                newChunk->addBlock(new Block(blockPos,getBlockIndex(blockIndex)),false);
            }
            ok=true;
        }
    }
    if(!ok){           //文件无法打开或读取错误,新建区块
        newChunk=createChunk(chunkPos);
    }
//        newChunk->updateAll();
    file.close();
    return newChunk;
}

ChunkMap *World::createChunk(QVector2D chunkPos)
{
    QVector3D nPos=QVector3D(chunkPos.x(),0,chunkPos.y());
    QVector3D oPos=DisplayChunk::calcChunkOriginPos(nPos);
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
    return newChunk;
}

void World::setfilePath()
{
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


int World::getMaxRenderLen() const
{
    return maxRenderLen;
}

void World::setMaxRenderLen(int value)
{
    maxRenderLen = value;
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

void World::setCameraPosition(const QVector3D &value)
{
    cameraPosition = value;
}
