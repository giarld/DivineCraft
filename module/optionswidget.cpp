#include "optionswidget.h"

OptionsWidget::OptionsWidget(QWidget *parent) : QWidget(parent)
{
    this->setStyleSheet("QPushButton{"
                        "font: bold 10pt;"
                        "}");
    QLabel *mLabel=new QLabel(tr("鼠标灵敏度:"),this);
    mouseLevelLabel=new QLabel(this);
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
    renderLabel=new QLabel(this);
    okRenderChangeButton=new QPushButton(tr("确定修改"),this);
    connect(okRenderChangeButton,SIGNAL(clicked()),this,SLOT(clickOkRender()));
    connect(renderSlider,SIGNAL(valueChanged(int)),this,SLOT(renderChange(int)));
    QHBoxLayout *rLayout=new QHBoxLayout;
    renderSlider->setValue(2);
    rLayout->addWidget(rLabel);
    rLayout->addWidget(renderSlider);
    rLayout->addWidget(renderLabel);
    rLayout->addWidget(okRenderChangeButton);
    rLayout->setStretchFactor(renderSlider,10);

    continueButton=new QPushButton(tr("返回游戏"),this);
    connect(continueButton,SIGNAL(clicked()),this,SIGNAL(continueGame()));

    QVBoxLayout *mainLayout=new QVBoxLayout;
    this->setLayout(mainLayout);
    mainLayout->addLayout(mLayout);
    mainLayout->addLayout(rLayout);
    mainLayout->addStretch(20);
    mainLayout->addWidget(continueButton);

//    this->setAttribute(Qt::WA_TranslucentBackground,true);          //背景透明
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
    QRect wRect(0,0,this->width(),this->height());
    QPainter painter(this);
    QPixmap pixmap(":/res/divinecraft/textures/ui/back_pack_bar.png");
    //    painter.drawRect(wRect);
    painter.drawPixmap(wRect,pixmap,QRect(0,0,pixmap.width(),pixmap.height()));
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

