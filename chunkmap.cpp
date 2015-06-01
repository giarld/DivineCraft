/*
 * Author:Gxin
 */
#include "chunkmap.h"
#include "block.h"
#include "glkernel/chunkmesh.h"
#include "gmath.h"
#include "world.h"

///========================================//
///
/// ========================================//
ChunkMap::ChunkMap(QVector2D cPos, World *myWorld)
    :chunkPosition(cPos)
    ,lastOPDC(NULL)
    ,show(true)
    ,myWorld(myWorld)
{
    displayChunk.clear();
    drawLock=false;
}

ChunkMap::ChunkMap(int cx, int cz, World *myWorld)
    :chunkPosition(QVector2D(cx,cz))
    ,lastOPDC(NULL)
    ,show(true)
    ,myWorld(myWorld)
{
    displayChunk.clear();
    drawLock=false;
}

ChunkMap::~ChunkMap()
{
    foreach (DisplayChunk *d, displayChunk) {
        if(d)
            delete d;
    }
    displayChunk.clear();
}

bool ChunkMap::addBlock(Block *block, bool update)
{
    QVector3D dcPos=DisplayChunk::calcChunckPos(block->getPosition());
    int dkey=dcPos.y();
    QVector2D cPos=GMath::v3d2v2d(dcPos);
    if(cPos!=chunkPosition)                                         //方块不属于当前区块，pass
        return false;
    if(displayChunk.value(dkey)==NULL){
        if(!createDisplayChunk(dcPos)) return false;
    }
    //获得DisplayChunk完成之后
    DisplayChunk *dc=displayChunk.value(dkey);
    dc->addBlock(block,update);
    lastOPDC=dc;                                                            //设置上一个操作区块为当前
    return true;
}

bool ChunkMap::removeBlock(QVector3D pos, bool update)
{
    QVector3D dcPos=DisplayChunk::calcChunckPos(pos);
    int dkey=dcPos.y();
    QVector2D cPos=GMath::v3d2v2d(dcPos);
    if(cPos!=chunkPosition)                                         //方块不属于当前区块，pass
        return false;
    DisplayChunk *dc=displayChunk.value(dkey);
    if(dc==NULL)
        return false;
    lastOPDC=dc;                                            //设置上一个操作区块为当前
    return dc->removeBlock(pos,update);
}

Block *ChunkMap::getBlock(QVector3D bPos)
{
    QVector3D dcPos=DisplayChunk::calcChunckPos(bPos);
    int dkey=dcPos.y();
    QVector2D cPos=GMath::v3d2v2d(dcPos);
    if(cPos!=chunkPosition)                                                 //坐标不是当前区块,pass
        return NULL;
    DisplayChunk *dc=displayChunk.value(dkey);
    if(dc==NULL)                              //实现区块未生成，pass
        return NULL;
    return dc->getBlock(bPos);
}


DisplayChunk *ChunkMap::getDisplayChunk(int y)
{
    return displayChunk.value(y);
}

void ChunkMap::draw(const QVector3D &pos, int maxLen)
{
    if(!isShow())
        return ;
    if(drawLock)
        return ;
    drawLock=true;

    QVector3D bPos=DisplayChunk::calcChunckPos(pos);
    foreach (DisplayChunk *dc, displayChunk) {
        if(dc && GMath::gAbs(int(dc->getDcPosition().distanceToPoint(bPos)))<=maxLen)          //有效区块且离camera区块的距离小于等于maxLen
        {
            dc->draw();
        }
    }
    drawLock=false;
}

bool ChunkMap::inDraw()
{
    return drawLock;
}

void ChunkMap::update(int y)
{
    if(y<MINLOW || y>MAXHIGHT)
        return ;
    if(displayChunk.value(y)==NULL)
        return ;
    displayChunk.value(y)->update();
}

void ChunkMap::updateLast()
{
    if(lastOPDC!=NULL)
        lastOPDC->update();
}

void ChunkMap::updateAll()
{
    foreach (DisplayChunk *dc, displayChunk) {
        if(dc)
            dc->update();
    }
}

bool ChunkMap::haveChange()
{
    foreach (DisplayChunk *dc, displayChunk) {
        if(dc){
            if(dc->getHaveChange())
                return true;
        }
    }
    return false;
}

void ChunkMap::saveAll()
{
    foreach (DisplayChunk *dc, displayChunk) {
        if(dc){
            dc->setHaveChange(false);
        }
    }
}

