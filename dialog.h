#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QScreen>
#include <QApplication>
#include <QPainter>
#include <QMouseEvent>
#include <QClipboard>

enum NOW_STATUS{
    START,
    SELECTED,
    EDITING,
    END
};

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();
    void hide_button();
    void show_button();
    void move_button(int x,int y);
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
    int s_width,s_height;

    QDesktopWidget *sys_window;
    QPoint start_pos,end_pos;
    QPixmap img;
    QPixmap select_img;
    QScreen *screen;
    QRect full_rect;
    QRect select_rect;
    QRect p_lu,p_ld,p_ru,p_rd,p_u,p_d,p_l,p_r;
    QColor line_color;
    NOW_STATUS nowStatus;

    int select_x,select_y,select_w,select_h;    //选区的位置和大小
    int start_mx,start_my,end_mx,end_my;        //贴图时，窗口位置
    int button_x,button_y;  //功能按钮相对位置

    QPoint drag_start,drag_end;    //拖动选区位置
    bool update_lock=false; //限制界面更新
};



#endif // DIALOG_H
