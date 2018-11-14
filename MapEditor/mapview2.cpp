#include "mapview2.h"
#include <QDebug>
#include <QRegularExpression>
#include <QtMath>

Player::Player()
{
}

Player::Player(int n, int g, int l, int s)
{
    num = n;
    gold = g;
    lumber = l;
    stone = s;
}

Unit::Unit()
{
}

Unit::Unit(QString n, int xc, int yc)
{
    name = n;
    x = xc;
    y = yc;
}

// Empty constructor
MapView2::MapView2() { }

// Default (new map)
MapView2::MapView2(QMap<QString,Texture*>& loadedAssets, QString projName)
{
    curProjectName = projName;
    defaultMap();
    assets = loadedAssets;
    prevX = prevY = -420;

    assets = loadedAssets;
    terrain = new Terrain(curProjectName,  curProjectName + "/img/Terrain.png");
    buttonColors = new Texture(curProjectName + "/img/ButtonColors.png", 1, 1, curProjectName);
    buttonIcons = new Texture(curProjectName + "/img/Icons.png", 46, 38, curProjectName);

    tileDim.setRect(1,1,32,32);

    // testing for MapRendering parsing
    terrain->renderingInfo(curProjectName + "/img/MapRendering.dat");
}

MapView2::MapView2(QIODevice &mapFile ,QMap<QString,Texture*>& loadedAssets, QString projName, const QString &mapTexName)
{
    curProjectName = projName;
    openMap(mapFile);
//    setup();
    assets = loadedAssets;
    terrain = new Terrain(curProjectName, mapTexName);

    // upper-left corner and the rectangle size of width and height
    tileDim.setRect(1,1,32,32);

    // testing for MapRendering parsing
    terrain->renderingInfo(curProjectName + "/img/MapRendering.dat");
}


void MapView2::setup(){
    // grab all the asset files
    QString path = curProjectName + "/img";
    QString colorFile = curProjectName + "/img/Colors.png";
    QString goldmineTool = curProjectName + "/img/GoldMine.dat";
    QString peasantTool = curProjectName + "/img/Peasant.dat";
    QString archerTool = curProjectName + "/img/Archer.dat";
    QString knightTool = curProjectName + "/img/Knight.dat";
    QString rangerTool = curProjectName + "/img/Ranger.dat";
    QString townhallTool = curProjectName + "/img/TownHall.dat";
    QString barracksTool = curProjectName + "/img/Barracks.dat";
    QString blacksmithTool = curProjectName + "/img/Blacksmith.dat";
    QString cannontowerTool = curProjectName + "/img/CannonTower.dat";
    QString castleTool = curProjectName + "/img/Castle.dat";
    QString farmTool = curProjectName + "/img/Farm.dat";
    QString guardtowerTool = curProjectName + "/img/GuardTower.dat";
    QString keepTool = curProjectName + "/img/Keep.dat";
    QString lumbermillTool = curProjectName + "/img/LumberMill.dat";
    QString scouttowerTool = curProjectName + "/img/ScoutTower.dat";


    // append them to a vector
    QVector<QString> files;
    files.append(peasantTool);
    files.append(archerTool);
    files.append(knightTool);
    files.append(rangerTool);
    files.append(goldmineTool);
    files.append(townhallTool);
    files.append(barracksTool);
    files.append(blacksmithTool);
    files.append(cannontowerTool);
    files.append(castleTool);
    files.append(farmTool);
    files.append(guardtowerTool);
    files.append(keepTool);
    files.append(lumbermillTool);
    files.append(scouttowerTool);


    int nObjects = files.size();

    // create a texture for each asset
    for(int i = 0; i < nObjects; i++){
        Texture *tex = new Texture(files.at(i),colorFile, curProjectName);
        assets.insert( tex->textureName, tex);
    }
    assets.value("Peasant")->paintAll();
    assets.value("Knight")->paintAll();
    assets.value("Ranger")->paintAll();
    assets.value("Archer")->paintAll();
    assets.value("TownHall")->paintAll();
    assets.value("Barracks")->paintAll();
    assets.value("Blacksmith")->paintAll();
    assets.value("CannonTower")->paintAll();
    assets.value("Castle")->paintAll();
    assets.value("Farm")->paintAll();
    assets.value("GuardTower")->paintAll();
    assets.value("Keep")->paintAll();
    assets.value("LumberMill")->paintAll();
    assets.value("ScoutTower")->paintAll();


}

