#include "module/panels.h"
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
    this->setZValue(0x3f3f3f3f);
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
    //    widgetProxy->show();
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

void BackPackBar::setViewPos(QPoint pos)
{
    widget->setViewPos(pos);
}

bool BackPackBar::isShow()
{
    return widgetProxy->isVisible();
}

void BackPackBar::setWorld(World *world)
{
    widget->setWorld(world);
}

void BackPackBar::setPocket(ItemBar *itemBar)
{
    widget->setPocket(itemBar);
}

//=========================
//背包的显示组件
//=========================
BackPackBarWidget::BackPackBarWidget()
    :QWidget(0)
{
    pocketThing.clear();

    for(int i=0;i<9;i++){                   //创建物品栏的9个映射格子
        ThingItemPanel *pi=new ThingItemPanel(16,this);
        pocketThing.append(pi);
        connect(pi,SIGNAL(mouseChoose()),this,SLOT(chooseItem()));
    }

    lastPageButton=new QPushButton("<",this);
    nextPageButton=new QPushButton(">",this);

    connect(lastPageButton,SIGNAL(clicked()),this,SLOT(lastPage()));
    connect(nextPageButton,SIGNAL(clicked()),this,SLOT(nextPage()));
    page=0;
    lastPageButton->setDisabled(true);

    this->setMouseTracking(true);
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

//这里要求在操作pocket时严格保证其指针不变
void BackPackBarWidget::setPocket(ItemBar *itemBar)
{
    if(itemBar==NULL){
        qWarning()<<"BackPackBarWidget::setPocket警告：错误的ItemBar类指针传递！当前指针为空";
        return ;
    }
    this->pocketBar=itemBar;
    for(int i=0;i<9;i++){
        pocketThing[i]->setItem(itemBar->getThingItem(i));
    }
    setPocketThingItem(barThing[2]->getItem(),1,0);
    emit pocketBar->thingIndexChange(pocketBar->getCurrThingID());
}

void BackPackBarWidget::setPocketThingItem(ThingItem *item,int amount,int index)
{
    if(index<0 || index>=9)
        return ;
    ThingItem *t=item;
    pocketThing[index]->setItem(t->id,t->name,t->texName,amount);
}

void BackPackBarWidget::setViewPos(QPoint pos)
{
    viewPos=pos;
}

void BackPackBarWidget::paintEvent(QPaintEvent *)
{
    QRect wRect(0,0,this->width(),this->height());
    QPainter painter(this);
    QPixmap pixmap(":/res/divinecraft/textures/ui/back_pack_bar.png");
    //    painter.drawRect(wRect);
    painter.drawPixmap(wRect,pixmap,QRect(0,0,pixmap.width(),pixmap.height()));
    painter.setPen(Qt::black);
    painter.drawText(wRect,Qt::AlignHCenter,tr("物品栏"));
    //对物品容器进行布局
    int bsize=(wRect.width()-60)/9;
    //    int bwh=bsize-5;
    int startH=wRect.height()-bsize-20;

    painter.setPen(Qt::blue);
    for(int i=0;i<9;i++){
        pocketThing[i]->setSize(bsize);
        pocketThing[i]->move(30+i*bsize,startH);
        pocketThing[i]->updateToolTip();
        painter.drawRect(QRect(30+i*bsize,startH,bsize,bsize));
    }

    startH=startH-20-(bsize*5);
    painter.setPen(Qt::red);

    for(int i=0;i<5*9;i++){
        int x=i%9;
        int y=i/9;
        int index=page*45+i;
        if(index>=barThing.length())
            break;
        barThing[index]->setVisible(true);
        barThing[index]->setSize(bsize);
        barThing[index]->move(30+x*bsize,startH+y*bsize);
        painter.drawRect(30+x*bsize,startH+y*bsize,bsize,bsize);
    }
    QRect PBRect(5,startH+bsize*2,20,bsize);
    lastPageButton->setGeometry(PBRect);
    PBRect.setRect(35+9*bsize,startH+bsize*2,20,bsize);
    nextPageButton->setGeometry(PBRect);

    flowItem->setSize(bsize);
    flowItem->move(QCursor::pos()-viewPos-this->pos()+QPoint(0,5));
}

void BackPackBarWidget::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
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
        ti->setVisible(false);
        ti->setItem(bl->id,bl->name,bl->texName[0],-1);
        ti->updateToolTip();
        barThing.append(ti);
        connect(ti,SIGNAL(mouseChoose()),this,SLOT(chooseItem()));
    }
    maxPage=barThing.length()/45+1;
    if(maxPage<=1)
        nextPageButton->setDisabled(true);

    flowItem=new ThingItemPanel(16,this);
    flowItem->setNULLItem();
}

