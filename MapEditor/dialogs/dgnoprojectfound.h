#ifndef DGNOPROJECTFOUND_H
#define DGNOPROJECTFOUND_H

#include <QDialog>

namespace Ui {
class DgNoProjectFound;
}

class DgNoProjectFound : public QDialog
{
    Q_OBJECT

public:
    explicit DgNoProjectFound(QString projectName, QWidget *parent = 0);
    ~DgNoProjectFound();
    QString newProjectDir;

private slots:
    void on_createFolder_clicked();

    void on_selectProject_clicked();

private:
    Ui::DgNoProjectFound *ui;
};

#endif // DGNOPROJECTFOUND_H
