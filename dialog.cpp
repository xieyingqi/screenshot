#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::rec_creatFull()
{
    this->showFullScreen();
    hide_button();
    nowStatus = START;
    /*获取屏幕分辨率和屏幕区域*/
    sys_window = QApplication::desktop();
    s_width = sys_window->width();
    s_height = sys_window->height();
    full_rect = QRect(0,0,s_width,s_height);
    screen = QGuiApplication::primaryScreen();
    img = screen->grabWindow(sys_window->winId(),0,0,s_width,s_height);
}

void Dialog::hide_button()
{
    ui->top->setVisible(false);
    ui->copy->setVisible(false);
    ui->save->setVisible(false);
    ui->edit->setVisible(false);
}

void Dialog::show_button()
{
    ui->top->setVisible(true);
    ui->copy->setVisible(true);
    ui->save->setVisible(true);
    ui->edit->setVisible(true);
}

void Dialog::move_button(int x, int y)
{
    ui->top->move(x-120,y+1);
    ui->copy->move(x-90,y+1);
    ui->save->move(x-60,y+1);
    ui->edit->move(x-30,y+1);
}

void Dialog::paintEvent(QPaintEvent *event)
{
    QPainter paint(this);
    line_color = QColor(0,0,255,255);
    switch (nowStatus) {
        case START: //未选择区域
            paint.drawPixmap(0,0,img);
            paint.fillRect(full_rect,QColor(0,0,0,50));

            if(mousePressed)
            {
                /*当前选择区域*/
                select_x = end_pos.x() > start_pos.x() ? start_pos.x():end_pos.x();  //根据鼠标移动方向，判断起点，若上移则变化起点
                select_y = end_pos.y() > start_pos.y() ? start_pos.y():end_pos.y();
                select_w = qAbs(end_pos.x()-start_pos.x());
                select_h = qAbs(end_pos.y()-start_pos.y());
                select_rect = QRect(select_x,select_y,select_w,select_h);
                /*复制屏幕的当前选择部分，并显示*/
                select_img = img.copy(select_rect);
                paint.drawPixmap(select_rect.topLeft(),select_img);
                /*显示选择框*/
                paint.setPen(line_color);
                paint.drawRect(select_rect);

                p_lu = QRect(select_x-2,select_y-2,5,5);
                p_ld = QRect(select_x-2,select_y+select_h-2,5,5);
                p_ru = QRect(select_x+select_w-2,select_y-2,5,5);
                p_rd = QRect(select_x+select_w-2,select_y+select_h-2,5,5);

                p_u = QRect(select_x+select_w/2-2,select_y-2,5,5);
                p_d = QRect(select_x+select_w/2-2,select_y+select_h-2,5,5);
                p_l = QRect(select_x-2,select_y+select_h/2-2,5,5);
                p_r = QRect(select_x+select_w-2,select_y+select_h/2-2,5,5);

                paint.fillRect(p_lu,line_color);
                paint.fillRect(p_ru,line_color);
                paint.fillRect(p_ld,line_color);
                paint.fillRect(p_rd,line_color);

                paint.fillRect(p_u,line_color);
                paint.fillRect(p_d,line_color);
                paint.fillRect(p_l,line_color);
                paint.fillRect(p_r,line_color);
            }
            break;
        case SELECTED:  //已选择区域
            paint.drawPixmap(0,0,img);
            paint.fillRect(full_rect,QColor(0,0,0,50));
            if(mousePressed && update_lock)
            {
                /*当前选择区域*/
                select_x = start_pos.x() + drag_end.x() - drag_start.x();
                select_y = start_pos.y() + drag_end.y() - drag_start.y();
                select_rect = QRect(select_x,select_y,select_w,select_h);
                /*复制屏幕的当前选择部分，并显示*/
                select_img = img.copy(select_rect);
                paint.drawPixmap(select_rect.topLeft(),select_img);
                /*显示选择框*/
                paint.setPen(line_color);
                paint.drawRect(select_rect);
            }
            break;
        case EDITING:   //编辑模式
            paint.drawPixmap(0,0,select_img);
            break;
        case END:   //中断操作，关闭此窗口，返回主窗口
            break;
    }
}