// creates a blank map and updates variables
void MapView2::defaultMap(){
    mapName = "untitled";

    // add 2 tiles for border
    mapDim.setHeight(64 + 2);
    mapDim.setWidth(96 + 2);

    // loop through all the tiles and add grass tiles
    for(int h = 0; h < mapDim.height(); h++ ){
        for(int w = 0; w < mapDim.width(); w++) {
            mapLayOut.append('G');
        }
    }

    // set player 0 (neutral)
    Player player = Player(0, 30000, 5000, 5000);
    players.append(player);

    // initialize players vector with 8 players
    for (int i = 1; i != 9; i++)
        players.append(Player(i, 1000, 1000, 1000));

}

// parses .map file and updates variables
void MapView2::openMap(QIODevice &mapFile){
    bool intTest;

    // check if the file is good
    if ( !mapFile.open(QIODevice::ReadOnly) ) {
        QMessageBox::warning(0,"error opening map",mapFile.errorString());
    }


    QString blankLine = " ";
    int lineNum = 0;

    QTextStream in(&mapFile);

    // Parse .map file
    while(!in.atEnd()){
        QString line = in.readLine();

        if (line == blankLine){
            QMessageBox::information(0,"file","blankLine");
            continue;
        }

        // get whether the first character is an int or not (to see whether we're done reading in the layout)
        line.toInt(&intTest, 10);

        lineNum++;
        if ( lineNum == 1){
            mapName = line;
        }
        else if ( lineNum == 2 ){
        }
        else if ( lineNum == 3) {
            // Dimension of the map
            QStringList strNums = line.split(" ");
            QVector<int> nums;
            for(auto iter = strNums.begin();iter != strNums.end();iter++){
                nums.append(iter->toInt());
            }
            // plus 2 for the border
            mapDim.setHeight(nums[1]+2);
            mapDim.setWidth(nums[0]+2);
        }
        else if ( lineNum == 4)
        {
            mapDescription = line;
        }
        else if ( lineNum == 5)
        {
            mapAllowedAIs = line.split(" ");
        }
        else if (!intTest && lineNum > 5){
            // key layout of the map
            for (auto iter = line.begin(); iter != line.end(); iter++ ){
                mapLayOut.append(*iter);
            }
        }
        else if (intTest){
            lineNum++;

            int numPlayers = line.toInt();

            // assumes the next line may be player's starting gold and lumber amount
            // or just a single number
            for(int pl = 0; pl <= numPlayers ; pl++)
            {
                line = in.readLine();
                QStringList playerValues = line.split(" ");
                Player player = Player(playerValues[0].toInt(), playerValues[1].toInt(), playerValues[2].toInt(), playerValues[3].toInt());
                players.append(player);
                lineNum++;
            }
            line = in.readLine();

            // grab number of units
            int numUnits = line.toInt();
            lineNum++;
            line = in.readLine();

            // for each unit
            for (int i = 0; i < numUnits; i++)
            {
                QStringList unitValues = line.split(" ");

                // create unit
                Unit unit(unitValues[0], unitValues[2].toInt(), unitValues[3].toInt());

                // add to the respective player
                players[unitValues[1].toInt()].units.append(unit);

                lineNum++;
                line = in.readLine();
            }


            // for each AI Trigger
            int numTriggers = line.toInt();
            lineNum++;
            line = in.readLine();

            for(int i = 0; i < numTriggers; i++) {
                QStringList list = line.split(',');

                QString name = list.takeFirst();
                QString type = list.takeFirst();
                bool persistence = list.takeFirst().toInt();
                QStringList trigArgs;
                trigArgs.append(list.takeFirst());
                trigArgs.append(list.takeFirst());
                QString event = list.takeFirst();
                QStringList eventArgs = list;

                // create Trigger
                AITrigger* trigger = new AITrigger(name, type, persistence, event, trigArgs, eventArgs);

                triggers.append(trigger);

                lineNum++;
                line = in.readLine();
            }

        }
    }

    mapFile.close();
}

QChar MapView2::getPreviousTile()
{
    return prevChar;
}

void MapView2::setAddedItems(QVector<QString> vec)
{
    addedItems.clear();
    for(int i = 0; i < vec.size(); i++)
        addedItems.append(vec.at(i));
}

