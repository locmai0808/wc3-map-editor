#ifndef MAPVIEW2_H
#define MAPVIEW2_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include "tile.h"
#include "texture.h"
#include "aitrigger.h"

class Unit
{
public:
    Unit();
    Unit(QString name, int x, int y);
    QString name;
    int x;
    int y;
};

class Player
{
public:
    Player();
    Player(int n, int g, int l, int s);
    enum playerType{ Human = 0, AI };
    int num;
    int gold;
    int lumber;
    int stone;
    playerType type = playerType::Human;
    QVector<Unit> units;
};

enum BrushDirection { None, North, South, East, West, NorthWest, NorthEast, SouthWest, SouthEast };

class MapView2
{
public:
    MapView2();
    MapView2(QMap<QString,Texture*>& loadedAssets, QString projName);

    MapView2(QIODevice &mapFile, QMap<QString,Texture*>& , QString projName, const QString &mapTexName);
//    QString tileEncode(QString strType,int i, int j);
//    void builtTreeTop(QGraphicsScene *scene);

    // get
    Terrain* getTerrain();
    Texture* getButtonColorsTx();
    Texture* getButtonIconsTx();
    void displayMap(QGraphicsScene *scene);
    void displayNewMap(QGraphicsScene *scene);

    QSize getMapDim();
    QString getMapName();
    QString getMapDescription();
    QVector<QChar> getMapLayout();
    QVector<Player> getPlayers();
    int getNumPlayers();
    int getNumUnits();
    QChar getPreviousTile();
    QVector<AITrigger*> getTriggers();
    Texture *getAsset(QString assetName);
    QString curProjectName;

    // set
    void setMapName(QString name);
    void setMapDescription(QString description);
    void setPlayers(QVector<Player> &newPlayers);
    void setMapLayout(QVector<QChar> layout);
    void addTrigger(AITrigger *trigger );
    void addUnit(Unit u, int player);
    void setAddedItems(QVector<QString> vec);

    // change tile at some pos and tiles around it
    void changeMapTile(QGraphicsScene *scene , QPointF point , Terrain::Type type);
    void brush_size(QGraphicsScene *scene , QPointF point , Terrain::Type type, int brush_size);

private:
    void defaultMap();
    void builtmap(QGraphicsScene *scene);
    void builtAssets(QGraphicsScene *scene);
    QString tileEncode(QString typeS,int i, int j);
    void builtTreeTop(QGraphicsScene *scene);

    void openMap(QIODevice &mapFile);
    void openMapTexture(const QString &mapTexture);
    void setup();
    void addPlayer(Player p);
    BrushDirection findBrushDirection(int posX, int posY);
    void assignSaveValues(bool **sv, int size);

    QImage createImageTile(QImage* image, const QRect &rect);

    QString mapName;
    QString shortProjectName;
    QVector<Player> players;
    QVector<QChar> mapLayOut;
    QRect tileDim;
    QSize mapDim;
    QMap<QString , QString> treeTopTiles;
    Terrain* terrain;
    Texture* toolbar;
    Texture* buttonColors;
    Texture* buttonIcons;
    QChar strToMapkey(QString str);

    QStringList mapAllowedAIs;
    QString mapDescription;
    QMap<QString,Texture*> assets;
    QVector<AITrigger*> triggers;
    QChar prevChar;
    bool saveChar;
    BrushDirection brushdir;
    int prevX, prevY;
    QVector<QString> addedItems;

};

#endif // MAPVIEW2_H
