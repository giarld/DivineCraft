#include "world.h"
#include <QDataStream>
#include <QDir>
#include <QFile>
#include "gmath.h"

World::World(QObject *parent) : QObject(parent)
{
    setMaxRenderLen(16);
}

World::~World()
{
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
    QVector3D cdPos=DisplayChunk::calcChunckPos(this->cameraPosition);
    QString key=getKey(GMath::v3d2v2d(cdPos));

}

void World::saveChunk()
{

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

int World::getMaxRenderLen() const
{
    return maxRenderLen;
}

void World::setMaxRenderLen(int value)
{
    maxRenderLen = value;
}


QVector3D World::getCameraPosition() const
{
    return cameraPosition;
}

void World::setCameraPosition(const QVector3D &value)
{
    cameraPosition = value;
}
