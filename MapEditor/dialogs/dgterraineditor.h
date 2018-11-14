#ifndef DGTERRAINEDITOR_H
#define DGTERRAINEDITOR_H

#include <QDialog>
#include "terrain.h"
#include "texture.h"
#include <QStringListModel>
#include <QItemSelectionModel>

namespace Ui {
class DgTerrainEditor;
}

class DgTerrainEditor : public QDialog
{
    Q_OBJECT

public:
    explicit DgTerrainEditor(Terrain *terrain, QWidget *parent = 0);
    ~DgTerrainEditor();

private slots:
    void on_combo_select_currentTextChanged(const QString &arg1);

    void on_button_load_clicked();

    void on_list_current_entered(const QModelIndex &index);
    void on_list_current_pressed(const QModelIndex &index);

    void on_list_new_entered(const QModelIndex &index);
    void on_list_new_pressed(const QModelIndex &index);

    void on_button_apply_clicked();

private:
    Ui::DgTerrainEditor *ui;
    QGraphicsScene* curTile;
    QGraphicsScene* previewTile;
    QStringListModel* curListModel;
    QStringListModel* newListModel;

    QMap< QString, QImage*>* curTxMap;
    QMap< QString, QImage*> newTxMap;

    void updateCurView(QString tileName);
    void updateNewView(QString tileName);
    QStringList getUniqueName(QStringList &names);

};

#endif // DGTERRAINEDITOR_H
