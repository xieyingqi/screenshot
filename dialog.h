#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QScreen>
#include <QApplication>
#include <QPainter>
#include <QMouseEvent>
#include <QClipboard>
#include <QFileDialog>
#include <QGraphicsEffect>

enum NOW_STATUS{
    START,
    SELECTED,
    EDITING,
    END
};

enum DRAG_POINT{
    L_U,
    L_D,
    R_U,
    R_D,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    IN,
    OUT
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
    void show_size(int x, int y);
    void draw_selected(QRect rect);
protected:
    /*绘图事件s*/
    void paintEvent(QPaintEvent *event);
    /*鼠标事件*/
    void mouseMoveEvent(QMouseEvent *event);         //移动
    void mousePressEvent(QMouseEvent *event);        //单击
    void mouseReleaseEvent(QMouseEvent *event);      //释放
    void keyPressEvent(QKeyEvent *event);

private slots:
    void rec_creatFull();

    void on_top_clicked();

    void on_copy_clicked();

    void on_save_clicked();

    void on_edit_clicked();

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
    bool update_lock = false; //限制界面更新
    DRAG_POINT drag_point; //鼠标在哪个拖拽点
    bool drag_flag = false; //拖拽状态
    bool drag_update = false;
};



#endif // DIALOG_H
