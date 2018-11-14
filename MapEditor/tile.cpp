#include "tile.h"
#include "texture.h"
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>


Tile::Tile(Terrain::Type type){ // not sure if needed
    Tile::type = type;
}


Tile::Tile(QString typeS,QPixmap texturePix, bool rmable):QGraphicsPixmapItem(texturePix){
    typeStr = typeS;
    assetName = typeS;
    texturePix = texturePix;
    removable = rmable;
    setFlag(ItemIsSelectable);
}

Tile::Tile(Terrain::Type type,QPixmap texturePix, bool rmable): QGraphicsPixmapItem(texturePix){
    Tile::type = type;
    terraintype = type;
    removable = rmable;
    Tile::texturePix = texturePix;
    note = "test";
}

//void Tile::mousePressEvent(QGraphicsSceneMouseEvent *event)
//{
    //qWarning() << Q_FUNC_INFO << event->scenePos();
    //qDebug() << "in Tile:mousePressEvent";
//}

//void Tile::setTileImage(QPixmap pix){
//    setPixmap(pix);
//}

void Tile::setTileImage(QPixmap pix, QString typeS){
    setPixmap(pix);
    typeStr = typeS;
}

Terrain::Type Tile::getTerrainType(){
    return terraintype;
}

QString Tile::getType(){
    return typeStr;
}

QString Tile::getAssetName(){
    return assetName;
}

bool Tile::getBool(){
    return removable;
}


QString Tile::toString()
{
    // enum  Type { Grass = 0, Dirt , Tree , Water , Rock , WallDamage , Wall , Rubble };
    switch(type)
    {
    case Terrain::Grass:
        return "Grass";
        break;
    case Terrain::Dirt:
        return "Dirt";
        break;
    case Terrain::Tree:
        return "Tree";
        break;
    case Terrain::Water:
        return "Water";
        break;
    case Terrain::Rock:
        return "Rock";
        break;
    case Terrain::Wall:
        return "Wall";
        break;
    case Terrain::WallDamage:
        return "WallDamage";
        break;
    case Terrain::Rubble:
        return "Rubble";
        break;
    }
    return 0;
}




//QRectF Tile::boundingRect() const {
//    return QRectF(0,0,100,100);
//}

//void Tile::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
//    QRectF rect = boundingRect();

//}

