#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QProcess>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lineEdit->setText("icc -O3");
    ui->plainTextEdit->setPlainText("int foo(int n)\n{\nreturn n/2;\n}\n");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked(bool checked)
{
    Q_UNUSED(checked);
    QString tmpfilename="/tmp/foo.cpp";
    QString outFilename = "/tmp/foo.S";
    saveFile(tmpfilename);
    QProcess proc;

    QStringList largs=ui->lineEdit->text().split(" ");

    QString program;
    if (largs.count()==0)
        program="g++";
    else {
        if (largs.at(0)=="icc")
            program="/opt/intel/bin/icc";
        else
            program=largs.at(0);
    }


    QStringList arguments;

    for(int i=1; i<largs.count(); ++i)
        arguments << largs.at(i);
    arguments << "-S" << "-o"<<"/tmp/foo.S" << tmpfilename;

    qDebug() << program << arguments;
    proc.start(program,arguments);
    proc.waitForFinished();
    qDebug() << proc.exitStatus()<< proc.readAllStandardError() << proc.readAllStandardError();

    loadFile(outFilename);
}

bool MainWindow::saveFile(QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << ui->plainTextEdit->toPlainText();
    QApplication::restoreOverrideCursor();


    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void MainWindow::loadFile(QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    ui->plainTextEdit2->setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();


    statusBar()->showMessage(tr("File loaded"), 2000);
}
