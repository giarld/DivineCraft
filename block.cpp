#include "block.h"

QVector3D posAOffsets[][4]={
    {//FRONT
     QVector3D(0.0,1.0,0.0),QVector3D(0.0,0.0,0.0),
     QVector3D(1.0,0.0,0.0),QVector3D(1.0,1.0,0.0)
    },
    {
        //BACK
        QVector3D(1.0,1.0,-1.0),QVector3D(1.0,0.0,-1.0),
        QVector3D(0.0,0.0,-1.0),QVector3D(0.0,1.0,-1.0)
    },
    {
        //LEFT
        QVector3D(0.0,1.0,-1.0),QVector3D(0.0,0.0,-1.0),
        QVector3D(0.0,0.0,0.0),QVector3D(0.0,1.0,0.0)
    },
    {
        //RIGHT
        QVector3D(1.0,1.0,0.0),QVector3D(1.0,0.0,0.0),
        QVector3D(1.0,0.0,-1.0),QVector3D(1.0,1.0,-1.0)
    },
    {
        //TOP
        QVector3D(0.0,1.0,-1.0),QVector3D(0.0,1.0,0.0),
        QVector3D(1.0,1.0,0.0),QVector3D(1.0,1.0,-1.0)
    },
    {
        //DOWN
        QVector3D(0.0,0.0,0.0),QVector3D(0.0,0.0,-1.0),
        QVector3D(1.0,0.0,-1.0),QVector3D(1.0,0.0,0.0)
    }
};

QVector3D posBOffsets[][4]={
    {//FRONT
     QVector3D(0.0,1.0,0.0),QVector3D(0.0,0.0,0.0),
     QVector3D(1.0,0.0,-1.0),QVector3D(1.0,1.0,-1.0)
    },
    {
        //BACK
        QVector3D(1.0,1.0,-1.0),QVector3D(1.0,0.0,-1.0),
        QVector3D(0.0,0.0,0.0),QVector3D(0.0,1.0,0.0)
    },
    {
        //LEFT
        QVector3D(0.0,1.0,-1.0),QVector3D(0.0,0.0,-1.0),
        QVector3D(1.0,0.0,0.0),QVector3D(1.0,1.0,0.0)
    },
    {
        //RIGHT
        QVector3D(1.0,1.0,0.0),QVector3D(1.0,0.0,0.0),
        QVector3D(0.0,0.0,-1.0),QVector3D(0.0,1.0,-1.0)
    },
};

QVector3D Anormals[]={
    QVector3D(0.0,0.0,1.0),QVector3D(0.0,0.0,-1.0),
    QVector3D(-1.0,0.0,0.0),QVector3D(1.0,0.0,0.0),
    QVector3D(0.0,1.0,0.0),QVector3D(0.0,-1.0,0.0)
};

QVector3D Bnormals[]={
    QVector3D(1.0,0.0,1.0),QVector3D(-1.0,0.0,-1.0),
    QVector3D(-1.0,0.0,1.0),QVector3D(1.0,0.0,-1.0)
};


float texSwing[4][2]={
    {0.0,0.0},{0.0,1.0},{1.0,1.0},{1.0,0.0}
};      //材质的延伸

float texRevise[4][2]={
    {0.001,0.001},{0.001,-0.001},{-0.001,-0.001},{-0.001,0.001}
};              //材质的修正

//标准1×1×1方块类

Block::Block(QVector3D position)                            //构造一个空气方块
    :position(position)
    ,mBlock(NULL)
{
    for(int i=FRONT;i<=DOWN;i++)
    {
        brothers<<NULL;
    }
}

Block::Block(const BlockListNode *mb, QVector3D position)
    :position(position)
    ,mBlock(mb)
{
    reSetBlock(mb,position);
}

Block::~Block()
{
    foreach (Face *f, face) {
        if(f)
            delete f;
    }
    foreach (Block *b, brothers) {
        if(b)
            delete b;
    }
}

