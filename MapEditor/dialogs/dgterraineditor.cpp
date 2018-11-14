#include "dgterraineditor.h"
#include "ui_dgterraineditor.h"
#include <QDebug>
#include "tile.h"


DgTerrainEditor::DgTerrainEditor(Terrain *t, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DgTerrainEditor)
{
    // save ref to current txMap
    curTxMap = t->getTxMap();
    // save a copy to revert changes
    newTxMap = *curTxMap;
    newTxMap.detach();


    // ====== setup UI stuff, lists and models =====
    ui->setupUi(this);

    curTile = new QGraphicsScene;
    previewTile = new QGraphicsScene;
    ui->view_current->setScene(curTile);
    ui->view_new->setScene(previewTile);
    ui->view_current->show();
    ui->view_new->show();

    newListModel = new QStringListModel;
    curListModel = new QStringListModel;

    ui->list_new->setModel(newListModel);
    ui->list_current->setModel(curListModel);

    QStringList list = curTxMap->uniqueKeys();
    newListModel->setStringList(list);

    ui->combo_select->addItems(getUniqueName(list));
}

DgTerrainEditor::~DgTerrainEditor() {
    delete ui;
}

// stolen from texture? i think
QStringList DgTerrainEditor::getUniqueName(QStringList &names){
    QStringList uniqueName;
    QString temp;
    for (int i = 0; i < names.length(); i++) {
        temp = names.at(i).split("-").first();

        if(uniqueName.contains(temp)) continue;

        uniqueName.append(temp);

    }
    return uniqueName;
}


// updating combo box updates the currentView
void DgTerrainEditor::on_combo_select_currentTextChanged(const QString &arg1)
{
    QStringList list = curTxMap->uniqueKeys();
    curListModel->setStringList(list.filter(arg1));

    ui->list_current->setCurrentIndex(curListModel->index(0));
    on_list_current_entered(curListModel->index(0));
}

// ================= For Left List ==================

void DgTerrainEditor::on_list_current_entered(const QModelIndex &index)
{
    // update current view as well as the preview view
    updateCurView(index.data().toString());

    int r = newListModel->stringList().indexOf(index.data().toString());
    ui->list_new->setCurrentIndex(newListModel->index(r));
    on_list_new_entered(newListModel->index(r));
}

void DgTerrainEditor::on_list_current_pressed(const QModelIndex &index)
{
    on_list_current_entered(index);
}

// update current graphicsview
void DgTerrainEditor::updateCurView(QString tileName) {
    QPixmap p = QPixmap::fromImage(*curTxMap->value(tileName));
    QGraphicsPixmapItem* t = new QGraphicsPixmapItem(p);
    curTile->clear();
    curTile->addItem(t);
}

// ================= For right list ==================

void DgTerrainEditor::on_list_new_entered(const QModelIndex &index)
{
    updateNewView(index.data().toString());
}

void DgTerrainEditor::on_list_new_pressed(const QModelIndex &index)
{
    on_list_new_entered(index);
}

// update current graphicsview
void DgTerrainEditor::updateNewView(QString tileName) {
    QPixmap p = QPixmap::fromImage(*newTxMap.value(tileName));
    QGraphicsPixmapItem* t = new QGraphicsPixmapItem(p);
    previewTile->clear();
    previewTile->addItem(t);
}

// ================ Committing changes ===================

// commit changes and save the preview tile to the current tile
void DgTerrainEditor::on_button_apply_clicked()
{
    QString curTileName = ui->list_current->currentIndex().data().toString();
    QString newTileName = ui->list_new->currentIndex().data().toString();

    QImage* i = newTxMap.value(newTileName);

    curTxMap->insert(curTileName, i);

    updateCurView(curTileName);
}

// ================= Adding new Tiles ==================

// this function opens a jpg or png and converts into a tile, then adds it to newTxMap
void DgTerrainEditor::on_button_load_clicked()
{

}