void MapView2::changeMapTile(QGraphicsScene *scene, QPointF pos , Terrain::Type type ){

    // tile inside scene to change
    Tile *centerTile = qgraphicsitem_cast< Tile*>( scene->itemAt(pos, QTransform()) );

    if(centerTile==0)
    {
        return;
    }
    int x = centerTile->scenePos().x()/tileDim.width();
    int y = centerTile->scenePos().y()/tileDim.height();

    if(y==0 || x == 0 ||x ==mapDim.width() -1 || y == mapDim.height() -1)
    {
         return;
    }
  
    QString typedx = terrain->toString(type);
    QString strType = tileEncode(typedx, y , x );
    //qDebug() << strType;
    //qDebug() << "before" << mapLayOut.at(y*mapDim.width() + x) ;
    //if(mapLayOut.at(y*mapDim.width()+x) == strToMapkey(typedx))
    //    //No need to continue running this function if the tiles are the same
    //    return;

    if(saveChar && mapLayOut.at(y*mapDim.width()+x) != strToMapkey(typedx))
        prevChar = mapLayOut.at(y*mapDim.width() + x);

    mapLayOut.replace((y)*mapDim.width() + x,strToMapkey(typedx));

    //qDebug() << "After" <<mapLayOut.at(y*mapDim.width() + x) ;
    // changes center tile
    QImage image = *terrain->getImageTile(strType);
    centerTile->setTileImage(QPixmap::fromImage(image), typedx );


    QVector<Tile*> tiles;
    int i = centerTile->scenePos().y() - tileDim.height();
    int j = centerTile->scenePos().x() - tileDim.width();

    int rows = i + 3*tileDim.height();
    int cols = j + 3*tileDim.width();

    for(int y = i; y < rows; y += tileDim.height() ){
        for(int x = j ; x < cols; x+= tileDim.width() ){
            //TODO: Prevent asset image from being replaced by terrain that isn't the center tile
            QString xstr, ystr;
            int a = x;// - tileDim.width();
            int b = y;// - tileDim.height();
            xstr.setNum(a);
            ystr.setNum(b);
            ystr.prepend(xstr);
            // skip middle tile
            if( (y == centerTile->scenePos().y() && x == centerTile->scenePos().x())
                   || addedItems.contains(ystr))
                continue;
//            qDebug() << "(" << x << "," << y << ")";

//            QPointF pt(x,y);
            Tile *tile = dynamic_cast<Tile*>( scene->itemAt( QPoint(x,y), QTransform() ));
            if (tile == NULL ){
               // qDebug() << "null";
                continue;
            }
            tiles.append(tile);
            int xw = tile->scenePos().x()/tileDim.width();
            int yh = tile->scenePos().y()/tileDim.width();
            //qDebug() << "(" << xw << "," << yh << ")";

            QString typeStr;
            switch ( mapLayOut.at(yh*mapDim.width() + xw).toLatin1() ){
                case 'G':
                    typeStr = "grass";
                    break;
                case 'F':
                    typeStr = "tree";
                    break;
                case 'D':
                    typeStr = "dirt";
                    break;
                case 'W':
                    typeStr = "wall";
                    break;
                case 'w':
                    typeStr = "wall-damaged";
                    break;
                case 'R':
                    typeStr = "rock";
                    break;
                case ' ':
                    typeStr = "water";
                    break;
            }

            QString typeSelect = tileEncode(typeStr,yh,xw);
            QImage imageTile = *terrain->getImageTile(typeSelect);
            tile->setPixmap(QPixmap::fromImage(imageTile));

        }
    }

    if (typedx == "tree"){
        builtTreeTop(scene);
    }
    else
    {
        treeTopTiles.clear();
    }

   }

BrushDirection MapView2::findBrushDirection(int posX, int posY)
{
    if(prevX == posX)
    {
        if(prevY == posY+1)
            return BrushDirection::North;
        else if(prevY == posY-1)
            return BrushDirection::South;
    }
    else if(prevY == posY)
    {
        if(prevX == posX-1)
            return BrushDirection::East;
        else if(prevX == posX+1)
            return BrushDirection::West;
    }
    else
    {
        if(prevX == posX-1)
        {
            if(prevY == posY-1)
                return BrushDirection::SouthEast;
            else if(prevY == posY+1)
                return BrushDirection::NorthEast;
        }
        else if(prevX == posX+1)
        {
            if(prevY == posY-1)
                return BrushDirection::SouthWest;
            else if(prevY == posY+1)
                return BrushDirection::NorthWest;
        }
    }
    return BrushDirection::None;
}

