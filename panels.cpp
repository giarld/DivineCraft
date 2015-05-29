#include <panels.h>
#include "gmath.h"
#include "world.h"
#include "block.h"

//==============================
//      数据面板
//==============================
DataPanel::DataPanel(int x, int y, int w, int z)
{
    rect=QRectF(x,y,w,z);
    fps=0.0f;
    mEyePosition=mPosition=QVector3D(0,0,0);
}

QRectF DataPanel::boundingRect() const
{
    return rect;
}

void DataPanel::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QColor color=QColor(0,0,0,50);
    painter->setPen(color);
    QBrush brush=QBrush(color);
    painter->setBrush(brush);
    painter->drawRect(boundingRect());
    painter->setPen(Qt::white);
    QRectF fpsRect(5,0,boundingRect().width()/2,20);
    QString fpsStr="FPS : "+QString::number(fps);
    painter->drawText(fpsRect,fpsStr);

    fpsRect=QRectF(boundingRect().width()/2+5,0,boundingRect().width()/2,20);
    QString rStr=QObject::tr("Radius : %1").arg(displayRadius);
    painter->drawText(fpsRect,rStr);

    QRectF posRect(5,20,100,80);
    QString posStr=QObject::tr("pos: \nX: %1\nY: %2\nZ: %3").arg(mPosition.x()).arg(mPosition.y()).arg(mPosition.z());
    painter->drawText(posRect,posStr);

    QRectF eyeRect(105,20,100,80);
    QString eyeStr=QObject::tr("pos: \nX: %1\nY: %2\nZ: %3").arg(mEyePosition.x()).arg(mEyePosition.y()).arg(mEyePosition.z());
    painter->drawText(eyeRect,eyeStr);
}

void DataPanel::setFps(int f)
{
    this->fps=f;
}

void DataPanel::setPosition(const QVector3D &pos, const QVector3D &ePos)
{
    this->mPosition=pos;
    this->mEyePosition=ePos;
}

void DataPanel::setDisplayRadius(int r)
{
    displayRadius=r;
}

//===========================
//消息提示面板,提供场景上方的消息提示
//===========================

MessagePanel::MessagePanel()
{
    gMessage=NULL;
    //    this->hide();
}

void MessagePanel::showMessage(GameMessage *message, QGraphicsScene *s)
{
    gMessage=message;
    if(gMessage==NULL){
        this->hide();
        return;
    }
    if(gMessage->showTime<=0){
        this->hide();
        return ;
    }
    //计算消息栏的大小位置
    font.setPointSize(gMessage->textSize);
    QFontMetricsF metrics(font);
    int texW=metrics.width(gMessage->text);
    int texH=metrics.height();
    int w=s->width()*0.25;

    //格式化文本
    int dwTsize=texW/gMessage->text.length();//单个字所占宽度
    int dwLen=(w*0.9)/dwTsize;              //每行字数
    QString str="";
    int j=0;
    int cw=0;
    for(int i=0;i<gMessage->text.length();i++){
        if(gMessage->text[i]=='\n'){
            j=0;
            cw++;
        }
        str.append(gMessage->text[i]);
        j++;
        if(j==dwLen && i<gMessage->text.length()-1 && gMessage->text[i]!='\n'){
            j=0;
            cw++;
            str.append("\n");
        }
    }
    int h=w*0.1+texH*(cw+1);            //计算高度
    if(h>s->height()*0.3)
        h=s->height()*0.3;
    rect=QRectF((s->width()-w)/2,30,w,h);
    gMessage->text=str;
    beginTime=QTime::currentTime();
    alpha=255;
    this->show();
}

QRectF MessagePanel::boundingRect() const
{
    return rect;
}

void MessagePanel::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(gMessage==NULL){
        this->hide();
        return;
    }

    painter->setPen(QColor(0,255,0,alpha));
    painter->setBrush(QBrush(QColor(0,255,0,alpha)));

    QPixmap pixmap(":/res/divinecraft/textures/blocks/brick.png");

    //    painter->drawRect(rect);
    painter->drawPixmap(rect,pixmap,QRectF(0,0,pixmap.width(),pixmap.height()));
    painter->setPen(QColor(gMessage->textColor.red()
                           ,gMessage->textColor.green(),gMessage->textColor.blue(),alpha));

    painter->setFont(font);
    QRectF texRect(rect.x()+rect.width()*0.05,rect.y()+rect.height()*0.05,rect.width()*0.9,rect.height()*0.9);
    painter->drawText(texRect,Qt::AlignCenter ,gMessage->text);

    if(beginTime.secsTo(QTime::currentTime())>=gMessage->showTime){
        if(alpha>0){                    //消失动画
            alpha-=5;
            if(alpha<0)
                alpha=0;
        }
        else{
            this->hide();
            deleteMessage();
        }
    }
}

void MessagePanel::deleteMessage()
{
    if(gMessage)
        delete gMessage;
}


//============================
//背包栏
//============================
BackPackBar::BackPackBar(QGraphicsScene *scene)
    :QObject(scene)
{
    rect=QRect(0,0,20,10);
    widgetProxy=new QGraphicsProxyWidget(0);
    widget=new BackPackBarWidget();
    widgetProxy->setWidget(widget);
    widget->setGeometry(100,100,100,200);
    widgetProxy->show();
    qobject_cast<QGraphicsScene *>(parent())->addItem(widgetProxy);
}

