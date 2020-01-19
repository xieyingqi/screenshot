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
    showFullScreen();
    setModal(true);
    hide_button();
    nowStatus = START;
    /*获取屏幕分辨率和屏幕区域*/
    sys_window = QApplication::desktop();
    s_width = sys_window->width();
    s_height = sys_window->height();
    full_rect = QRect(0,0,s_width,s_height);
    screen = QGuiApplication::primaryScreen();
    img = screen->grabWindow(sys_window->winId(),0,0,s_width,s_height);

    setFixedSize(s_width,s_height);
    setStyleSheet("background-color: rgb(198, 198, 198);");
    move(0,0);
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
    int temp_y;

    temp_y = y > s_height-30 ? y-30 : y;

    ui->top->move(x-120,temp_y+1);
    ui->copy->move(x-90,temp_y+1);
    ui->save->move(x-60,temp_y+1);
    ui->edit->move(x-30,temp_y+1);
}

void Dialog::show_size(int x, int y)
{
    ui->label->setVisible(true);

    int temp_y;

    temp_y = y < 12 ? y+12 : y;

    ui->label->move(x+2,temp_y-12);
    ui->label->setText(QString("%1*%2").arg(select_w).arg(select_h));
    ui->label->adjustSize();
}

void Dialog::draw_selected(QRect rect)
{
    show_size(select_x,select_y);

    if(select_w && select_h)
    {
        QPainter paint(this);
        line_color = QColor(0,0,255,255);

        /*复制屏幕的当前选择部分，并显示*/
        select_img = img.copy(rect);
        paint.drawPixmap(rect.topLeft(),select_img);
        /*显示选择框*/
        paint.setPen(line_color);
        paint.drawRect(rect);

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
}

void Dialog::paintEvent(QPaintEvent *event)
{
    QPainter paint(this);
    switch (nowStatus) {
        case START: //未选择区域
            paint.drawPixmap(0,0,img);
            paint.fillRect(full_rect,QColor(0,0,0,100));

            if(mousePressed)
            {
                /*当前选择区域*/
                select_x = end_pos.x() > start_pos.x() ? start_pos.x():end_pos.x();  //根据鼠标移动方向，判断起点，若上移则变化起点
                select_y = end_pos.y() > start_pos.y() ? start_pos.y():end_pos.y();
                select_w = qAbs(end_pos.x()-start_pos.x());
                select_h = qAbs(end_pos.y()-start_pos.y());
                select_rect = QRect(select_x,select_y,select_w,select_h);

                draw_selected(select_rect);
            }
            break;
        case SELECTED:  //已选择区域
            paint.drawPixmap(0,0,img);
            paint.fillRect(full_rect,QColor(0,0,0,100));
            if(mousePressed) //拖拽选区
            {
                if(update_lock)
                {
                    /*当前选择区域*/
                    select_x = start_pos.x() + drag_end.x() - drag_start.x();
                    select_y = start_pos.y() + drag_end.y() - drag_start.y();

                    if(select_x < 0)    select_x = 0;
                    if(select_y < 0)    select_y = 0;
                    if((select_x+select_w) > s_width)   select_x = s_width - select_w;
                    if((select_y+select_h) > s_height)   select_y = s_height - select_h;

                    select_rect = QRect(select_x,select_y,select_w,select_h);

                    draw_selected(select_rect);
                }
                else if(drag_update)    //拖拽点
                {
                    /*当前选择区域*/
                    select_x = end_pos.x() > start_pos.x() ? start_pos.x():end_pos.x();  //根据鼠标移动方向，判断起点，若上移则变化起点
                    select_y = end_pos.y() > start_pos.y() ? start_pos.y():end_pos.y();
                    select_w = qAbs(end_pos.x()-start_pos.x());
                    select_h = qAbs(end_pos.y()-start_pos.y());
                    select_rect = QRect(select_x,select_y,select_w,select_h);

                    draw_selected(select_rect);
                }
            }

            break;
        case EDITING:   //编辑模式
            paint.drawPixmap(1,1,select_img);
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
                switch (drag_point) {
                    case L_U:
                        start_pos.setX(select_x+select_w);
                        start_pos.setY(select_y+select_h);
                        drag_flag = true;
                        break;
                    case L_D:
                        start_pos.setX(select_x+select_w);
                        start_pos.setY(select_y);
                        drag_flag = true;
                        break;
                    case R_U:
                        start_pos.setX(select_x);
                        start_pos.setY(select_y+select_h);
                        drag_flag = true;
                        break;
                    case R_D:
                        start_pos.setX(select_x);
                        start_pos.setY(select_y);
                        drag_flag = true;
                        break;
                    case UP:
                        start_pos.setX(select_x+select_w);
                        start_pos.setY(select_y+select_h);
                        drag_flag = true;
                        break;
                    case DOWN:
                        start_pos.setX(select_x);
                        start_pos.setY(select_y);
                        drag_flag = true;
                        break;
                    case LEFT:
                        start_pos.setX(select_x+select_w);
                        start_pos.setY(select_y+select_h);
                        drag_flag = true;
                        break;
                    case RIGHT:
                        start_pos.setX(select_x);
                        start_pos.setY(select_y);
                        drag_flag = true;
                        break;
                    case IN:
                        drag_start = event->pos();
                        drag_flag = false;
                        break;
                    case OUT:
                        nowStatus = START;
                        start_pos = event->pos();
                        drag_flag = false;
                        break;
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
                if(drag_flag == true)
                {
                    switch (drag_point)
                    {
                        case UP:
                            end_pos.setX(select_x);
                            end_pos.setY(event->pos().y());
                            drag_update = true;
                            update();
                            break;
                        case DOWN:
                            end_pos.setX(select_x+select_w);
                            end_pos.setY(event->pos().y());
                            drag_update = true;
                            update();
                            break;
                        case LEFT:
                            end_pos.setX(event->pos().x());
                            end_pos.setY(select_y);
                            drag_update = true;
                            update();
                            break;
                        case RIGHT:
                            end_pos.setX(event->pos().x());
                            end_pos.setY(select_y+select_h);
                            drag_update = true;
                            update();
                            break;
                        default:
                            end_pos = event->pos();
                            update();
                            drag_update = true;
                            break;
                    }
                }
                else
                {
                    if(drag_point == IN)
                    {
                        drag_end = event->pos();
                        update();
                        update_lock = true;
                    }
                }
                button_x = select_x + select_w;
                button_y = select_y + select_h;
                move_button(button_x,button_y);
                break;
            case EDITING:   //编辑模式
                drag_end = event->pos();
                move(end_mx+event->globalX()-start_mx,end_my+event->globalY()-start_my);    //拖动操作，移动窗口
                break;
            case END:   //中断操作，关闭此窗口，返回主窗口
                break;
        }
    }

    if(!mousePressed)
    {
        if(p_lu.contains(event->pos()))         //左上
        {
            setCursor(Qt::SizeFDiagCursor);
            drag_point = L_U;
        }
        else if(p_ld.contains(event->pos()))   //左下
        {
            setCursor(Qt::SizeBDiagCursor);
            drag_point = L_D;
        }
        else if(p_ru.contains(event->pos()))   //右上
        {
            setCursor(Qt::SizeBDiagCursor);
            drag_point = R_U;
        }
        else if(p_rd.contains(event->pos()))   //右下
        {
            setCursor(Qt::SizeFDiagCursor);
            drag_point = R_D;
        }
        else if(p_u.contains(event->pos()))   //上
        {
            setCursor(Qt::SizeVerCursor);
            drag_point = UP;
        }
        else if(p_d.contains(event->pos()))   //下
        {
            setCursor(Qt::SizeVerCursor);
            drag_point = DOWN;
        }
        else if(p_l.contains(event->pos()))   //左
        {
            setCursor(Qt::SizeHorCursor);
            drag_point = LEFT;
        }
        else if(p_r.contains(event->pos()))   //右
        {
            setCursor(Qt::SizeHorCursor);
            drag_point = RIGHT;
        }
        else if(select_rect.contains(event->pos()))
        {
            setCursor(Qt::SizeAllCursor);
            drag_point = IN;
        }
        else
        {
            setCursor(Qt::ArrowCursor);
            drag_point = OUT;
        }
    }
}

void Dialog::mouseReleaseEvent(QMouseEvent *event)
{
    mousePressed = false;
    update_lock = false;
    drag_flag = false;
    drag_update = false;
    switch (nowStatus) {
        case START: //未选择区域
            end_pos = event->pos();

            button_x = select_x + select_w;
            button_y = select_y + select_h;
            move_button(button_x,button_y);
            show_button();
            show_size(select_x,select_y);
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

void Dialog::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
    {
        close();
        nowStatus = START;
    }
}

void Dialog::on_top_clicked()
{
    setFixedSize(select_w+2,select_h+2);
    move(select_x,select_y);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    show();
    end_mx = select_x;
    end_my = select_y;

    nowStatus = EDITING;
}

void Dialog::on_copy_clicked()
{
    QClipboard *cl_board = QApplication::clipboard();
    cl_board->setPixmap(select_img);
    close();
}

void Dialog::on_save_clicked()
{
    QString file = QFileDialog::getSaveFileName(this, tr("Save Image"), ".", tr("Image Files(*.jpg)"));
    select_img.save(file,"JPG",100);
    close();
}

void Dialog::on_edit_clicked()
{

}