void MapView2::assignSaveValues(bool **sv, int size)
{
    //Wherever true, save the previous tile at position (i,j)
    //Wherever false, then don't
    if(brushdir == BrushDirection::West)
    {
        for(int i = 0; i < size; i++)
        {
            for(int j = 0; j < size; j++)
                sv[i][j] = false;
            sv[i][0] = true;
        }
    }
    else if(brushdir == BrushDirection::NorthWest)
    {
        for(int i = 0; i < size; i++)
            for(int j = 0; j < size; j++)
            {
                sv[i][j] = false;
                if(i == 0 || j == 0)
                    sv[i][j] = true;
            }
    }
    else if(brushdir == BrushDirection::North)
    {
        for(int i = 0; i < size; i++)
            for(int j = 0; j < size; j++)
            {
                sv[i][j] = false;
                if(i == 0)
                    sv[i][j] = true;
            }
    }
    else if(brushdir == BrushDirection::NorthEast)
    {
        for(int i = 0; i < size; i++)
            for(int j = 0; j < size; j++)
            {
                sv[i][j] = false;
                if(i == 0 || j == size-1)
                    sv[i][j] = true;
            }
    }
    else if(brushdir == BrushDirection::East)
    {
        for(int i = 0; i < size; i++)
        {
            for(int j = 0; j < size; j++)
                sv[i][j] = false;
            sv[i][size-1] = true;
        }
    }
    else if(brushdir == BrushDirection::SouthEast)
    {
        for(int i = 0; i < size; i++)
            for(int j = 0; j < size; j++)
            {
                sv[i][j] = false;
                if(i == size-1 || j == size-1)
                    sv[i][j] = true;
            }
    }
    else if(brushdir == BrushDirection::South)
    {
        for(int i = 0; i < size; i++)
            for(int j = 0; j < size; j++)
            {
                sv[i][j] = false;
                if(i == size-1)
                    sv[i][j] = true;
            }
    }
    else if(brushdir == BrushDirection::SouthWest)
    {
        for(int i = 0; i < size; i++)
            for(int j = 0; j < size; j++)
            {
                sv[i][j] = false;
                if(i == size-1 || j == 0)
                    sv[i][j] = true;
            }
    }
    else
    {
        for(int i = 0; i < size; i++)
            for(int j = 0; j < size; j++)
                sv[i][j] = true;
    }
}

void MapView2::brush_size(QGraphicsScene *scene, QPointF pos , Terrain::Type type, int brush_size){
   QPointF position;
   int Actual_brush_size = brush_size;

   //Checks for different previous tiles in each direction
   QChar prevTile = 'Z';

   //Save the x- and y-coordinates of the current position for processing the next tile placement
   Tile *tile = qgraphicsitem_cast< Tile*>( scene->itemAt(pos, QTransform()) );
   int posX = tile->scenePos().x()/tileDim.width();
   int posY = tile->scenePos().y()/tileDim.height();

   if((type == Terrain::Water || type == Terrain::Rock) && brush_size > 1)
       Actual_brush_size = brush_size;
   else if(type == Terrain::Grass && brush_size > 1)
       Actual_brush_size = brush_size;
   else
       Actual_brush_size = 1;

   if(prevX == -420 && prevY == -420)
   {
        prevX = posX;
        prevY = posY;
   }

   //Find the brush direction to determine which previous tile to save for undo/redo
   brushdir = findBrushDirection(posX, posY);

   //Dynamically allocate booleans based on brush size for tile-saving
   bool **saveValues = new bool*[Actual_brush_size];
   for(int i = 0; i < Actual_brush_size; i++)
        saveValues[i] = new bool[Actual_brush_size];

   assignSaveValues(saveValues, Actual_brush_size);

   //Iterators for saveValues
   int a, b;
   a = b = 0;

   for(int i =pos.y(); i <pos.y() + (Actual_brush_size*32); i+=32)
   {
       for(int j= pos.x(); j < pos.x()+(Actual_brush_size*32);j+=32)
       {
//           qDebug()<< "inside the lo0p";
           position.setX(j);
           position.setY(i);
           saveChar = saveValues[a][b];
           changeMapTile(scene, position, type);
           if(Actual_brush_size > 1
              && saveValues[a][b]
              && prevTile != 'Z' //prevTile was not initialized
              && prevTile != prevChar)
           {//If there are different previous tiles across all saveChars, just assign grass
               prevChar = 'G';
           }
           if(saveValues[a][b])
               //Update prevTile for the next check
               prevTile = prevChar;
           b++;
       }
       b = 0;
       a++;
   }
   //Update previous values for the next iteration
   prevX = posX;
   prevY = posY;
}


