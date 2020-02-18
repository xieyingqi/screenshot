#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QBuffer>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tary = new QSystemTrayIcon(this);
    tary->setIcon(QIcon("../screenshot/ico/copy.png"));
    tary->setToolTip("截图工具");
    tary->show();
    tary->showMessage("截图工具", "单击截屏",QSystemTrayIcon::Information,1000);

    connect(tary,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason)));

    connect(this,SIGNAL(send_creatFull()),&full_screen,SLOT(rec_creatFull()));
    connect(&full_screen,SIGNAL(ocr_complete()),this,SLOT(ocr_resShow()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
        case QSystemTrayIcon::Trigger:
            send_creatFull();
            break;

        default:
            break;
    }
}

void MainWindow::ocr_resShow()
{
    show(); //不加这句post请求完成的槽函数不进入

    /*请求access_token*/
    QNetworkRequest req;
    network = new QNetworkAccessManager(this);
    connect(network, SIGNAL(finished(QNetworkReply*)), this, SLOT(get_access_token(QNetworkReply*)));
    req.setUrl(QUrl("https://aip.baidubce.com/oauth/2.0/token"));
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    network->post(req,"grant_type=client_credentials&client_id=Se3MVpunmZlBQGqMdIkZWPm8&client_secret=zsx9OgPLd8tTAO6FZl3Zv1jBjNMPgx9I");
}
/*access_token请求完成后，再次post，上传图片接收结果*/
void MainWindow::get_access_token(QNetworkReply *reply)
{
     if (reply->error() == QNetworkReply::NoError)
     {
         QByteArray bytes = reply->readAll();
         //qDebug() << bytes;

         QJsonParseError parseJsonErr;
         QJsonDocument document = QJsonDocument::fromJson(bytes,&parseJsonErr);
         if(!(parseJsonErr.error == QJsonParseError::NoError))
         {
             qDebug()<<tr("解析json文件错误！");
             return;
         }
         QJsonObject jsonObject = document.object();
         access_token = jsonObject["access_token"].toString();
        /*图片base64编码*/
         QImage image("screen.jpg");
         QByteArray ba;
         QBuffer buf(&ba);
         image.save(&buf,"PNG",20);
         QByteArray hexed = ba.toBase64();
         QString imgbase64 = hexed.toPercentEncoding();
         buf.close();
        /*post请求*/
         QNetworkRequest req;
         m_network = new QNetworkAccessManager(this);
         connect(m_network, SIGNAL(finished(QNetworkReply*)), this, SLOT(get_ocr(QNetworkReply*)));
         req.setUrl(QUrl("https://aip.baidubce.com/rest/2.0/ocr/v1/accurate_basic"));
         req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
         m_network->post(req,QString("access_token=%1&image=%2").arg(access_token).arg(imgbase64).toUtf8());
     }
     else
     {
         qDebug() << reply->errorString();
     }
     reply->deleteLater();
}

void MainWindow::get_ocr(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = reply->readAll();
        //qDebug() << bytes;

        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(bytes,&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            qDebug()<<tr("解析json文件错误！");
            return;
        }
        /*json数组形式解析*/
        QJsonObject jsonObject = document.object();
        QJsonValue jsonvalue = jsonObject.value("words_result");
        QJsonArray jsonarry = jsonvalue.toArray();
        ui->ocr_res->clear();
        for(int i=0;i<jsonarry.size();i++)
        {
            //qDebug() << jsonarry.at(i).toObject().value("words").toString();
            QString temp = jsonarry.at(i).toObject().value("words").toString();
            ui->ocr_res->append(temp);
        }
    }
    else
    {
        qDebug() << reply->errorString();
    }
    reply->deleteLater();
}

void MainWindow::on_pushButton_clicked()
{
    QNetworkRequest req;
    t_network = new QNetworkAccessManager(this);
    connect(t_network, SIGNAL(finished(QNetworkReply*)), this, SLOT(trans_finished(QNetworkReply*)));

    QString t_words = ui->ocr_res->toPlainText();
    QString t_appid = "20200213000383492";
    QString t_salt = "xieyingqi";
    QString t_key = "Z2gV0Y0_wqsKYCzYnrGc";

    /*md5加密*/
    QString t_sign;
    QString pwd = t_appid + t_words + t_salt + t_key;
    QByteArray bb;
    bb = QCryptographicHash::hash(pwd.toUtf8(),QCryptographicHash::Md5);
    t_sign = bb.toHex();

    req.setUrl(QUrl("https://fanyi-api.baidu.com/api/trans/vip/translate"));
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    t_network->post(req,QString("q=%1&from=auto&to=zh&appid=%2&salt=%3&sign=%4").arg(t_words).arg(t_appid).arg(t_salt).arg(t_sign).toUtf8());
}

void MainWindow::trans_finished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = reply->readAll();
        //qDebug() << bytes;

        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(bytes,&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            qDebug()<<tr("解析json文件错误！");
            return;
        }
        QJsonObject jsonObject = document.object();
        QJsonValue jsonvalue = jsonObject.value("trans_result");
        QJsonArray jsonarry = jsonvalue.toArray();
        ui->translate_res->clear();
        for(int i=0;i<jsonarry.size();i++)
        {
            //qDebug() << jsonarry.at(i).toObject().value("dst").toString();
            QString temp = jsonarry.at(i).toObject().value("dst").toString();
            ui->translate_res->append(temp);
        }
    }
}
