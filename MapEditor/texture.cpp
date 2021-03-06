#include "texture.h"
#include <QDebug>


Texture::Texture(const QString &texFileName, int w, int h, QString projName)
{
    curProjectName = projName;
    // save texture tile size
    Texture::width = w;
    Texture::height = h;

    // load QImage from file
    fullImage = open(texFileName);

    // load typelist from the corresponding dat file
    datFileName = toDat(texFileName);

    scanDatFile(datFileName, w, h);

    // upper-left corner and the rectangle size of width and height
    tileDim.setRect(0,0,32,32);
}


Texture::Texture( const QString &textureName, const QString &colorFile, QString projName)
{
    curProjectName = projName;
    scanTexture(textureName);
    openColor(colorFile);
    tileDim.setRect(0,0,32,32);
}

Texture::Texture( const QString &textureName, QString projName){
    curProjectName = projName;
    scanTexture(textureName);
}

// helper function to convert filename.png to filename.dat
QString Texture::toDat(QString texFileName) {
    QStringList filePath = texFileName.split(".");
    QStringList pathnames = filePath[0].split("/");
    QString texName = pathnames.back();
    return QString(curProjectName + "/img/" + texName + ".dat");
}

// load QImage from file
QImage Texture::open(const QString &textureName){
    QImage img;
    if( !img.load(textureName)){
        QMessageBox::information(0,"error1","image");
    }
    return img;
}

void Texture::openColor(const QString &colorFile){
    QImage colorImg;

    if( !colorImg.load(colorFile)){
        QMessageBox::information(0,"error2","image");
    }


    // stores colors in Qmap
    QVector<QColor> colorV;
    for(int i = 0; i < colorImg.height(); i++){
        QVector<QColor> colorV;
        for(int j = 0; j < colorImg.width(); j++ ){
            QColor color(colorImg.pixel(j,i) );
            colorV.append(color);
        }

        colorMap.insert(i, colorV);
        colorV.clear();
    }
}

QVector<QImage> Texture::paintUnit(int colorPick){
    int blue = 0;
    int shade = 0;
    QVector< QImage > imagesPaint;
    // skip blue
    for(int nImg = 0; nImg < imageList.size(); ++nImg ){
        QImage image = imageList.at(nImg);
        for(int i=0; i < image.height() ;i++)
        {
            for(int j=0 ; j < image.width();j++ )
            {
                QColor color(image.pixelColor(i,j));

                for(int index = 0; index < colorMap[blue].size(); index++ ){
                    if (color == colorMap[blue].at(index)){
                        shade = index;
                        QColor c = colorMap.value(colorPick).at(shade);
                        image.setPixel( i,j, c.rgb() );
                    }
                }

            }
        }
        imagesPaint.append(image);
    }

//    QGraphicsScene *scene = new QGraphicsScene();
//    QGraphicsView *view = new QGraphicsView(scene);
//    QPixmap pix = QPixmap::fromImage(image);
//    scene->addPixmap(pix);
//    view->show();

    return imagesPaint;
}

void Texture::paintAll(){

    int nPlayers = 8;
    int blue = 1;
    // will need to change later but player 0 is neutral
    // actual player 1 is blue
    // however the colors start with blue = 0.
    colorPlayerImg.insert(blue,imageList);
    // skip first players of blue color
    for(int i = 0; i < nPlayers; i++){
         colorPlayerImg.insert(i+1,paintUnit(i));
    }
}

// this function takes all image files from a large png, chops them up and stores them in Texture::txMap
void Texture::scanDatFile(const QString datFileName, int width, int height) {
    QFile file(datFileName);

    if (!file.open(QIODevice::ReadOnly)){
        QMessageBox::information(0,"error5",file.errorString());
    }

    QString name;
    int lineNum = 0;

    QTextStream in(&file);
    while(!in.atEnd()){

        QString line  = in.readLine();

        if (line == QString(" "))
            continue;

        lineNum++;

        if(lineNum > 2){
            int pos = (lineNum - 3) * height;
        QImage* tile = new QImage(fullImage.copy(0,pos,width,height));
            Texture::txMap.insert(line, tile);
        }

    }
}


void Texture::scanTexture(const QString &texFileName){
    QFile file(texFileName);

    if (!file.open(QIODevice::ReadOnly)){
        QMessageBox::information(0,"error4",file.errorString());
    }

    QString name;
    int lineNum = 0;
    int numTypes = 0;

    QTextStream in(&file);
    while(!in.atEnd()){

        QString line  = in.readLine();

        // skip blankline
        if (line == QString(" "))
            continue;

        lineNum++;

        if (lineNum == 1){
            name = line;
            name = name.remove(0,2);
            textureName = name.remove(QRegExp(".png"));

        }
        else if(lineNum == 2){
            numTypes =  line.toInt();

        }
        else if(lineNum > 2){
            types.append(line);
        }
    }

    QDir dir;
    QString path = curProjectName + "/img/";
    QString nameFile = path+textureName+".png";

    QImage img;
    if( !img.load(nameFile)){
        QMessageBox::information(0,"error3","Couldn't load: " + nameFile);
    }

    int offsetHeight = 0;
    // assumes each img is squared.
    int width = img.width();
    int height = width;
    for(int i = 0; i < numTypes; ++i){
        QImage image = img.copy(0,offsetHeight,width,height);
        offsetHeight += width;
        imageList.append(image);
    }

}

QMap<QString, QImage*>* Texture::getTxMap() {
    return rTxMap;
}

QImage* Texture::getImage(QString txName){
    return txMap.value(txName);
}

QPixmap Texture::getPixTile(QString tileName){
    return QPixmap::fromImage(*txMap.value(tileName));
}

void Texture::display(){

}