void BackPackBarWidget::chooseItem()
{
    ThingItemPanel* item = qobject_cast<ThingItemPanel *>(sender());
    if(item->isNULL()){                                     //如果目标位置为空
        if(flowItem->isNULL()==false){              //如果悬浮item不空则填充
            item->copyItem(flowItem->getItem());
            flowItem->setNULLItem();
        }
    }
    else{
        if(item->isPick()){                         //非无限方块的处理
            ThingItem *t=flowItem->getItem();
            ThingItem *temp=new ThingItem(t->id,t->name,t->texName,t->amount);
            if(item->getItem()->id==flowItem->getItem()->id){               //相同合并
                int sum=t->amount+item->getItem()->amount;
                int sy=0;
                if(sum>=64){
                    sy=sum-64;
                    sum=64;
                }
                else{
                    sy=0;
                }
                flowItem->copyItem(item->getItem());
                flowItem->setAmount(sy);
                temp->amount=sum;
            }
            else{
                flowItem->copyItem(item->getItem());
            }
            item->copyItem(temp);
            delete temp;
            item->setToolTip(item->getItem()->name);
        }
        else{               //无限物品的处理
            if(item->getItem()->id==flowItem->getItem()->id){
                flowItem->setAmount(flowItem->getAmount()+1);
            }
            else{
                if(flowItem->isNULL()){
                    flowItem->copyItem(item->getItem());
                    flowItem->setAmount(1);
                }
                else
                    flowItem->setNULLItem();
            }
        }
    }

    emit pocketBar->thingIndexChange(pocketBar->getCurrThingID());
}

void BackPackBarWidget::lastPage()
{
    page--;
    if(page<=0){
        page=0;
        lastPageButton->setDisabled(true);
    }
    if(page<maxPage-1){
        nextPageButton->setEnabled(true);
    }
    for(int i=0;i<barThing.length();i++){               //隐藏所有的item以防止重叠
        barThing[i]->setVisible(false);
    }
}

