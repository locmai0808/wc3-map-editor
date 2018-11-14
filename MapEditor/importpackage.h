#ifndef IMPORTPACKAGE_H
#define IMPORTPACKAGE_H

#include <QDialog>
#include <QNetworkReply>

namespace Ui {
class ImportPackage;
}

class ImportPackage : public QDialog
{
    Q_OBJECT

public:
    explicit ImportPackage(QWidget *parent = 0);
    ~ImportPackage();
    QNetworkReply *reply;

public slots:
    void finishedSlot(QNetworkReply *reply);
    void finished();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::ImportPackage *ui;
};

#endif // IMPORTPACKAGE_H
