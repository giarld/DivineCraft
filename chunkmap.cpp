#include "chunkmap.h"
#include "block.h"
#include "glkernel/chunkmesh.h"

int gAbs(int x)
{
    return x>=0?x:-x;
}

///========================================//
///
/// ========================================//
ChunkMap::ChunkMap()
{

}

ChunkMap::~ChunkMap()
{

}

//=============================================//

DisplayChunk::DisplayChunk()
    :dcPosition(QVector3D(0.0,-1.0,0.0))
    ,blockCount(0)
    ,displayListID(GLuint(0))
{
    blocks.clear();
}

DisplayChunk::DisplayChunk(QVector3D dcPos)
    :dcPosition(dcPos)
    ,blockCount(0)
    ,displayListID(GLuint(0))
{
    blocks.clear();
}

DisplayChunk::~DisplayChunk()
{

}

void DisplayChunk::resetDisplayChunk()
{
    resetDisplayChunk(QVector3D(0.0,-1.0,0.0));
}

void DisplayChunk::resetDisplayChunk(QVector3D dcPos)
{
    dcPosition=dcPos;
    blockCount=0;
    displayListID=GLuint(0);
    blocks.clear();
}

bool DisplayChunk::addBlock(Block *block, bool update)
{
    if(calcChunckPos(block->getPosition())!=dcPosition)        //方块不属于当前显示区块，不增加
        return false;
    if(!block)                                                    //空不添加
        return false;
    int key=calcKey(block->getPosition());
    blocks.insert(key,block);
    if(block->isAir()==false)
        blockCount+=1;

    if(update)
        this->update();

    return true;
}

bool DisplayChunk::removeBlock(QVector3D pos)
{
    if(calcChunckPos(pos)!=dcPosition)             //方块不在本区块，跳过
        return false;
    int key=calcKey(pos);
    Block *tb=blocks.value(key);
    if(!tb || tb->isAir())                              //空气方块不操作
        return false;
    blocks.insert(key,NULL);
    blockCount-=1;
    update();
    return true;
}

void DisplayChunk::update()
{
    if(! isOk() || displayListID==0)            //非就绪区块或者未分配显示列表不进行视图刷新
        return ;
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
    int key=calcKey(bPos);
    return blocks.value(key);
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
    int x=gAbs((int)bPos.x());
    int y=gAbs((int)bPos.y());
    int z=gAbs((int)bPos.z());
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
    int xx=x>=0?x/16:x/16-1;
    int yy=y>=0?y/16:y/16-1;
    int zz=z>=0?z/16:z/16-1;
    QVector3D pos=QVector3D(xx,yy,zz);
    return pos;
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

void DisplayChunk::updateDisplayList()
{
    int faceSum=0;
    QMapIterator<int,Block*> mi(blocks);
    while(mi.hasNext()){        //计算可绘制的面数
        Block *temp=mi.next().value();
        if(!temp || temp->isAir()) continue;             //空气方块跳过
       faceSum+=temp->getShowFaceSum();
    }
    qDebug()<<"Face Sum = "<<faceSum;

    QMapIterator<int,Block*> mj(blocks);
    ChunkMesh *mesh=new ChunkMesh(faceSum);
    glNewList(displayListID,GL_COMPILE);
        while(mj.hasNext()){
            Block *temp=mj.next().value();
            if(!temp || temp->isAir()) continue;
            for(int i=0;i<temp->faceSum();i++){
                if(temp->isFaceHide(i)==false){
                    mesh->addFace(temp->getFace(i));
                }
            }
        }
        mesh->draw();
    glEndList();
    delete mesh;
}
