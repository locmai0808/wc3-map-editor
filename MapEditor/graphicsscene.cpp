#include "graphicsscene.h"
#include <QDebug>
#include "ui_mainwindow.h"
#include "tile.h"
#include "texture.h"
#include "mainwindow.h"
#include <QMediaPlayer>
#include <QUrl>

GraphicsScene::GraphicsScene(QObject *parent, MapView2 *curMap, QMap<QString, Texture *> *loadedAssets, QString projName) : QGraphicsScene(parent)
{
    curProjectName = projName;
    GraphicsScene::parent = parent;
    GraphicsScene::mapInfo = curMap;
    GraphicsScene::assets = loadedAssets;
    brushing = false;
    brushable = false;
    large = false;
    music = new QMediaPlayer();
    gridON = false;
}

void GraphicsScene::delayUnit(int millisecondsToWait)
{
    QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

void GraphicsScene::addToolItem(QGraphicsSceneMouseEvent *mouseEvent)
{
    QWidget *q =  mouseEvent->widget()->parentWidget();
    QString name = q->accessibleName();
    if (name.compare("minimap") == 0)
    {
        QGraphicsView *view = this->views()[0];
        view->centerOn(mouseEvent->scenePos());
    }
    else
    {
        Tile *item = (Tile *)this->itemAt(mouseEvent->scenePos(), QTransform());

        int x = item->scenePos().x();
        int y = item->scenePos().y();
        int frames = 0;
        //Terrain *terrain = mapInfo->getTerrain();
        Terrain::Type type;
        Texture *asset = 0;


        if (curTool == "grass")
            type = Terrain::Grass;
        else if (curTool == "dirt")
            type = Terrain::Dirt;
        else if (curTool == "water")
            type = Terrain::Water;
        else if (curTool == "rock")
            type = Terrain::Rock;
        else if (curTool == "tree")
            type = Terrain::Tree;
        else if (curTool == "wall")
            type = Terrain::Wall;
        else if (curTool == "rubble")
            type = Terrain::Rubble;
        else if (curTool == "Peasant")
        {
            asset = mapInfo->getAsset("Peasant");
            music->setMedia(QUrl(curProjectName + "/snd/peasant/ready.wav"));
            widthXheight = 1;
        }
        else if (curTool == "Ranger")
        {
            asset = mapInfo->getAsset("Ranger");
            music->setMedia(QUrl(curProjectName + "/snd/archer/ready.wav"));
            widthXheight = 1;
        }
        else if (curTool == "Archer")
        {
            asset = mapInfo->getAsset("Archer");
            music->setMedia(QUrl(curProjectName + "/snd/archer/ready.wav"));
            widthXheight = 1;
        }
        else if (curTool == "Knight")
        {
            asset = mapInfo->getAsset("Knight");
            music->setMedia(QUrl(curProjectName + "/snd/knight/ready.wav"));
            widthXheight = 1;
        }
        else if (curTool == "GoldMine")
        {
            asset = mapInfo->getAsset("GoldMine");
            music->setMedia(QUrl(curProjectName + "/snd/buildings/gold-mine.wav"));
            widthXheight = 4;
            large = true;
        }
        else if (curTool == "TownHall")
        {
            asset = mapInfo->getAsset("TownHall");
            music->setMedia(QUrl(curProjectName + "/snd/misc/thunk.wav"));
            widthXheight = 4;
            large = true;
        }
        else if (curTool == "Barracks")
        {
            asset = mapInfo->getAsset("Barracks");
            music->setMedia(QUrl(curProjectName + "/snd/misc/thunk.wav"));
            widthXheight = 3;
            large = true;
        }
        else if (curTool == "Blacksmith")
        {
            asset = mapInfo->getAsset("Blacksmith");
            music->setMedia(QUrl(curProjectName + "/snd/buildings/blacksmith.wav"));
            widthXheight = 3;
            large = true;
        }
        else if (curTool == "CannonTower")
        {
            asset = mapInfo->getAsset("CannonTower");
            music->setMedia(QUrl(curProjectName + "/snd/misc/thunk.wav"));
            widthXheight = 2;
        }
        else if (curTool == "Castle")
        {
            asset = mapInfo->getAsset("Castle");
            music->setMedia(QUrl(curProjectName + "/snd/misc/thunk.wav"));
            widthXheight = 4;
            large = true;
        }
        else if (curTool == "Farm")
        {
            asset = mapInfo->getAsset("Farm");
            music->setMedia(QUrl(curProjectName + "/snd/buildings/farm.wav"));
            widthXheight = 2;
        }
        else if (curTool == "GuardTower")
        {
            asset = mapInfo->getAsset("GuardTower");
            music->setMedia(QUrl(curProjectName + "/snd/misc/construct.wav"));
            widthXheight = 2;
        }
        else if (curTool == "ScoutTower")
        {
            asset = mapInfo->getAsset("ScoutTower");
            music->setMedia(QUrl(curProjectName + "/snd/misc/construct.wav"));
            widthXheight = 2;
        }
        else if (curTool == "Keep")
        {
            asset = mapInfo->getAsset("Keep");

            music->setMedia(QUrl(curProjectName + "/snd/misc/thunk.wav"));
            widthXheight = 4;
            large = true;
        }

        else if (curTool == "LumberMill")
        {
            asset = mapInfo->getAsset("LumberMill");
            music->setMedia(QUrl(curProjectName + "/snd/buildings/lumber-mill.wav"));
            widthXheight = 3;
            large = true;
        }
        else if (curTool == "hand")
        {
            clearSelection();
            return;
        }
        else if( curTool == "Trigger"){
            brushable = false;
            QImage image;
            image.load(":/data/default/img/Trigger.png");
            Tile *item = new Tile("Trigger", QPixmap::fromImage(image), true);
            item->setPos(x,y);
            item->setZValue(10);
            addItem(item);
            emit open_DTrigger(this, item);
            return;
        }

        QImage imageDx;
        if (!asset){
           // tile change
            if(checkPosForTile(x,y,CurBrushSize))
            {
              if((type == Terrain::Water || type == Terrain::Rock) && CurBrushSize == 1)
                  CurBrushSize = 2;
                brushable = true;
                mapInfo->brush_size(this, mouseEvent->scenePos(),type,CurBrushSize);
                //mapInfo->changeMapTile(this, mouseEvent->scenePos(),type);
            }
            else
            {
                QMessageBox::warning(0,"Error!","Cannot put tile on assets");
                return;
            }
        }
        else
        {//Loads/Animates the assets
            int besho = 2;
            if(large)
            {
                besho = widthXheight;
            }
            if(x==0||y==0|| (x/32) + besho>= mapInfo->getMapDim().width()||(y/32)+besho >= mapInfo->getMapDim().height())
            {
                QMessageBox::warning(0,"Cannot place","Place somewhere else away from border!");
                return;
            }

            brushable = false;
            if(curTool == "Peasant" || curTool == "Archer" || curTool == "Knight" || curTool == "Ranger")
                imageDx = asset->colorPlayerImg[curPlayer][20];
            else if (curTool == "TownHall" || curTool == "Barracks" || curTool == "Farm" || curTool == "ScoutTower" || curTool == "LumberMill" || curTool == "Blacksmith")
                imageDx = asset->colorPlayerImg[curPlayer][2];
            else if (curTool == "CannonTower" || curTool == "Castle" || curTool == "GuardTower" || curTool == "Keep")
                imageDx = asset->colorPlayerImg[curPlayer][1];
            else if (curTool == "GoldMine")
                imageDx = asset->imageList[0];

            QPixmap pixmap = QPixmap::fromImage(imageDx);

            Tile * pixItem = new Tile(curTool, pixmap, true);
            pixItem->setPos(x, y);
            if (checkPosition(qgraphicsitem_cast<QGraphicsItem* >(pixItem)))
            {

                addItem(pixItem);
                music->play();
            }
            else
            {
                QMessageBox::warning(0,"Cannot overlap","Place somewhere else!");
                return;
            }
        }

        if (!asset)
            emit changedLayout(x, y, type);
        else
        {
            if (curTool == "GoldMine")
                emit changedAsset(x, y, curTool, 0);
            else
                emit changedAsset(x, y, curTool, curPlayer);
        }
    }
}

void GraphicsScene::removeToolItem(QGraphicsSceneMouseEvent *mouseEvent)
{
    QWidget *q =  mouseEvent->widget()->parentWidget();
    QString name = q->accessibleName();

    if (name.compare("minimap") == 0)
    {
        QGraphicsView *view = this->views()[0];
        view->centerOn(mouseEvent->scenePos());
    }
    else
    {
        Tile *item = (Tile *)this->itemAt(mouseEvent->scenePos(), QTransform());

        if (item->getBool() == true)
        {
            this->removeItem(item);
        }
        else
        {
            return;
        }
    }
}

bool GraphicsScene::checkPosForTile(int PositionX, int PositionY, int BrushSize)
{
    QPointF tempPoint;
    bool placeable = true;
    for(int i = 0; i <= 32*BrushSize; i++)
    {
        for (int j =0; j <= 32*BrushSize; j++)
        {
            tempPoint.setX(PositionX+i);
            tempPoint.setY(PositionY+j);
            Tile *place = qgraphicsitem_cast< Tile* >(this->itemAt(tempPoint,QTransform()));
            if(place->getBool())
            {
                qDebug() << "Cannot place on" << place->getAssetName();
                return false;
            }
        }
    }
    return placeable;
}

bool GraphicsScene::checkPosition(QGraphicsItem *newitem){
   QPointF tempPoint;
   bool placeable = false;
   for(int i =0; i< widthXheight*32; i++)
   {
       for(int j =0; j< widthXheight*32;j++)
       {
           tempPoint.setX(newitem->x()+i);
           tempPoint.setY(newitem->y()+j);
           Tile *place = qgraphicsitem_cast< Tile* >(this->itemAt(tempPoint,QTransform()));
           //Terrain::Type is for loading a new map, the tile = Terrain::Type
           //"tiletype" is while building, placing a new tile will give String
           if(place->getAssetName().length() > 0 ||
                   place->getTerrainType() == Terrain::Tree ||
                   place->getTerrainType() == Terrain::Rock ||
                   place->getTerrainType() == Terrain::Wall ||
                   place->getTerrainType() == Terrain::Water ||
                   place->getTerrainType() == Terrain::Rubble ||
                   place->getType() == "water" ||
                   place->getType() == "tree" ||
                   place->getType() == "rock" ||
                   place->getType() == "wall" ||
                   place->getType() == "rubble")
           {
               return false;
           }
           else if(place->getTerrainType() == Terrain::Grass || place->getTerrainType() == Terrain::Dirt)
           {
               placeable = true;
           }
       }
   }
   return placeable;
}


void GraphicsScene::setBrushable(bool b)
{
    brushable = b;
}

int GraphicsScene::getCurBrushSize()
{
    return CurBrushSize;
}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton && withinBounds(mouseEvent))
        brushing = true;
        brushable = true;
    rubberband = mouseEvent->scenePos().toPoint();
    if((mouseEvent->modifiers() & Qt::ControlModifier) && mouseEvent->button()==Qt::LeftButton)
    {
        Tile *item = (Tile *)this->itemAt(mouseEvent->scenePos(), QTransform());
        qDebug() << item->getAssetName();
        qDebug() << item->getBool();
        qDebug() << item->getTerrainType();
    }

    QGraphicsScene::mousePressEvent(mouseEvent);
}

