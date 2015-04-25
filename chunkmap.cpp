#include "chunkmap.h"
#include "block.h"
#include "glkernel/chunkmesh.h"

int gAbs(int x)
{
    return x>=0?x:-x;
}

int g2Int(float x)                      //浮点数转整数的自定义规则
{
    if(float(int(x))==x)
        return x;
    if(x>=0.0)
        return int(x);
    else{
        return int(x)-1;
    }
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

DisplayChunk *ChunkMap::getDisplayChunk()
{

}

QVector2D ChunkMap::v3d2v2d(const QVector3D &v3d)
{
    float x=v3d.x();
    float z=v3d.z();
    return QVector2D(x,z);
}

//=============================================//

DisplayChunk::DisplayChunk()
    :dcPosition(QVector3D(0.0,-1.0,0.0))
    ,blockCount(0)
    ,show(true)
    ,displayListID(GLuint(0))
{
    blocks.clear();
}

DisplayChunk::DisplayChunk(int cx, int cy, int cz)
    :dcPosition(QVector3D(cx,cy,cz))
    ,blockCount(0)
    ,show(true)
    ,displayListID(GLuint(0))
{
    blocks.clear();
}

DisplayChunk::DisplayChunk(QVector3D dcPos)
    :dcPosition(dcPos)
    ,blockCount(0)
    ,show(true)
    ,displayListID(GLuint(0))
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
    setShow(true);
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
    if(displayListID!=0 && isOk() && isShow())
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

bool DisplayChunk::isShow()
{
    return show;
}

void DisplayChunk::setShow(bool s)
{
    this->show=s;
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
    int xx=g2Int(x/16.0);
    int yy=g2Int(y/16.0);
    int zz=g2Int(z/16.0);
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

///
/// \brief DisplayChunk::updateDisplayList
///
void DisplayChunk::updateDisplayList()
{
    int faceSum=0;
    QVector<Face *> faces;
    QMapIterator<int,Block*> mi(blocks);
    while(mi.hasNext()){        //计算可绘制的面数，面的消隐
        Block *temp=mi.next().value();
        if(!temp || temp->isAir()) continue;             //空气方块跳过
        for(int i=0;i<temp->faceSum();i++){
            Block *vis=getBlock(temp->vicinityPosition(i));
            if(vis==NULL){
                faces<<temp->getFace(i);
                continue;
            }
            if(temp->getType()==1){
                faces<<temp->getFace(i);
                continue;
            }
            if(temp->getId()==vis->getId())
                continue;
            if(vis->isTrans()==false)
                continue;

            faces<<temp->getFace(i);
        }
    }
    faceSum=faces.length();
    qDebug()<<"Face Sum = "<<faceSum;
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
