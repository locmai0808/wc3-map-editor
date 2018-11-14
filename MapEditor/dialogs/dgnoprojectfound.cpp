#include "dgnoprojectfound.h"
#include "ui_dgnoprojectfound.h"
#include <QDialogButtonBox>
#include <QStandardPaths>
#include <QFileDialog>
#include <QDir>

#define SAVE_PATH QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first() + "/MapEditor"

DgNoProjectFound::DgNoProjectFound(QString projectName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DgNoProjectFound)
{
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowStaysOnTopHint);

}

DgNoProjectFound::~DgNoProjectFound()
{
    delete ui;
}


void DgNoProjectFound::on_createFolder_clicked()
{
    newProjectDir = ui->projectPath->text();

    if(newProjectDir.isEmpty()) return;

    QDir dir("/" + newProjectDir);

    QDir imgDir(newProjectDir + "/img");
    QDir resDir(newProjectDir + "/res");
    QDir sndDir(newProjectDir + "/snd");
    QDir upgDir(newProjectDir + "/upg");

    dir.mkpath(".");
    imgDir.mkpath(".");
    resDir.mkpath(".");
    sndDir.mkpath(".");
    upgDir.mkpath(".");


    foreach (const QString &img, QDir(":data/default/img/").entryList() )
    {
        QFile::copy(":/data/default/img/" + img, imgDir.path() + "/" + img);
    }

    foreach (const QString &snd, QDir(":data/default/snd/").entryList() )
    {
        QFile::copy(":/data/default/snd/" + snd, sndDir.path() + "/" + snd);
    }

    QVector<QDir> directories = {imgDir,resDir,sndDir,upgDir};
    foreach( QDir dir , directories){
        foreach (const QString &img, dir.entryList() )  {

            QString path = imgDir.absolutePath() + "/" + img;
            QFile file(path);
            file.setPermissions(path,QFile::WriteOwner|QFile::WriteUser|QFile::WriteOther|QFile::ReadGroup|QFile::ReadOwner|QFile::ReadUser|QFile::ReadOther|QFile::ReadGroup|QFile::ExeGroup|QFile::ExeUser|QFile::ExeOwner|QFile::ExeOther);
        }
    }

    accept();
}

void DgNoProjectFound::on_selectProject_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    "/home",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);

    ui->projectPath->setText(dir);
}
