#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QScreen>
#include <QApplication>
#include <QPainter>
#include <QMouseEvent>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

protected:
    /*绘图事件s*/
    void paintEvent(QPaintEvent *event);
    /*鼠标事件*/
    void mouseMoveEvent(QMouseEvent *event);         //移动
    void mousePressEvent(QMouseEvent *event);        //单击
    void mouseReleaseEvent(QMouseEvent *event);      //释放

private slots:
    void rec_creatFull();

private:
    Ui::Dialog *ui;

    bool mousePressed = false;
    QPoint start_pos,end_pos;
};

#endif // DIALOG_H
