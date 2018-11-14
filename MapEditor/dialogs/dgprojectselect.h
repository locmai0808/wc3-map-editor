#ifndef DGPROJECTSELECT_H
#define DGPROJECTSELECT_H

#include <QDialog>
#include <QFileDialog>

namespace Ui {
class DgProjectSelect;
}

class DgProjectSelect : public QDialog
{
    Q_OBJECT

public:
    explicit DgProjectSelect(QWidget *parent = 0);
    ~DgProjectSelect();

    QString filePath;
    bool shouldLoad = false;

private slots:
    void on_button_choose_clicked();
    void on_button_load_clicked();

    void on_button_new_clicked();

private:
    Ui::DgProjectSelect *ui;

};

#endif // DGPROJECTSELECT_H
