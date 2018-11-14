#include "dgimportpkg.h"
#include "ui_dgimportpkg.h"
#include "quazip/JlCompress.h"

#include "quazip/quazip.h"
#include "quazip/quazipfile.h"
#include "QSettings"
#include "QFileDialog"

DgImportPkg::DgImportPkg(QString projName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DgImportPkg)
{
    curProjectName = projName;
    ui->setupUi(this);
}

DgImportPkg::~DgImportPkg()
{
    delete ui;
}

void DgImportPkg::on_selectPkg_clicked()
{
    QSettings settings;
    QString curPath = settings.value("file/pwd", QDir::homePath()).toString();

    QFileDialog dialog(this);
    dialog.setDirectory(curPath);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setNameFilter(tr("Map Files (*.mpk *.zip)"));
    if(dialog.exec() != QDialog::Accepted) {
        settings.setValue("file/pwd", dialog.directory().path());
        return;
    }

    filePath = dialog.selectedFiles().first();
    ui->pkgPath->setText(filePath);
}

void DgImportPkg::on_buttonBox_accepted()
{
    QString package = ui->pkgPath->text();

    QStringList fileList = JlCompress::getFileList(package);

    // create folder to extract package to
    QString projectPath = QDir::currentPath() + "/" + ui->projectName->text();
    QDir dir(projectPath);
    dir.mkpath(".");

    // extract to project path
    QStringList imgList = JlCompress::extractDir(package, projectPath);

    curProjectName = projectPath;



    accept();
}