void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent){
    if ((mouseEvent->buttons() & Qt::LeftButton) && brushing && withinBounds(mouseEvent) && brushable)
        addToolItem(mouseEvent);
    mouseX = mouseEvent->scenePos().toPoint().x();
    mouseY = mouseEvent->scenePos().toPoint().y();
    offsetX = mouseX % 32;
    offsetY = mouseY % 32;
    emit changecursor(curTool);
}

void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton && withinBounds(mouseEvent) && brushable) {
        addToolItem(mouseEvent);
        brushing = false;        
    }
    if (mouseEvent->button() == Qt::RightButton){
        removeToolItem(mouseEvent);
        brushing = false;
    }
   QRect rect(rubberband, mouseEvent->scenePos().toPoint());
   QPainterPath pp;
   pp.addRect(rect);
   setSelectionArea(pp);
   itemList = selectedItems();
   QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void GraphicsScene::keyPressEvent(QKeyEvent *keyEvent)
{
    if(keyEvent->key() == Qt::Key_D)
    {
        for (int i=0; i<itemList.size(); i++)
        {
              this->removeItem(itemList[i]);
        }
    }
}

bool GraphicsScene::withinBounds(QGraphicsSceneMouseEvent *mouseEvent)
{//Checks to see if the mouse event occurs within map bounds to prevent crashing
    return mouseEvent->scenePos().x() >= 0 && mouseEvent->scenePos().x() < width()
            && mouseEvent->scenePos().y() >= 0 && mouseEvent->scenePos().y() < height();
}

void GraphicsScene::setGridlines(bool showGrid){
    gridON = showGrid;
}

void GraphicsScene::drawForeground(QPainter *painter, const QRectF &rect){
    if( !gridON){
        return;
    }
    const int gridSize = 32;
    qreal left = int(rect.left()) - (int(rect.left()) % gridSize);
    qreal top = int(rect.top()) - (int(rect.top()) % gridSize);

    QVarLengthArray<QLineF, 100> lines;

    for (qreal x = left; x < rect.right(); x += gridSize)
        lines.append(QLineF(x, rect.top(), x, rect.bottom()));
    for (qreal y = top; y < rect.bottom(); y += gridSize)
        lines.append(QLineF(rect.left(), y, rect.right(), y));

    //qDebug() << lines.size();

    painter->drawLines(lines.data(), lines.size());

}
