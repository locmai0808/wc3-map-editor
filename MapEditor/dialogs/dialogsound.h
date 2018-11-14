#ifndef DIALOGSOUND_H
#define DIALOGSOUND_H

#include <QDialog>
#include "sound.h"
#include <QUrl>
#include <QComboBox>
#include <QRadioButton>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLineEdit>
#include <QFileDialog>

namespace Ui {
class DialogSound;
}

class DialogSound : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSound(QWidget *parent = 0, Sound *sound=0,QString name = "");
    ~DialogSound();
    void setName(QString name);
    void readDirectory(QString path);


public slots:
    void saveComboBoxTopChoices();


private slots:
    void pushButton_clicked();
    void openDirectory();
    void directoryChange(QString);

//    void open();


private:
    Ui::DialogSound *ui;
    void horizontalColumns();
    void setComboBoxList(QString name);
    void updateComboBoxList(QStringList newItems);
    void horizontalInput();

    QString findDirectory(QString);

    Sound *soundLoaded;
    Sound *tempSound;
    QString nameType;
    QMap<QString,QVector<QComboBox*> > comboBoxlist;
    QMap<QString,QVector<QRadioButton*>> radioButtons;
    QVBoxLayout *mainLayout;
    QDialogButtonBox *buttonBox;
    QStringList columnList = {"acknowledge","ready","annoyed","selected"};
    QPushButton *pushButton;
    QStringList openDirectories;
    QMap<QString,QString> openFiles;
    QComboBox* foldersBox;


};

#endif // DIALOGSOUND_H
