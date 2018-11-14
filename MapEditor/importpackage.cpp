#include "importpackage.h"
#include "ui_importpackage.h"
#include <QFileDialog>
#include <QHttpMultiPart>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QTextStream>
#include <QFileInfo>
#include <QNetworkRequest>

ImportPackage::ImportPackage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportPackage)
{
    ui->setupUi(this);
}

ImportPackage::~ImportPackage()
{
    delete ui;
}

void ImportPackage::on_buttonBox_accepted()
{
    QUrl url(ui->packageURL->text());
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);


    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finishedSlot(QNetworkReply*)));


    QNetworkRequest request;
    request.setUrl(url);
    reply = manager->get(QNetworkRequest(url));

    //connect(reply, SIGNAL(finished()), this, SLOT(finished()));

    accept();
}

void ImportPackage::finishedSlot(QNetworkReply *r)
{
    r->deleteLater();
    QByteArray data = r->readAll();
    QFile other("zzz.mpk");
    other.open(QIODevice::WriteOnly);
    other.write(data);
    other.flush();
    other.close();
}

void ImportPackage::finished()
{
        ui->label->setText("pls");
        QFile other("zzz.mpk");
        other.open(QIODevice::WriteOnly);
        other.write(reply->readAll());
        other.flush();
        other.close();
}