QChar MapView2::strToMapkey(QString str){
    QChar mapkey;
    if (str == "grass"){
        mapkey = 'G';
    }
    else if (  str == "dirt"){
        mapkey = 'D';
    }
    else  if(  str == "water"){
        mapkey = ' ';
    }
    else if ( str == "tree"){
        mapkey = 'F';
    }
    else if (  str == "wall"){
        mapkey = 'W';
    }
    else if (  str == "rock"){
        mapkey = 'R';
    }
    else if (str == "wall-damaged"){
        mapkey = 'w';
    }

    return mapkey;

}

QString MapView2::tileEncode(QString strType ,int i , int j){
    if(i==0 || j== 0 ||j==mapDim.width() -1 || i == mapDim.height() -1)
    {
         return "border";
    }

    QString valueStrType ="";
    QVector<QChar> tiles;
    QString encodeStr = "";
    QChar centerType = strToMapkey(strType);
    // the following to ckeck whats around the current tile and get the right tile based in what around it.
    QChar upperLTile = (i-1 <= 0 || j - 1  <= 0) ? 'X' : mapLayOut.at((i-1)*mapDim.width() + (j-1));
    QChar TopTile = (i-1 <= 0 ) ? 'X' : mapLayOut.at((i-1)*mapDim.width() + j );
    QChar upperRTile = (i-1 <= 0 || j+1 >= mapDim.width() - 1) ? 'X' : mapLayOut.at((i-1)*mapDim.width() + (j+1));
    QChar centerLTile = (j-1 <= 0) ? 'X': mapLayOut.at((i)*mapDim.width() + (j-1));
    QChar centerRTile = (j+1 >= mapDim.width() -1 ) ? 'X' : mapLayOut.at((i)*mapDim.width() + (j+1));
    QChar downLTile = (i+1 >= mapDim.height() -1 || j -1 <= 0 ) ? 'X' : mapLayOut.at((i+1)*mapDim.width() + (j-1));
    QChar belowTile = (i+1 >= mapDim.height() -1 ) ? 'X': mapLayOut.at((i+1)*mapDim.width() + (j));
    QChar downRTile = (i+1 >= mapDim.height() -1 || j+1 >= mapDim.width() -1) ? 'X': mapLayOut.at((i+1)*mapDim.width() + (j+1));

    // water and rock have the same way of getting the right tile we are using 3 by 3 matrix of 0 and 1's zero for unmatch and 1 for match
    // the current tile will be at position 1,1.

    if (strType == "water" || strType == "rock"  ){

        tiles.append(downRTile);
        tiles.append(belowTile);
        tiles.append(downLTile);
        tiles.append(centerRTile);
        tiles.append(centerType);
        tiles.append(centerLTile);
        tiles.append(upperRTile);
        tiles.append(TopTile);
        tiles.append(upperLTile);
        for(int i = 0; i < tiles.size(); i++){
            if (i == 4){
                continue;
            }
            else if ( tiles.at(i) == centerType ){
                encodeStr += "1";
            }
            else {
                encodeStr += "0";
            }

        }
        bool ok;
        int num = encodeStr.toInt(&ok,2);
        valueStrType = strType +"-"+ QString().setNum(num);

    }
    else if(strType == "tree" ){

        tiles.append(downRTile);
        tiles.append(belowTile);
        tiles.append(downLTile);
        tiles.append(centerRTile);
        tiles.append(centerType);
        tiles.append(centerLTile);

        tiles.append(upperRTile);
        tiles.append(TopTile);
        tiles.append(upperLTile);

       // qDebug() <<"this is the cordinate "<< "(" << i << "," << j << ")";



        for(int i = 0; i < tiles.size(); i++){
            if ( tiles.at(i) == centerType ){
                encodeStr += "1";
            }
            else {
                encodeStr += "0";
            }
        }

       // qDebug() << encodeStr;

        bool ok;

        int num1 = encodeStr.left(6).toInt(&ok,2);
        int num2 = encodeStr.right(6).toInt(&ok,2);
        if(TopTile != centerType){
            if (i - 1 > 0 ){
                QString sPoint = QString().setNum( j*tileDim.width() ) + " " +  QString().setNum( (i-1)*tileDim.height() );
                treeTopTiles.insert( sPoint, strType + "-" + QString().setNum(num2) );
            }
          //  qDebug() << "tree top: " << strType + "-" + QString().setNum(num2);
        }

        valueStrType = strType + "-" + QString().setNum(num1);
    }
    else if(strType == "wall"){
        tiles.append(centerLTile);
        tiles.append(belowTile);
        tiles.append(centerRTile);
        tiles.append(TopTile);

        for(int i = 0; i < tiles.size(); i++){
            if ( tiles.at(i) == centerType ){
                encodeStr += "1";
            }
            else {
                encodeStr += "0";
            }
        }

        bool ok;
        int num = encodeStr.toInt(&ok,2);

        valueStrType = strType+ "-" + QString().setNum(num);

    }
   else if (strType == "grass"){

        tiles.append(downRTile);
        tiles.append(belowTile);
        tiles.append(downLTile);
        tiles.append(centerRTile);
        tiles.append(centerType);
        tiles.append(centerLTile);
        tiles.append(upperRTile);
        tiles.append(TopTile);
        tiles.append(upperLTile);


        for(int i = 0; i < tiles.size(); i++){
            if (i == 4){
                continue;
            }
            else if ( tiles.at(i) == centerType){
                encodeStr += "0";
            }
            else if(tiles.at(i) == 'W'|| tiles.at(i) == 'F'){
               // qDebug() << "Its a wall";
                encodeStr += "0";
            }
            else {
                encodeStr += "1";
            }

        }

        bool ok;
        int num = encodeStr.toInt(&ok,2);
        if(num==0)
        {
            //qDebug() << "all graas";

            valueStrType =  strType;
        }
        else
        {
            valueStrType = "dirt-"+ QString().setNum(num);
        }

    }
    else {
        valueStrType =  strType;

    }

    return valueStrType;

}