void BackPackBarWidget::nextPage()
{
    page++;
    if(page>=maxPage-1){
        page=maxPage-1;
        nextPageButton->setDisabled(true);
    }
    if(page>0){
        lastPageButton->setEnabled(true);
    }
    for(int i=0;i<barThing.length();i++){       //隐藏所有的item以防止重叠
        barThing[i]->setVisible(false);
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
    //    if(item)
    //        delete item;
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
    item=i;
}

void ThingItemPanel::copyItem(ThingItem *i)
{
    setItem(i->id,i->name,i->texName,i->amount);
}

void ThingItemPanel::setNULLItem()
{
    if(item==NULL)
        item=new ThingItem(0,"","air.png",-1);
    else
        item->setItem(0,"","air.png",-1);
}

void ThingItemPanel::setAmount(int s)
{
    if(s>64 || s<-1)
        return ;
    item->amount=s;
}

int ThingItemPanel::getAmount()
{
    return item->amount;
}


ThingItem *ThingItemPanel::getItem()
{
    return item;
}

void ThingItemPanel::setSize(int size)
{
    tSize=size;
}

void ThingItemPanel::updateToolTip()
{
    this->setToolTip(item->name);
}

bool ThingItemPanel::isNULL()
{
    if(item==NULL || item->id==0)           //空的条件
        return true;
    return false;
}

bool ThingItemPanel::isPick()
{
    if(item->amount<0)
        return false;
    return true;
}

void ThingItemPanel::paintEvent(QPaintEvent *)
{
    this->setFixedSize(tSize,tSize);
    QRect tRect(0,0,tSize,tSize);
    QPainter painter(this);
    //    painter.setPen(Qt::blue);
    //    painter.drawRect(tRect);
    if(item->amount==0){
        this->setNULLItem();
    }
    if(item==NULL || item->id==0)            //为空不显示
        return;
    QRect texRect(tRect.x()+tRect.width()*0.1,tRect.y()+tRect.height()*0.1
                  ,tRect.width()*0.8,tRect.height()*0.8);
    QPixmap texmap(tr(":/res/divinecraft/textures/blocks/%1").arg(item->texName));                  //选取正面材质来作为图标
    painter.drawPixmap(texRect,texmap,QRect(0,0,texmap.width(),texmap.height()));

    if(item->amount>=0){            //画数量
        QFont font;
        font.setPointSize(tSize*0.3);
        painter.setPen(Qt::white);
        painter.setFont(font);
        painter.drawText(tRect,Qt::AlignBottom | Qt::AlignRight,QString::number(item->amount));
    }

    QFont font;

}

void ThingItemPanel::mousePressEvent(QMouseEvent *event)
{
    if(event->button() & Qt::LeftButton){           //按下左键等于选中当前的物品
        emit mouseChoose();
    }
}

//====================================
///
/// \brief ItemBar::ItemBar
/// \param scene
ItemBar::ItemBar(QGraphicsScene *scene)
    :QObject(scene)
{
    widget=new ItemBarWidget();
    proxyWidget=new QGraphicsProxyWidget(0);
    proxyWidget->setWidget(widget);
    scene->addItem(proxyWidget);
    connect(widget,SIGNAL(thingIndexChange(int)),this,SIGNAL(thingIndexChange(int)));
    proxyWidget->show();
}

ThingItem *ItemBar::getThingItem(int i)
{
    return widget->getThingItem(i);
}

void ItemBar::setThingItem(int id, QString table, QString texName, int amount, int index)
{
    widget->setThingItem(id,table,texName,amount,index);
}

void ItemBar::resetSIze(int sceneW,int sceneH, int h)
{
    int size=h*0.8;
    int ww=size*9+8*(h*0.1)+h*0.4;
    widget->setGeometry((sceneW-ww)/2,sceneH-h-10,ww,h);
}

int ItemBar::getCurrThingID()
{
    return widget->getCurrThingID();
}

void ItemBar::setIndex(int i)
{
    widget->setIndex(i);
}

void ItemBar::nextIndex()
{
    widget->nextIndex();
}

void ItemBar::lastIndex()
{
    widget->lastIndex();
}

void ItemBar::midBlock(BlockListNode *blockIndex)
{
    widget->midBlock(blockIndex);
}

//=========================================
//物品栏显示组件
//=========================================
ItemBarWidget::ItemBarWidget()
    :QWidget(0)
{
    this->setAttribute(Qt::WA_TranslucentBackground,true);          //背景透明
    currentIndex=0;
    for(int i=0;i<9;i++){
        ThingItemPanel *item=new ThingItemPanel(16,this);
        pocketThing.append(item);
        item->setNULLItem();                                     //设置为默认物品
    }
}

ItemBarWidget::~ItemBarWidget()
{

}

void ItemBarWidget::setThingItem(ThingItem *t, int index)
{
    if(index<0 || index>=9)
        return;
    pocketThing[index]->setItem(t);
}

void ItemBarWidget::setThingItem(int id, QString table, QString texName, int amount, int index)
{
    pocketThing[index]->setItem(id,table,texName,amount);
}

ThingItem *ItemBarWidget::currThingItem()
{
    return pocketThing[currentIndex]->getItem();
}

int ItemBarWidget::getCurrThingID()
{
    return pocketThing[currentIndex]->getItem()->id;
}

ThingItem *ItemBarWidget::getThingItem(int i)
{
    if(i<0 || i>=9)
        return NULL;
    return pocketThing[i]->getItem();
}

void ItemBarWidget::setIndex(int i)
{
    if(i<0 || i>=9)
        return;
    currentIndex=i;
    emit thingIndexChange(getCurrThingID());
}

void ItemBarWidget::nextIndex()
{
    currentIndex++;
    if(currentIndex>=9)
        currentIndex=0;
    emit thingIndexChange(getCurrThingID());
}

void ItemBarWidget::lastIndex()
{
    currentIndex--;
    if(currentIndex<0)
        currentIndex=8;
    emit thingIndexChange(getCurrThingID());
}

void ItemBarWidget::midBlock(BlockListNode *blockIndex)
{
    bool isC=false;
    for(int i=0;i<pocketThing.length();i++){
        if(pocketThing[i]->getItem()->id == blockIndex->id){                //物品栏中有对应方块，则选中
            currentIndex=i;
            isC=true;
            break;
        }
    }
    if(!isC){
        for(int i=0;i<pocketThing.length();i++){
            if(pocketThing[i]->isNULL()){                                       //物品栏中有空位置则放置在空位置
                currentIndex=i;
                break;
            }
        }
        pocketThing[currentIndex]->setItem(blockIndex->id,blockIndex->name,blockIndex->texName[0],1);
    }
    emit thingIndexChange(getCurrThingID());
}

void ItemBarWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QRect rect(0,0,this->width(),this->height());
    int size=this->height()*0.8;
    QPen backPen;
    backPen.setWidth(2);
    backPen.setColor(Qt::white);
    painter.setPen(backPen);
    painter.setBrush(QBrush(QColor(63,221,111,100)));
    painter.drawRect(rect);

    int x=this->height()*0.2;           //前空20%高度
    int y=this->height()*0.1;

    QPen selectPen;
    selectPen.setWidth(3);
    selectPen.setColor(Qt::red);
    for(int i=0;i<9;i++){
        if(currentIndex==i){
            painter.setPen(selectPen);
            painter.drawRect(x-rect.height()*0.1,0,rect.height(),rect.height());
            pocketThing[i]->setSize(rect.height());
            pocketThing[i]->move(x-rect.height()*0.1,0);
        }
        else{
            pocketThing[i]->setSize(size);
            pocketThing[i]->move(x,y);
        }
        x=x+size+rect.height()*0.1;
    }
}

