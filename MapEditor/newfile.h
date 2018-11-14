#ifndef NEWFILE_H
#define NEWFILE_H

#include <QDialog>

namespace Ui {
class NewFile;
}

class NewFile : public QDialog
{
    Q_OBJECT

public:
    explicit NewFile(QWidget *parent = 0);
    ~NewFile();
    QString newProjectName;

signals:
    void changeProjectName(QString projectName);
    void open();

private slots:

    void on_accept_accepted();
    void on_newFileName_textEdited(const QString &arg1);
    void reject();

private:
    Ui::NewFile *ui;
    bool copy_dir_recursive(QString from_dir, QString to_dir, bool replace_on_conflit);
};

#endif // NEWFILE_H
