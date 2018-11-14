#ifndef DGIMPORTPKG_H
#define DGIMPORTPKG_H

#include <QDialog>

namespace Ui {
class DgImportPkg;
}

class DgImportPkg : public QDialog
{
    Q_OBJECT

public:
    explicit DgImportPkg(QString projName, QWidget *parent = 0);
    ~DgImportPkg();
    QString filePath;

private slots:
    void on_selectPkg_clicked();

    void on_buttonBox_accepted();

private:
    Ui::DgImportPkg *ui;
    QString curProjectName;
};

#endif // DGIMPORTPKG_H
