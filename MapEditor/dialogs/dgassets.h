#ifndef DGASSETS_H
#define DGASSETS_H
#include "texture.h"
#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QTime>
#include <QDialogButtonBox>
#include <QVBoxLayout>

/* Name: dgassets.h
 * Class: DgAssets
 *
 * This file includes the class for the Assets Editor window.
 * See MainWindow::open_DgAssets()
 *
 */

namespace Ui {
class DgAssets;
}

class DgAssets : public QMainWindow
{
    Q_OBJECT
    enum directions{up, down, left, right};

public:
    explicit DgAssets(QString currentProject,QWidget *parent = 0);
    ~DgAssets();

private slots:
    bool on_actionopen_triggered();
    void on_actionsave_triggered();
    void on_actionExit_triggered();
    void on_comboBox_currentTextChanged(const QString &arg1);
    void on_pause_button_clicked();
    void on_play_button_clicked();
    void on_prev_button_clicked();
    void on_next_button_clicked();
    void on_left_button_clicked();
    void on_right_button_clicked();
    void on_up_button_clicked();
    void on_down_button_clicked();

private:
    bool getMax(QImage currentImage, DgAssets::directions direction);
    void select_animation(QString animationType, QStringList *);
    void delayUnit(int millisecondsToWait);
    QImage shiftPixel(QImage image, int shiftsize, DgAssets::directions direction);
    QStringList getUniqueName(QStringList *);
    Ui::DgAssets *ui;
    Texture *assets;
    QByteArray curFileDialogState;
    bool load_assets(QString name, QIODevice &file);
    QMap<QString,QVector<QImage> > animationImages;
    QGraphicsScene * scene;
    int counter = 0;
    bool pause = false;
    bool shiftLimit = true;
    QStringList besho;
    QString curProject;
    QString fileName;
    QDialogButtonBox *buttonBox;
    QVBoxLayout * layout;
};

#endif // DGASSETS_H