bool ChunkMap::createDisplayChunk(QVector3D dcPos)
{
    QVector2D cPos=GMath::v3d2v2d(dcPos);
    if(cPos!=chunkPosition)                                                 //显示区块不属于当前区块，pass
        return false;
    int key=dcPos.y();
    if(key<MINLOW || key>MAXHIGHT)                              //显示区块越界,pass
        return false;
    if(displayChunk.value(key)!=NULL)                                   //存在了，返还完成
        return true;
    displayChunk.insert(key,new DisplayChunk(dcPos,myWorld));
    return true;
}
QVector2D ChunkMap::getChunkPosition() const
{
    return chunkPosition;
}

void ChunkMap::setShow(bool s)
{
    this->show=s;
}

bool ChunkMap::isShow()
{
    return show;
}

//void ChunkMap::setChunkPosition(const QVector2D &value)
//{
//    chunkPosition = value;
//}


//=============================================//

DisplayChunk::DisplayChunk()
    :dcPosition(QVector3D(0.0,-1.0,0.0))
    ,blockCount(0)
    ,displayListID(GLuint(0))
{
    blocks.clear();
    setHaveChange(false);
}

DisplayChunk::DisplayChunk(int cx, int cy, int cz, World *myWorld)
    :dcPosition(QVector3D(cx,cy,cz))
    ,blockCount(0)
    ,displayListID(GLuint(0))
    ,myWorld(myWorld)
{
    blocks.clear();
    setHaveChange(false);
}

DisplayChunk::DisplayChunk(QVector3D dcPos, World *myWorld)
    :dcPosition(dcPos)
    ,blockCount(0)
    ,displayListID(GLuint(0))
    ,myWorld(myWorld)
{
    blocks.clear();
}

DisplayChunk::~DisplayChunk()
{
    deleteDisplayList();
    foreach (Block*b, blocks) {
        if(b)
            delete b;
    }
    blocks.clear();
}

void DisplayChunk::resetDisplayChunk()
{
    resetDisplayChunk(QVector3D(0.0,-1.0,0.0));
}

void DisplayChunk::resetDisplayChunk(QVector3D dcPos)
{
    dcPosition=dcPos;
    foreach (Block*b, blocks) {
        if(b)
            delete b;
    }
    blocks.clear();
    blockCount=0;
    deleteDisplayList();
}

bool DisplayChunk::addBlock(Block *block, bool update)
{
    if(calcChunckPos(block->getPosition())!=dcPosition)        //方块不属于当前显示区块，不增加
        return false;
    if(!block)                                                    //空不添加
        return false;
    int key=calcKey(block->getPosition());
    if(blocks.value(key)!=NULL)                    //此处有方块，先移除
        removeBlock(blocks.value(key)->getPosition(),update);                    //属性与否尊崇原则

    blocks.insert(key,block);
    if(block->isAir()==false)
        blockCount+=1;

    if(update)
        this->update();
    this->setHaveChange(true);                              //标记区块有修改
    return true;
}

bool DisplayChunk::removeBlock(QVector3D pos, bool update)
{
    if(calcChunckPos(pos)!=dcPosition)             //方块不在本区块，跳过
        return false;
    int key=calcKey(pos);
    Block *tb=blocks.value(key);
    if(!tb || tb->isAir())                              //空气方块不操作
        return false;
    delete tb;
    blocks.insert(key,NULL);
    blockCount-=1;
    if(update)
        this->update();
    this->setHaveChange(true);                      //标记区块有修改
    return true;
}

void DisplayChunk::update()
{
    if(! isOk())            //非就绪区块或者未分配显示列表不进行视图刷新
        return ;
    genDisplayList();
    updateDisplayList();
}

bool DisplayChunk::isOk()
{
    if(dcPosition.y()<0)                   //界外区块，notOk
        return false;
    if(blockCount<=0)                   //空区块,notOk
        return false;
    return true;
}

Block *DisplayChunk::getBlock(QVector3D bPos)
{
    if(calcChunckPos(bPos)!=dcPosition)
        return NULL;
    int key=calcKey(bPos);
    return blocks.value(key);
}

void DisplayChunk::draw()
{
    if(displayListID!=0 && isOk())
        glCallList(displayListID);
}
GLuint DisplayChunk::getDisplayListID() const
{
    return displayListID;
}