void MapView2::builtTreeTop(QGraphicsScene *scene){
    if(treeTopTiles.empty()){
      //  qDebug() << "No tree tops";
        return;
    }

    QString x = "";
    QString y = "";
    // sPoint is a string such as "32 64" this is the coordinates of tile
    for ( auto sPoint: treeTopTiles.keys()){
        QString typeS = treeTopTiles.value(sPoint);
        QImage image = *terrain->getImageTile(typeS);
        Tile * tile = new Tile(Terrain::Tree,QPixmap::fromImage(image), false);
        QStringList ptStr = sPoint.split(" ");
        x = ptStr.at(0);
        y = ptStr.at(1);
        tile->setPos( x.toInt() , y.toInt()  );
        scene->addItem(tile);
    }
    treeTopTiles.clear();
}

void MapView2::displayNewMap(QGraphicsScene *scene){
    QString type = "";
        int x = 0;
        int y = 0;
        for(int i = 0; i < mapDim.height(); ++i){
            for(int j = 0; j < mapDim.width(); ++j){
                if(i==0|| j==0||i== mapDim.height()-1 ||j==mapDim.width()-1)
                {
                    type = "border";
                }
                else if ( mapLayOut.at(i*mapDim.width() + j).toLatin1() == 'G'){
                    type = "grass-0";
                }
                QImage imageDx = *terrain->getImageTile(type);
                Tile *tile = new Tile(Terrain::Grass, QPixmap::fromImage(imageDx), false);

                x = j*tileDim.width();
                y = i*tileDim.height();
                tile->setPos(x,y);
                scene->addItem(tile);

            }
        }
}