void Block::reSetBlock(const BlockListNode *mb, QVector3D position)
{
    for(int i=FRONT;i<=DOWN;i++)
    {
        brothers<<NULL;
    }
    if(mb->id!=0)   //不为空气方块，创建面
        createBlock();
}

void Block::createBlock()
{
    face.clear();
    if(mBlock->type==0){
        int n=DOWN;
        for(int i=FRONT;i<=n;i++){
            Face *nf=new Face;
            nf->ishide=false;
            for(int j=0;j<4;j++){
                nf->vert[j].position=posAOffsets[i][j]+position;                 //实际坐标
                nf->vert[j].normal=Anormals[i];                                              //法线
                float uw=mBlock->texWidth;
                float vh=mBlock->texHeight;
                nf->vert[j].texCoord=QVector2D(mBlock->tex[i].x()/uw+(1.0/uw)*texSwing[j][0]+texRevise[j][0],
                        mBlock->tex[i].y()/vh+(1.0/vh)*texSwing[j][1]+texRevise[j][1]);                     //纹理
            }
            face<<nf;
        }
    }
    else if(mBlock->type==1){
        int n=RIGHT;
        for(int i=FRONT;i<=n;i++){
            Face *nf=new Face;
            nf->ishide=false;
            for(int j=0;j<4;j++){
                nf->vert[j].position=posBOffsets[i][j]+position;                 //实际坐标
                nf->vert[j].normal=Bnormals[i];                                              //法线
                float uw=mBlock->texWidth;
                float vh=mBlock->texHeight;
                nf->vert[j].texCoord=QVector2D(mBlock->tex[i].x()/uw+(1.0/uw)*texSwing[j][0]+texRevise[j][0],
                        mBlock->tex[i].y()/vh+(1.0/vh)*texSwing[j][1]+texRevise[j][1]);                     //纹理
            }
            face<<nf;
        }
    }
}

Face *Block::getFace(int site)
{
    if(site<Block::FRONT || site>=face.length()) return NULL;
    return face[site];
}

bool Block::setBrother(int site, Block *b)
{
    if(site<Block::FRONT || site>=face.length())return false;
    brothers[site]=b;
    updateFace();
    return true;
}

bool Block::removeBrother(int site)
{
    if(site<Block::FRONT || site>=face.length())return false;
    if(!brothers[site]) return false;
    brothers[site]=NULL;
    updateFace();
    return true;
}

void Block::updateFace()
{
    if(mBlock->type==1 || mBlock==NULL || mBlock->id==0)            //自己是一个1类的四面方块或者是空气方块不更新面的隐藏
        return;
    showAll();                                                                                  //先显示全部再逐一隐藏
    for(int i=0;i<face.length();i++){
        if(!brothers[i]) continue;                                  //没有方块，跳过
        if(brothers[i]->getType()==1)                           //对面是个1类方块，跳过
            continue;
        if( brothers[i]->isTrans()==false){           //不是透明方块 则隐藏这个面
            hideFace(i);
        }
        else if(getId()==brothers[i]->getId()){        //都是透明方块，隐藏面
            hideFace(i);
        }
    }
}

void Block::hideFace(int site, bool hide)
{
    if(site < Block::FRONT || site>=face.length()) return ;
    face[site]->ishide=hide;
}

void Block::hideAll()
{
    foreach (Face *f, face) {
        f->ishide=true;
    }
}

void Block::showAll()
{
    foreach (Face *f, face) {
        f->ishide=false;
    }
}

void Block::showFace(int site)
{
    hideFace(site,false);
}

bool Block::isFaceHide(int site)
{
    if(site<Block::FRONT || site>=face.length()) return true;
    return face[site]->ishide;
}

bool Block::isCollide()
{
    return mBlock->collide;
}

bool Block::isTrans()
{
    return mBlock->trans;
}

int Block::faceSum()
{
    return face.length();
}

int Block::getShowFaceSum()
{
    int i,sum=0;
    for(i=0;i<face.length();i++)
        if(!face[i]->ishide) sum++;
    return sum;
}

int Block::getId()
{
    return mBlock->id;
}

int Block::getType()
{
    return mBlock->type;
}