void DisplayChunk::setDisplayListID(const GLuint &value)
{
    displayListID = value;
}

QVector3D DisplayChunk::blockPos2dcPos(QVector3D bPos)
{
    int x=GMath::gAbs((int)bPos.x());
    int y=GMath::gAbs((int)bPos.y());
    int z=GMath::gAbs((int)bPos.z());
    int xx=x%16;
    int yy=y%16;
    int zz=z%16;
    QVector3D dcPos=QVector3D(xx,yy,zz);
    return dcPos;
}

QVector3D DisplayChunk::calcChunckPos(QVector3D bPos)
{
    int x=(int)bPos.x();
    int y=(int)bPos.y();
    int z=(int)bPos.z();
    int xx=GMath::g2Int(x/16.0);
    int yy=GMath::g2Int(y/16.0);
    int zz=GMath::g2Int(z/16.0);
    QVector3D pos=QVector3D(xx,yy,zz);
    return pos;
}

QVector3D DisplayChunk::calcChunkOriginPos(QVector3D cPos)
{
    int x=int(cPos.x())+1;
    int y=int(cPos.y())+1;
    int z=int(cPos.z())+1;
    int xx=x*16-16;
    int yy=y*16-16;
    int zz=z*16-16;
    return QVector3D(xx,yy,zz);
}

int DisplayChunk::calcKey(QVector3D bPos)
{
    QVector3D dcPos=blockPos2dcPos(bPos);
    int x=(int)dcPos.x();
    int y=(int)dcPos.y();
    int z=(int)dcPos.z();
    int key=(16*16)*y+16*z+x;
    return key;
}

///
/// \brief DisplayChunk::updateDisplayList
///
void DisplayChunk::updateDisplayList()
{
    int faceSum=0;
    QVector<Face *> faces;
    QMapIterator<int,Block*> mi(blocks);
    while(mi.hasNext()){        //计算可绘制的面数，面的消隐算法
        Block *temp=mi.next().value();
        if(!temp || temp->isAir()) continue;             //空气方块跳过
        for(int i=0;i<temp->faceSum();i++){
            Block *vis=myWorld->getBlock(Block::vicinityPosition(temp->getPosition(),i));
            if(vis==NULL || vis->isAir()){
                faces<<temp->getFace(i);
                continue;
            }
            if(temp->doNotHideFace()){                    //强制不消隐
                faces<<temp->getFace(i);
                continue;
            }
//            if(temp->getType()==1){
//                faces<<temp->getFace(i);
//                continue;
//            }
            if(temp->getId()==vis->getId())             //相同方块间消隐
                continue;
            if(vis->isTrans()==false)                       //透明方块的邻近方块不消隐
                continue;

            faces<<temp->getFace(i);
        }
    }
    faceSum=faces.length();
//    qDebug()<<"Face Sum = "<<faceSum;
    glNewList(displayListID,GL_COMPILE);

    if(faceSum>=16200){                                                 //每个mesh最多容纳16200个面，如果面数多余16200，再创建一个mesh
        faceSum=16200;
    }
    ChunkMesh *mesh=new ChunkMesh(faceSum);
    //    foreach (Face *f, faces) {
    //        mesh->addFace(f);
    //    }
    int ii;
    for(ii=0;ii<faceSum;ii++){
        mesh->addFace(faces[ii]);
    }
    mesh->draw();
    delete mesh;
    //面数超过配额，增加新的buffer
    if(ii<faces.length()){
        ChunkMesh *mesh2=new ChunkMesh(faces.length()-faceSum+1);
        for(int j=ii;j<faces.length();j++){
            mesh2->addFace(faces[j]);
        }
        mesh2->draw();
        delete mesh2;
    }
    glEndList();
    faces.clear();
}

void DisplayChunk::genDisplayList()
{
    if(displayListID==0)
        displayListID=glGenLists(1);
}

void DisplayChunk::deleteDisplayList()
{
    if(displayListID!=GLuint(0)){
        glDeleteLists(displayListID,1);
        displayListID=GLuint(0);
    }
}

QMap<int, Block *> DisplayChunk::getBlocks() const
{
    return blocks;
}

bool DisplayChunk::getHaveChange() const
{
    return haveChange;
}

void DisplayChunk::setHaveChange(bool value)
{
    haveChange = value;
}

QVector3D DisplayChunk::getDcPosition() const
{
    return dcPosition;
}