// reads map array and updates the scene
void MapView2::builtmap(QGraphicsScene *scene)
{
    int x = 0;
    int y = 0;
    QString typeString = "";
    Terrain::Type type;
    QString tileStr = "";
    int n = 0;

    for(int i = 0; i < mapDim.height(); ++i){
        for( int j = 0; j < mapDim.width(); ++j){

            n = i*mapDim.width() + j;
            switch ( mapLayOut.at(n).toLatin1() ){
                case 'G':
                    typeString = "grass";
                    type = Terrain::Grass;
                    break;
                case 'F':
                    typeString = "tree";
                    type = Terrain::Tree;
                    break;
                case 'D':
                    typeString = "dirt";
                    type = Terrain::Dirt;
                    break;
                case 'W':
                    typeString = "wall";
                    type = Terrain::Wall;
                    break;
                case 'w':
                    typeString = "wall-damaged";
                    type = Terrain::Rubble;
                    break;
                case 'R':
                    typeString = "rock";
                    type = Terrain::Rock;
                    break;
                case ' ':
                    typeString = "water";
                    type = Terrain::Water;
                    break;
            }
            tileStr = tileEncode(typeString,i,j);
            QImage imageDx = *terrain->getImageTile(tileStr);
            QPixmap pixmap = QPixmap::fromImage(imageDx);
            Tile* pixItem = new Tile(type, pixmap, false);

            // sets each tile image x = 0*32,1*32,2*32,... y= 0*32,1*32,2*32,...
            x = j*tileDim.width();
            y = i*tileDim.height();
            pixItem->setPos(x,y);
            scene->addItem(pixItem);
        }
    }

}

void MapView2::builtAssets(QGraphicsScene *scene){

    QString unitName = "";
    int x = 0;
    int y = 0;
    for(int i = 0; i < players.size(); ++i){
        for(int j = 0; j < players[i].units.size(); ++j){

            unitName = players[i].units[j].name;
            // skip player 0, since it has no color assets
            QImage imageDx;
            if( i > 0){
                imageDx = assets.value(unitName)->colorPlayerImg.value(i).at(1);
            }
            else{
                imageDx = assets.value(unitName)->imageList.at(0);
            }
            Tile *unitItem = new Tile(unitName, QPixmap::fromImage(imageDx), true);
            x = tileDim.width()*players[i].units[j].x;
            y = tileDim.height()*players[i].units[j].y;
            unitItem->setPos(x,y);
            scene->addItem(unitItem);
        }
     }

    for(int i = 0; i < triggers.size(); i++) {
        QImage trigImage(":/data/default/img/Trigger.png");
        Tile* newTriggerTile = new Tile("Trigger", QPixmap::fromImage(trigImage), true);
        newTriggerTile->setZValue(10);

        newTriggerTile->setPos(32, (i+1)*32);
        if(triggers.at(i)->getType() == "TriggerTypeLocation")
            newTriggerTile->setPos((*triggers.at(i)->getPos())*32);

        triggers[i]->setTile(newTriggerTile);
        scene->addItem(newTriggerTile);
    }
}

void MapView2::displayMap(QGraphicsScene *scene){
    builtmap(scene);
    builtTreeTop(scene);
    builtAssets(scene);
}

void MapView2::addTrigger(AITrigger *trigger){
    if (trigger == 0){
        return;
    }

    //qDebug() << "added";
    triggers.append(trigger);
}

QVector<AITrigger*> MapView2::getTriggers(){
    return triggers;
}

Texture* MapView2::getButtonColorsTx(){
    return buttonColors;
}

Texture* MapView2::getButtonIconsTx(){
    return buttonIcons;
}

Terrain* MapView2::getTerrain(){
    return terrain;
}

QSize MapView2::getMapDim()
{
    return mapDim;
}

QString MapView2::getMapName()
{
    return mapName;
}

void MapView2::setMapName(QString name) {
    mapName = name;
}

QString MapView2::getMapDescription() {
    return mapDescription;
}

void MapView2::setMapDescription(QString description) {
    mapDescription = description;
}

QVector<QChar> MapView2::getMapLayout()
{
    return mapLayOut;
}

QVector<Player> MapView2::getPlayers()
{
    return players;
}

void MapView2::setPlayers(QVector<Player> &newPlayers) {
    players = newPlayers;
}

void MapView2::addPlayer(Player p)
{
    players.append(p);
}

int MapView2::getNumPlayers()
{
    return players.size() - 1; // ignore player 0
}

int MapView2::getNumUnits()
{
    int n = 0;

    for (auto itr = players.begin(); itr != players.end(); itr++){
        n += (*itr).units.size();
    }

    return n;
}

void MapView2::addUnit(Unit u, int player)
{
    players[player].units.append(u);
}

void MapView2::setMapLayout(QVector<QChar> layout)
{
    mapLayOut = layout;
}

Texture *MapView2::getAsset(QString assetName)
{
    Texture *t = assets.value(assetName);
    return t;
}
