#include "mainwindow.h"
#include "ui_mainwindow.h"

#include    <QDir>
#include    <QFileDialog>
#include    <QTextStream>
#include    <QTextDocument>
#include    <QTextBlock>
#include    <QSaveFile>
#include    <QException>
#include <QMessageBox>
#include <QProcess>
QString mypath;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mypath = QApplication::applicationDirPath();
}

MainWindow::~MainWindow()
{
    delete ui;
}
QFileInfo fileInfo;
bool flag = false;
void MainWindow::on_DeleteFile_clicked()
{
    ui->textEdit->clear();
    flag = false;
}
QString curPath = QDir::currentPath();
void MainWindow::on_OpenFile_clicked()
{
    QString dlgTitle="打开一个 Phigros 谱面文件（.json 格式）";
    QString filter="Phigros 谱面文件(*.json)";
    QString aFileName=QFileDialog::getOpenFileName(this,dlgTitle,curPath,filter);

    if (aFileName.isEmpty())
        return;
    flag = true;
    fileInfo.setFile(aFileName);
    curPath = fileInfo.absolutePath();

    QString readloc = "你选中的谱面是： "+fileInfo.absoluteFilePath();
    QString outloc = "生成的节奏谱是： "+fileInfo.absolutePath()+"/"+fileInfo.baseName()+".midi";
    ui->textEdit->setPlainText(readloc);
    ui->textEdit->append(outloc);
}
void MainWindow::on_ButtonOK_clicked()
{
    if(!flag){
        QMessageBox::warning(this,"发生错误","没有选中 Phigros 谱面文件");
        return ;
    }
    QString readloc = fileInfo.absoluteFilePath();
    QString outloc = fileInfo.absolutePath()+"/"+fileInfo.baseName()+".midi";
    QFile myexec("loc.txt");
    myexec.open(QIODevice::ReadWrite|QIODevice::Text);
    QTextStream out(&myexec);
    out<<readloc<<"\n";
    out<<outloc<<"\n";
    myexec.close();

    QProcess *proc = new QProcess(this);
    proc->start("finalver.exe");
    if(!proc->waitForFinished()){
        qCritical()<<"OOP";
        QMessageBox::warning(this,"发生错误","finalver未启动");
    }else{
        flag = false;
        ui->textEdit->clear();
        QMessageBox::information(this,"已完成","您的节奏谱已经成功生成，请注意调整节奏谱的 bpm 和拍号",QMessageBox::Ok,QMessageBox::NoButton);
    }


}

