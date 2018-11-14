#include "newfile.h"
#include "ui_newfile.h"
#include "QDir"
#include "QString"
#include "QFile"
#include <QStandardPaths>
#include <QDebug>

#define SAVE_PATH QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first() + "/MapEditor"

NewFile::NewFile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewFile)
{
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowStaysOnTopHint);
}

NewFile::~NewFile()
{
    delete ui;
}

void NewFile::on_newFileName_textEdited(const QString &arg1)
{

    QString newProjectName = arg1;

    QDir dir(SAVE_PATH + "/" + newProjectName);

    if (dir.exists())
    {
        ui->validStatus->setText("Name invalid or project folder exists");
        ui->accept->setDisabled(true);
    }
    else
    {
        ui->validStatus->setText("Folder valid to create");
        ui->accept->setEnabled(true);
    }
}

void NewFile::on_accept_accepted()
{
    newProjectName = ui->newFileName->text();

    QDir dir(SAVE_PATH + "/" + newProjectName);

    QDir imgDir(SAVE_PATH + "/" + newProjectName + "/img");
    QDir resDir(SAVE_PATH + "/" + newProjectName + "/res");
    QDir sndDir(SAVE_PATH + "/" + newProjectName + "/snd");
    QDir upgDir(SAVE_PATH + "/" + newProjectName + "/upg");

    dir.mkpath(".");
    imgDir.mkpath(".");
    resDir.mkpath(".");
    sndDir.mkpath(".");
    upgDir.mkpath(".");

    QVector<QDir> dirs = { imgDir , resDir , sndDir ,upgDir};
    QStringList dirNames = {":data/default/img/" , ":data/default/res/",":data/default/snd/",":data/default/upg/"};
    for( int i = 0; i < dirs.length(); i++){

        foreach( const QString &file, QDir(dirNames.at(i)).entryList()){
            QString d = dirNames.at(i);
            QFileInfo fileInfo( d + file);
            if( fileInfo.isDir()){
                QDir folder(dirNames.at(i) + file);
                QStringList files = folder.entryList();
                foreach( const QString &subFile , files){
                    QDir dir;
                    dir.mkdir(dirs.at(i).path() + "/" + file);
                    QFile::copy( dirNames.at(i) + file + "/" + subFile, dirs.at(i).path() + "/" + file + "/" + subFile );
                    QFile setFile(dirs.at(i).path() + "/" + file + "/" + subFile);
                    setFile.setPermissions(QFile::WriteOwner|QFile::WriteUser|QFile::WriteOther|QFile::ReadOwner|
                                           QFile::ReadOther|QFile::ReadUser|QFile::ReadGroup|QFile::ExeUser|QFile::ExeOwner|QFile::ExeGroup);

                }
            }
            else{

                QFile::copy(dirNames.at(i) + file, dirs.at(i).path() + "/"+ file);
                QFile setFile(dirs.at(i).path() + "/" + file);
                setFile.setPermissions(QFile::WriteOwner|QFile::WriteUser|QFile::WriteOther|QFile::ReadOwner|QFile::ReadOther|QFile::ReadUser);
            }
        }

    }


    QFile::copy(":/data/default/snd/generalsoundfont.sf2",sndDir.path() + "/generalsoundfont.sf2");

//             QString path = imgDir.absolutePath() + "/" + img;
//             QFile file(path);
//             file.setPermissions(path,QFile::WriteOwner|QFile::WriteUser|QFile::WriteOther|QFile::ReadGroup|QFile::ReadOwner|QFile::ReadUser|QFile::ReadOther|QFile::ReadGroup|QFile::ExeGroup|QFile::ExeUser|QFile::ExeOwner|QFile::ExeOther);
//         }
//    }
    emit changeProjectName(dir.path());
}

bool NewFile::copy_dir_recursive(QString from_dir, QString to_dir, bool replace_on_conflit)
{

}

void NewFile::reject()
{
    return;
}