void Dialog::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)   //如果左键按下
    {
        mousePressed = true;

        switch (nowStatus) {
            case START: //未选择区域
                start_pos = event->pos();
                break;
            case SELECTED:  //已选择区域
                if((event->pos().x() > select_x) && (event->pos().x() < select_x + select_w) && (event->pos().y() > select_y) && (event->pos().y() < select_y + select_h))  //鼠标点击在区域内
                {
                    drag_start = event->pos();
                }
                else
                {
                    nowStatus = START;
                    start_pos = event->pos();
                }
                break;
            case EDITING:   //编辑模式
                start_mx = event->globalX();    //拖动起始坐标
                start_my = event->globalY();
                break;
            case END:   //中断操作，关闭此窗口，返回主窗口
                break;
        }
    }
}

void Dialog::mouseMoveEvent(QMouseEvent *event)
{
    if(mousePressed)    //如果按下没有释放
    {
        switch (nowStatus) {
            case START: //未选择区域
                end_pos = event->pos();
                update();
                break;
            case SELECTED:  //已选择区域
                drag_end = event->pos();
                if((event->pos().x() > select_x) && (event->pos().x() < select_x + select_w) && (event->pos().y() > select_y) && (event->pos().y() < select_y + select_h))  //鼠标点击在区域内
                {
                    drag_end = event->pos();
                    update();
                    update_lock = true;

                    button_x = select_x + select_w;
                    button_y = select_y + select_h;
                    move_button(button_x,button_y);
                }
                else
                {

                }
                break;
            case EDITING:   //编辑模式
                move(end_mx+event->globalX()-start_mx,end_my+event->globalY()-start_my);    //拖动操作，移动窗口
                break;
            case END:   //中断操作，关闭此窗口，返回主窗口
                break;
        }
    }

    if(p_lu.contains(event->pos()))         //左上
    {
        setCursor(Qt::SizeFDiagCursor);
    }
    else if(p_ld.contains(event->pos()))   //左下
    {
        setCursor(Qt::SizeBDiagCursor);
    }
    else if(p_ru.contains(event->pos()))   //右上
    {
        setCursor(Qt::SizeBDiagCursor);
    }
    else if(p_rd.contains(event->pos()))   //右下
    {
        setCursor(Qt::SizeFDiagCursor);
    }
    else if(p_u.contains(event->pos()))   //上
    {
        setCursor(Qt::SizeVerCursor);
    }
    else if(p_d.contains(event->pos()))   //下
    {
        setCursor(Qt::SizeVerCursor);
    }
    else if(p_l.contains(event->pos()))   //左
    {
        setCursor(Qt::SizeHorCursor);
    }
    else if(p_r.contains(event->pos()))   //右
    {
        setCursor(Qt::SizeHorCursor);
    }
    else if(select_rect.contains(event->pos()))
    {
        setCursor(Qt::SizeAllCursor);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
    }
}

void Dialog::mouseReleaseEvent(QMouseEvent *event)
{
    mousePressed = false;
    update_lock = false;
    switch (nowStatus) {
        case START: //未选择区域
            end_pos = event->pos();

/*以下内容加到贴图功能中*/
//            setFixedSize(select_w,select_h);
//            move(select_x,select_y);
//            setWindowFlag(Qt::FramelessWindowHint);
//            setWindowFlag(Qt::WindowStaysOnTopHint);
//            show();
//            end_mx = select_x;
//            end_my = select_y;

            button_x = select_x + select_w;
            button_y = select_y + select_h;
            move_button(button_x,button_y);
            show_button();
            nowStatus = SELECTED;
            break;
        case SELECTED:  //已选择区域
            start_pos.setX(select_x);
            start_pos.setY(select_y);
            break;
        case EDITING:   //编辑模式
            end_mx = end_mx+event->globalX()-start_mx;  //将拖动释放后的坐标记录
            end_my = end_my+event->globalY()-start_my;
            break;
        case END:   //中断操作，关闭此窗口，返回主窗口
            break;
    }
}
