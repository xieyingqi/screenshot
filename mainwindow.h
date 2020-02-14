#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QProcess>
#include <QTextCodec>
#include "dialog.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);
    void ocr_resShow(void);
    void on_pushButton_clicked();

signals:
    void send_creatFull();

private:
    Ui::MainWindow *ui;
    Dialog full_screen;
    QSystemTrayIcon *tary;
};
#endif // MAINWINDOW_H
