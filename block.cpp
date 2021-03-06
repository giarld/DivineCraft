/*
 * Author:Gxin
 */
#include "block.h"

QVector3D posAOffsets[][4]={        //0类方块顶点偏移
    {//FRONT
     QVector3D(0.0f,1.0f,1.0f),QVector3D(0.0f,0.0f,1.0f),
     QVector3D(1.0f,0.0f,1.0f),QVector3D(1.0f,1.0f,1.0f)
    },
    {
        //BACK
        QVector3D(1.0f,1.0f,0.0f),QVector3D(1.0f,0.0f,0.0f),
        QVector3D(0.0f,0.0f,0.0f),QVector3D(0.0f,1.0f,0.0f)
    },
    {
        //LEFT
        QVector3D(0.0f,1.0f,0.0f),QVector3D(0.0f,0.0f,0.0f),
        QVector3D(0.0f,0.0f,1.0f),QVector3D(0.0f,1.0f,1.0f)
    },
    {
        //RIGHT
        QVector3D(1.0f,1.0f,1.0f),QVector3D(1.0f,0.0f,1.0f),
        QVector3D(1.0f,0.0f,0.0f),QVector3D(1.0f,1.0f,0.0f)
    },
    {
        //TOP
        QVector3D(1.0f,1.0f,1.0f),QVector3D(1.0f,1.0f,0.0f),
        QVector3D(0.0f,1.0f,0.0f),QVector3D(0.0f,1.0f,1.0f)
    },
    {
        //DOWN
        QVector3D(0.0f,0.0f,0.0f),QVector3D(1.0f,0.0f,0.0f),
        QVector3D(1.0f,0.0f,1.0f),QVector3D(0.0f,0.0f,1.0f)
    }
};

QVector3D posBOffsets[][4]={                //1类方块顶点偏移
    {//FRONT
     QVector3D(0.0f,1.0f,0.0f),QVector3D(0.0f,0.0f,0.0f),
     QVector3D(1.0f,0.0f,1.0f),QVector3D(1.0f,1.0f,1.0f)
    },
    {
        //BACK
        QVector3D(1.0f,1.0f,1.0f),QVector3D(1.0f,0.0f,1.0f),
        QVector3D(0.0f,0.0f,0.0f),QVector3D(0.0f,1.0f,0.0f)
    },
    {
        //LEFT
        QVector3D(0.0f,1.0f,1.0f),QVector3D(0.0f,0.0f,1.0f),
        QVector3D(1.0f,0.0f,0.0f),QVector3D(1.0f,1.0f,0.0f)
    },
    {
        //RIGHT
        QVector3D(1.0f,1.0f,0.0f),QVector3D(1.0f,0.0f,0.0f),
        QVector3D(0.0f,0.0f,1.0f),QVector3D(0.0f,1.0f,1.0f)
    },
};

QVector3D Anormals[]={              //0类方块法线
    QVector3D(0.0f,0.0f,1.0f),QVector3D(0.0f,0.0f,-1.0f),
    QVector3D(-1.0f,0.0f,0.0f),QVector3D(1.0f,0.0f,0.0f),
    QVector3D(0.0f,1.0f,0.0f),QVector3D(0.0f,-1.0f,0.0f)
};

QVector3D Bnormals[]={              //1类方块法线
    QVector3D(1.0f,0.0f,1.0f),QVector3D(-1.0,0.0,-1.0f),
    QVector3D(-1.0f,0.0f,1.0f),QVector3D(1.0,0.0,-1.0f)
};


float texSwing[4][2]={
    {0.0f,0.0f},{0.0f,1.0f},{1.0f,1.0f},{1.0f,0.0f}
};      //材质的延伸

//向四周一个单位的偏移
QVector3D vicinityOffset[]={
    QVector3D(0,0,1),QVector3D(0,0,-1),
    QVector3D(-1,0,0),QVector3D(1,0,0),
    QVector3D(0,1,0),QVector3D(0,-1,0)
};

//标准1×1×1方块类

Block::Block(QVector3D position)                            //构造一个空气方块
{
    reSetBlock(NULL,position);
}

Block::Block(QVector3D position, const BlockListNode *mb)
{
    reSetBlock(mb,position);
}

Block::~Block()
{
    foreach (Face *f, face) {
//        if(f)
            delete f;
    }

}

bool Block::isAir()
{
    if(mBlock==NULL || mBlock->id==0)
        return true;
    return false;
}

void Block::reSetBlock(const BlockListNode *mb, QVector3D position)
{
    this->position=position;
    mBlock=mb;

    bName="";
    face.clear();
    if( mb && mb->id!=0)   //不为空气方块，创建面
    {
        bName=mb->name;
        createBlock();
    }
}

void Block::createBlock()
{
    if(mBlock->type==0){
        int n=DOWN;
        for(int i=FRONT;i<=n;i++){
            Face *nf=new Face;
            for(int j=0;j<4;j++){               //每个面四个顶点
                nf->vert[j].position=posAOffsets[i][j]+position;                 //实际坐标
                nf->vert[j].normal=Anormals[i];                                              //法线
                nf->vert[j].texCoord=QVector3D(texSwing[j][0],texSwing[j][1],1.0f*mBlock->tex[i]/mBlock->texLength);   //纹理
            }
            face<<nf;
        }
    }
    else if(mBlock->type==1){
        int n=RIGHT;
        for(int i=FRONT;i<=n;i++){
            Face *nf=new Face;
            for(int j=0;j<4;j++){
                nf->vert[j].position=posBOffsets[i][j]+position;                 //实际坐标
                nf->vert[j].normal=Bnormals[i];                                              //法线
                nf->vert[j].texCoord=QVector3D(texSwing[j][0],texSwing[j][1],1.0f*mBlock->tex[i]/mBlock->texLength);   //纹理
            }
            face<<nf;
        }
    }
}
QVector3D Block::getPosition() const
{
    return position;
}

void Block::setPosition(const QVector3D &value)
{
    position = value;
}

QVector3D Block::vicinityPosition(QVector3D centerPos, int site)
{
    if(site<Block::FRONT || site>=Block::MAX_FACE_SUM){
        return centerPos;
    }
    return centerPos+vicinityOffset[site];
}

QString Block::getBName() const
{
    return bName;
}

void Block::setBName(const QString &name)
{
    bName = name;
}

Face *Block::getFace(int site)
{
    if(site<Block::FRONT || site>=face.length()) return NULL;
    return face[site];
}

bool Block::isCollide()
{
    return mBlock->collide;
}

bool Block::isTrans()
{
    return mBlock->trans;
}

bool Block::doNotHideFace()
{
    return mBlock->dnotHideFace;
}

int Block::faceSum()
{
    return face.length();
}


int Block::getId()
{
    return mBlock->id;
}

int Block::getType()
{
    return mBlock->type;
}

