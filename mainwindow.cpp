#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tary = new QSystemTrayIcon(this);
    tary->setIcon(QIcon("E:/7_Other/0_lianxi/Qt/screenshot/screenshot/ico/copy.png"));
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
/*QByteArray中文转unicode*/
QString GetCorrectUnicode(const QByteArray &ba)
{
    QTextCodec::ConverterState state;
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString text = codec->toUnicode(ba.constData(), ba.size(), &state);
    if (state.invalidChars > 0)
    {
        text = QTextCodec::codecForName("GBK")->toUnicode(ba);
    }
    else
    {
        text = ba;
    }
    return text;
}

void MainWindow::ocr_resShow()
{
    QProcess *ana = new QProcess(this);
    ana->start("word.exe");

    show();
    ui->ocr_res->clear();

    QFile ocr_word("temp.txt");
    if(ocr_word.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ana->execute("word.exe");

        while(!ocr_word.atEnd())
        {
            QByteArray line = ocr_word.readLine();
            QString temp = GetCorrectUnicode(line);
            ui->ocr_res->append(temp);
        }
        ocr_word.close();
    }
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

void MainWindow::on_pushButton_clicked()
{
    QString origial = ui->ocr_res->toPlainText();

    QFile origialFile("origial.txt");
    if(origialFile.exists())
    {
       QFile::remove("origial.txt");
    }

    if(origialFile.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        origialFile.write(origial.toUtf8());
    }
    origialFile.close();

    QProcess *tarns = new QProcess(this);
    tarns->start("Translate.exe");

    ui->translate_res->clear();
    QFile trans_word("translate.txt");
    if(trans_word.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        tarns->execute("Translate.exe");

        while(!trans_word.atEnd())
        {
            QByteArray line = trans_word.readLine();
            QString temp = GetCorrectUnicode(line);

            ui->translate_res->append(temp);
        }
        trans_word.close();
    }
}
