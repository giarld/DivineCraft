#ifndef OPTIONSWIDGET_H
#define OPTIONSWIDGET_H

#include <QtWidgets>

class OptionsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OptionsWidget(QWidget *parent = 0);
    ~OptionsWidget();
    void setMouseLevel(int v);
    void setRenderLen(int v);

protected:
    void paintEvent(QPaintEvent *);

signals:
    void continueGame();
    void mouseLevelValueChange(int v);
    void renderValueChange(int v);
private slots:
    void mouseLevelChange(int v);
    void renderChange(int v);
    void clickOkRender();
    void aboutGame();
private:
    QPushButton *aboutButton;
    QPushButton *continueButton;                //继续按钮
    QSlider *mouseLevelSlider;
    QLabel *mouseLevelLabel;
    QSlider *renderSlider;
    QLabel *renderLabel;
    QPushButton *okRenderChangeButton;
};

#endif // OPTIONSWIDGET_H
