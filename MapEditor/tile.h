#ifndef TILE_H
#define TILE_H

#include <QGraphicsPixmapItem>
#include "texture.h"
#include "terrain.h"

class Tile : public QGraphicsPixmapItem
{
public:
    Tile(Terrain::Type type);
    Tile(Terrain::Type type, QPixmap texturePix, bool rmable);
    Tile(QString typeStr,QPixmap texturePix, bool rmable);

    QString toString();
    Terrain::Type getTerrainType();
//    void setTileImage(QPixmap pix);
    void setTileImage(QPixmap pix, QString typeS);
    QString getType();
    QString getAssetName();
    bool getBool();

//protected:
    //void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    Terrain::Type type;
    Terrain::Type terraintype;
    QString note;
    bool removable;
    QString typeStr;
    QPixmap texturePix;
    QString assetName;
//    Texture *tex;

};

#endif // TILE_H
