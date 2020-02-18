#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QCryptographicHash>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
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
    void get_access_token(QNetworkReply *reply);
    void get_ocr(QNetworkReply *reply);
    void trans_finished(QNetworkReply *reply);

signals:
    void send_creatFull();

private:
    Ui::MainWindow *ui;
    Dialog full_screen;
    QSystemTrayIcon *tary;
    QNetworkAccessManager *network;
    QNetworkAccessManager *m_network;
    QNetworkAccessManager *t_network;
    QString access_token;
};
#endif // MAINWINDOW_H
