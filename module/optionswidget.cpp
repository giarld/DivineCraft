#include "optionswidget.h"

OptionsWidget::OptionsWidget(QWidget *parent) : QWidget(parent)
{
    //    this->setStyleSheet("QPushButton{"
    //                        "font: bold 10pt;"
    //                        "}");
    QLabel *mLabel=new QLabel(tr("鼠标灵敏度:"),this);
    mouseLevelLabel=new QLabel(tr("1"),this);
    mouseLevelSlider=new QSlider(Qt::Horizontal,this);
    mouseLevelSlider->setRange(1,100);
    connect(mouseLevelSlider,SIGNAL(valueChanged(int)),this,SLOT(mouseLevelChange(int)));
    QHBoxLayout *mLayout=new QHBoxLayout;
    mLayout->addWidget(mLabel);
    mLayout->addWidget(mouseLevelSlider);
    mLayout->addWidget(mouseLevelLabel);
    mLayout->addStretch(5);
    mLayout->setStretchFactor(mouseLevelSlider,10);

    QLabel *rLabel=new QLabel(tr("渲染距离:"),this);
    renderSlider=new QSlider(Qt::Horizontal,this);
    renderSlider->setRange(2,32);
    renderLabel=new QLabel(tr("2"),this);
    okRenderChangeButton=new QPushButton(tr("确定修改"),this);
    connect(okRenderChangeButton,SIGNAL(clicked()),this,SLOT(clickOkRender()));
    connect(renderSlider,SIGNAL(valueChanged(int)),this,SLOT(renderChange(int)));
    QHBoxLayout *rLayout=new QHBoxLayout;
    rLayout->addWidget(rLabel);
    rLayout->addWidget(renderSlider);
    rLayout->addWidget(renderLabel);
    rLayout->addWidget(okRenderChangeButton);
    rLayout->setStretchFactor(renderSlider,10);

    continueButton=new QPushButton(tr("返回游戏"),this);
    connect(continueButton,SIGNAL(clicked()),this,SIGNAL(continueGame()));

    aboutButton=new QPushButton(tr("帮助与说明"),this);
    connect(aboutButton,SIGNAL(clicked()),this,SLOT(aboutGame()));

    quitButton=new QPushButton(tr("退出游戏"),this);
    connect(quitButton,SIGNAL(clicked()),this,SIGNAL(quitClick()));

    QVBoxLayout *mainLayout=new QVBoxLayout;
    this->setLayout(mainLayout);
    mainLayout->addLayout(mLayout);
    mainLayout->addLayout(rLayout);
    mainLayout->addStretch(20);
    mainLayout->addWidget(aboutButton);
    mainLayout->addWidget(quitButton);
    mainLayout->addWidget(continueButton);

    this->setAttribute(Qt::WA_TranslucentBackground,true);          //背景透明
}

OptionsWidget::~OptionsWidget()
{
    delete continueButton;
    delete mouseLevelSlider;
    delete mouseLevelLabel;
    delete renderSlider;
    delete renderLabel;
}

void OptionsWidget::setMouseLevel(int v)
{
    mouseLevelSlider->setValue(v);
}

void OptionsWidget::setRenderLen(int v)
{
    renderSlider->setValue(v);
}

void OptionsWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPixmap pix(":/res/divinecraft/textures/ui/back_pack_bar.png");
    painter.drawPixmap(QRect(0,0,this->width(),this->height()),pix,QRect(0,0,pix.width(),pix.height()));
}

void OptionsWidget::mouseLevelChange(int v)
{
    this->mouseLevelLabel->setText(QString::number(v));
    emit mouseLevelValueChange(v);
}

void OptionsWidget::renderChange(int v)
{
    this->renderLabel->setText(QString::number(v));
}

void OptionsWidget::clickOkRender()
{
    emit renderValueChange(renderSlider->value());
}

void OptionsWidget::aboutGame()
{
    QMessageBox::about(0,tr("DivineCraft帮助与说明"),tr("DivineCraft是一款开放式创造类沙盘游戏"
                                                   "玩家可以在游戏中随意创造与破坏。\n"
                                                   "当前版本为:%1\n"
                                                   "开放者 by Gxin (请关注DivineCraft官方贴吧)\n\n"
                                                   "操作说明:\n"
                                                   "W,S,A,D: \t前进,后退,向左,向右\n"
                                                   "M: \t切换飞行模式与重力模式\n"
                                                   "空格键: \t重力模式下跳跃,飞行模式下上升\n"
                                                   "Shift: \t飞行模式下下降\n"
                                                   "E: \t物品背包栏\n"
                                                   "1~9: \t切换物品栏当前物品\n"
                                                   "Esc: \t进入设置菜单或退出物品栏\n"
                                                   "F11: \t切换全屏\n"
                                                   "F2: \t截图(图片会被保存在screenshots目录下).").arg(QApplication::applicationVersion()));
}

