#include "dgprojectselect.h"
#include "ui_dgprojectselect.h"
#include <QMainWindow>
#include <QSettings>

DgProjectSelect::DgProjectSelect(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DgProjectSelect)
{
    ui->setupUi(this);

}

DgProjectSelect::~DgProjectSelect()
{
    delete ui;
}


void DgProjectSelect::on_button_choose_clicked()
{
    QSettings settings;
    QString curPath = settings.value("file/pwd", QDir::homePath()).toString();

    QFileDialog dialog(this);
    dialog.setDirectory(curPath);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setNameFilter(tr("Map Files (*.map *.mpk *.zip)"));
    if(dialog.exec() != QDialog::Accepted) {
        settings.setValue("file/pwd", dialog.directory().path());
        return;
    }

    filePath = dialog.selectedFiles().first();
    ui->txt_filepath->setText(filePath);
}

void DgProjectSelect::on_button_load_clicked()
{
    if (filePath == "")
        return;

    shouldLoad = true;
    accept();
}

void DgProjectSelect::on_button_new_clicked()
{
    shouldLoad = false;
    accept();
}