BackPackBar::~BackPackBar()
{
    //item不需要主动回收Scene会回收所有的Item
}

void BackPackBar::show()
{
    //    widgetProxy->setVisible(true);
    widgetProxy->show();
}

void BackPackBar::hide()
{
    //    widgetProxy->setVisible(false);
    widgetProxy->hide();
}

void BackPackBar::setGeometry(int x, int y, int w, int h)
{
    rect=QRect(x,y,w,h);
    widget->setGeometry(rect);
}

bool BackPackBar::isShow()
{
    return widgetProxy->isVisible();
}

void BackPackBar::setWorld(World *world)
{
    widget->setWorld(world);
}

//=========================
//背包的显示组件
//=========================
BackPackBarWidget::BackPackBarWidget()
    :QWidget(0)
{
    pocketThing.clear();
    //    for(int i=0;i<5*9;i++){                                         //创建49个物品容器对象
    //        ThingItemPanel * ti=new ThingItemPanel(16,this);
    //        barThing.append(ti);
    //    }

    for(int i=0;i<9;i++){
        ThingItemPanel *pi=new ThingItemPanel(16,this);
        pocketThing.append(pi);
    }

    this->setAttribute(Qt::WA_TranslucentBackground,true);          //背景透明
}

BackPackBarWidget::~BackPackBarWidget()
{
    foreach (ThingItemPanel *t, barThing) {
        if(t)
            delete t;
    }

    foreach (ThingItemPanel *t, pocketThing) {
        if(t)
            delete t;
    }
}

void BackPackBarWidget::setWorld(World *world)
{
    if(world==NULL){
        qWarning(" BackPackBarWidget::setWorld警告：错误的World类指针传递！当前指针为空。");
    }
    myWorld=world;
    initBar();
}

void BackPackBarWidget::paintEvent(QPaintEvent *)
{
    QRect wRect(0,0,this->width(),this->height());
    QPainter painter(this);
    //对物品容器进行布局
    int bsize=(wRect.width()-40)/9;
    int bwh=bsize-5;
    int startH=wRect.height()-bsize-20;
    painter.setPen(Qt::blue);
    for(int i=0;i<9;i++){
        pocketThing[i]->setSize(bwh);
        pocketThing[i]->move(20+i*bsize,startH);
        painter.drawRect(QRect(20+i*bsize,startH,bwh,bwh));
    }
    startH=startH-20-(bsize*5);
    painter.setPen(Qt::red);
    for(int i=0;i<barThing.length();i++){
        int x=i%9;
        int y=i/9;
        barThing[i]->setSize(bwh);
        barThing[i]->move(20+x*bsize,startH+y*bsize);
        painter.drawRect(20+x*bsize,startH+y*bsize,bwh,bwh);
    }


    QPen pen;
    pen.setColor(Qt::red);
    painter.setPen(pen);
    painter.drawRect(wRect);
}

void BackPackBarWidget::initBar()
{
    barThing.clear();
    int k=1;
    for(;;k++){
        BlockListNode *bl=myWorld->getBlockIndex(k);
        if(bl->id==0)
            break;
        ThingItemPanel * ti=new ThingItemPanel(16,this);
        ti->setItem(bl->id,bl->name,bl->texName[0],-1);
        barThing.append(ti);
    }
}

//===========================
//   物品单元格
//===========================
ThingItemPanel::ThingItemPanel(int size, QWidget *parent)
    :QWidget(parent)
    ,tSize(size)
{
    item=NULL;
}

ThingItemPanel::~ThingItemPanel()
{
    if(item)
        delete item;
}

void ThingItemPanel::setItem(int id, QString table, QString texName, int amount)
{
    if(item==NULL){
        item=new ThingItem(id,table,texName,amount);
    }
    else{
        item->setItem(id,table,texName,amount);
    }
}

void ThingItemPanel::setItem(ThingItem *i)
{
    if(item==NULL){
        item=new ThingItem(i->id,i->name,i->texName,i->amount);
    }
    else{
        item->setItem(i->id,i->name,i->texName,i->amount);
    }
}

void ThingItemPanel::setSize(int size)
{
    tSize=size;
}

void ThingItemPanel::paintEvent(QPaintEvent *)
{
    this->setFixedSize(tSize,tSize);
    QRect tRect(0,0,tSize,tSize);
    QPainter painter(this);
//    painter.setPen(Qt::blue);
//    painter.drawRect(tRect);
    if(item==NULL)
        return;
    QRect texRect(tRect.x()+tRect.width()*0.1,tRect.y()+tRect.height()*0.1
                  ,tRect.width()*0.8,tRect.height()*0.8);
    QPixmap texmap(tr(":/res/divinecraft/textures/blocks/%1").arg(item->texName));
    painter.drawPixmap(texRect,texmap,QRect(0,0,texmap.width(),texmap.height()));
    painter.setPen(Qt::white);
    if(item->amount>=0)
        painter.drawText(tRect,QString::number(item->amount));
}
