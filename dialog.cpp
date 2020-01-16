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
}

void Dialog::paintEvent(QPaintEvent *event)
{
    QPainter paint(this);
    QDesktopWidget *sys_window = QApplication::desktop();
    /*获取屏幕分辨率*/
    int s_width = sys_window->width();
    int s_height = sys_window->height();

    QRect full_rect(0,0,s_width,s_height);
    /*获取指定范围的图像，返回QPixmap类型*/
    QScreen *screen = QGuiApplication::primaryScreen();
    QPixmap img = screen->grabWindow(sys_window->winId(),0,0,s_width,s_height);
    paint.drawPixmap(0,0,img);
    /*创建遮蔽蒙版*/
    paint.fillRect(full_rect,QColor(0,0,0,50));

    if(mousePressed)
    {
        QRect select_rect(start_pos.x(),start_pos.y(),qAbs(end_pos.x()-start_pos.x()),qAbs(end_pos.y()-start_pos.y())); //当前选择区域
        QPixmap select_img = img.copy(select_rect);
        //paint.drawPixmap(select_rect.topLeft(),select_img);
        paint.drawRect(select_rect);
    }
}

void Dialog::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)   //如果左键按下
    {
        mousePressed = true;
        start_pos = event->pos();
    }
}

void Dialog::mouseMoveEvent(QMouseEvent *event)
{
    if(mousePressed)    //如果按下没有释放
    {
        end_pos = event->pos();
        update();
    }
}

void Dialog::mouseReleaseEvent(QMouseEvent *event)
{
    end_pos = event->pos();
    mousePressed = false;
}
