#include "dgassets.h"
#include "ui_dgassets.h"
#include <QFile>
#include <QFileDialog>
#include <QDebug>
#include <QList>
#include <QSignalBlocker>
#include <QColor>



DgAssets::DgAssets(QString currentProject,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DgAssets)
{
    ui->setupUi(this);
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout = new QVBoxLayout();
    connect(buttonBox,SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    layout->addWidget(buttonBox);

    scene = new QGraphicsScene;
    ui->graphicsView->setScene(scene);
    ui->graphicsView->scale(2,2);
    curProject = currentProject;
}

DgAssets::~DgAssets()
{
    delete ui;
}

bool DgAssets::on_actionopen_triggered()
{
     QFileDialog dialog(this);
     dialog.restoreState(curFileDialogState);
     dialog.setWindowModality(Qt::WindowModal);
     dialog.setFileMode(QFileDialog::ExistingFile);
     dialog.setAcceptMode(QFileDialog::AcceptOpen);
     dialog.setNameFilter(tr(" Files (*.dat)"));
//     if (!maybeSave())
//         return false;
     if(dialog.exec() != QDialog::Accepted)
         return false;
     curFileDialogState = dialog.saveState();

    fileName = dialog.selectedFiles().first().split(".").first().split("/").last();
    qDebug()<<fileName;
     if(dialog.selectedFiles().first().split(".").last() == "dat")
     {
         QFile file(dialog.selectedFiles().first());
         return load_assets(dialog.selectedFiles().first(), file);
     }
}


void DgAssets::delayUnit(int millisecondsToWait)
{
    QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

void DgAssets::on_actionExit_triggered()
{

}
bool DgAssets::load_assets(QString name, QIODevice &file)
{
    // check if the file is good
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(0,"error opening map",file.errorString());
        return false;
    }
    file.close();
    const bool blocked = ui->comboBox->blockSignals(true);
    ui->comboBox->clear();
    ui->comboBox->blockSignals(blocked);
    animationImages.clear();

    assets = new Texture(name, curProject);
    besho = getUniqueName(&assets->types);
    //qDebug() << besho;
    for(int i=0; i < besho.size(); i++)
    {
        select_animation(besho.at(i),&assets->types );
    }
    ui->comboBox->addItems(besho);
}

void DgAssets::select_animation(QString animationType, QStringList *names){
    QStringList animation;
    animation = names->filter(QRegExp(animationType + "[0-9]"));
   // qDebug() << animation;
    QVector<QImage > imageVector;
    for(int i = 0; i < animation.length(); i++)
    {
        for(int j = 0; j < names->length(); j++)
        {
            if(animation.at(i) == names->at(j))
            {
                imageVector.append(assets->imageList.at(j));
            }
        }
    }
    animationImages.insert(animationType,imageVector);
}

QStringList DgAssets::getUniqueName(QStringList *names){
    QStringList uniqueName;
    QString temp;
    for (int i = 0; i < names->length(); i++)
    {
        temp = names->at(i).left(names->at(i).length() - 1);
        if(uniqueName.contains(temp))
        {
            continue;
        }
        else
        {
            uniqueName.append(temp);
        }
    }
    return uniqueName;
    //qDebug() << uniqueName;
}

void DgAssets::on_comboBox_currentTextChanged(const QString &arg1)
{
    scene->clear();
    //select_animation(arg1, &assets->types);
    scene->addPixmap(QPixmap::fromImage(animationImages.value(arg1).at(0) ));
    counter = 0;
    //on_play_button_clicked();
    on_pause_button_clicked();
}

bool DgAssets::getMax(QImage currentImage, DgAssets::directions direction)
{
    for(int i =0; i< currentImage.height(); i++)
    {
        for (int j = 0; j < currentImage.height(); j++)
        {
            if(direction == up)
            {
                QColor curPixelColor(currentImage.pixelColor(i,1));
                if(curPixelColor.alpha() != 0)
                {
                    return false;
                }
            }
            else if(direction == down)
            {
                QColor curPixelColor(currentImage.pixelColor(i,currentImage.width()- 1));
                if(curPixelColor.alpha() != 0)
                {
                    return false;
                }
            }
            else if(direction == left)
            {
                QColor curPixelColor(currentImage.pixelColor(1,j));
                if(curPixelColor.alpha() != 0)
                {
                    return false;
                }
            }
            else if(direction == right)
            {
                QColor curPixelColor(currentImage.pixelColor(currentImage.height()-1,j));
                if(curPixelColor.alpha() != 0)
                {
                    return false;
                }
            }

        }
    }
    return true;
}

void DgAssets::on_play_button_clicked()
{
    pause = false;
    ui->pause_button->setDisabled(false);
    ui->play_button->setDisabled(true);
    ui->next_button->setDisabled(true);
    ui->prev_button->setDisabled(true);
    ui->comboBox->setDisabled(true);
    scene->clear();
    QVector<QImage> temp = animationImages.value(ui->comboBox->currentText());
    for(counter; counter < temp.size() && !pause; counter++)
    {
       // qDebug() << counter;
        QGraphicsPixmapItem *items = new QGraphicsPixmapItem();
        items->setPixmap(QPixmap::fromImage(temp.at(counter)));
        scene->addItem(items);
        delayUnit(250);
        scene->removeItem(items);
        if(counter == temp.size()-1)
        {
            counter = -1;
        }
    }
}


void DgAssets::on_pause_button_clicked()
{
    pause = true;
    ui->pause_button->setDisabled(true);
    ui->play_button->setDisabled(false);
    ui->next_button->setDisabled(false);
    ui->prev_button->setDisabled(false);
    ui->comboBox->setDisabled(false);


    QGraphicsPixmapItem * lastitem = new QGraphicsPixmapItem();
    lastitem->setPixmap(QPixmap::fromImage(animationImages.value(ui->comboBox->currentText()).at(counter)));
    scene->addItem(lastitem);
}


void DgAssets::on_prev_button_clicked()
{
    scene->clear();
    counter = counter - 1;
    if(counter < 0)
    {
        counter = animationImages.value(ui->comboBox->currentText()).size() - 1;
    }
    QGraphicsPixmapItem * previous = new QGraphicsPixmapItem();
    previous->setPixmap(QPixmap::fromImage(animationImages.value(ui->comboBox->currentText()).at(counter)));
    scene->addItem(previous);
}

void DgAssets::on_next_button_clicked()
{
    scene->clear();
    counter = counter + 1;
    if(counter >= animationImages.value(ui->comboBox->currentText()).size())
    {
        counter = 0;
    }
    QGraphicsPixmapItem * nextItem = new QGraphicsPixmapItem();
    nextItem->setPixmap(QPixmap::fromImage(animationImages.value(ui->comboBox->currentText()).at(counter)));
    scene->addItem(nextItem);
}

QImage DgAssets::shiftPixel(QImage image, int shiftsize, DgAssets::directions direction)
{
    int heightXwidth;
    heightXwidth = image.height();
    QImage shiftImage=image;
    //qDebug() << heightXwidth;
    if(direction == left)
    {
        for (int i = 0; i < heightXwidth; i++)
        {
            for (int j =0; j < heightXwidth; j++)
            {
               if (j+1 < heightXwidth)
               {
                   QColor pix = image.pixelColor(j+1,i);
                   shiftImage.setPixelColor(j,i,pix);
               }
            }
        }
    }
    else if(direction == right)
    {
        for (int i = 0; i < heightXwidth; i++)
        {
            for (int j =0; j < heightXwidth; j++)
            {
               if (j-1 > 0)
               {
                   QColor pix = image.pixelColor(j-1,i);
                   shiftImage.setPixelColor(j,i,pix);
               }
            }
        }
    }
    else if(direction == up)
    {
        for (int i = 0; i < heightXwidth; i++)
        {
            for (int j =0; j < heightXwidth; j++)
            {
               if (i+1 < heightXwidth)
               {
                   QColor pix = image.pixelColor(j,i+1);
                   shiftImage.setPixelColor(j,i,pix);
               }
            }
        }
    }
    else
    {
        for (int i = 0; i < heightXwidth; i++)
        {
            for (int j =0; j < heightXwidth; j++)
            {
               if (i-1 > 0)
               {
                   QColor pix = image.pixelColor(j,i-1);
                   shiftImage.setPixelColor(j,i,pix);
               }
            }
        }
    }
    return shiftImage;
}

void DgAssets::on_left_button_clicked()
{
    QImage tempImage = shiftPixel((animationImages.value(ui->comboBox->currentText()).at(counter)), 1, left);
    if(getMax(tempImage, left) == false)
    {
        return;
    }
    else
    {
        scene->clear();
        QGraphicsPixmapItem * leftimage = new QGraphicsPixmapItem();
        leftimage->setPixmap(QPixmap::fromImage(tempImage));
        scene->addItem(leftimage);
        QVector<QImage> images = animationImages.value(ui->comboBox->currentText());
        images[counter] = tempImage;
        animationImages[ui->comboBox->currentText()]= images;
    }
}

void DgAssets::on_right_button_clicked()
{
    QImage tempImage = shiftPixel((animationImages.value(ui->comboBox->currentText()).at(counter)), 1, right);
    if(getMax(tempImage, right) == false)
    {
        return;
    }
    else
    {
        scene->clear();
        QGraphicsPixmapItem * leftimage = new QGraphicsPixmapItem();
        leftimage->setPixmap(QPixmap::fromImage(tempImage));
        scene->addItem(leftimage);
        QVector<QImage> images = animationImages.value(ui->comboBox->currentText());
        images[counter] = tempImage;
        animationImages[ui->comboBox->currentText()]= images;
    }
}

void DgAssets::on_up_button_clicked()
{
    QImage tempImage = shiftPixel((animationImages.value(ui->comboBox->currentText()).at(counter)), 1, up);
    if(getMax(tempImage, up) == false)
    {
        return;
    }
    else
    {
        scene->clear();
        QGraphicsPixmapItem * leftimage = new QGraphicsPixmapItem();
        leftimage->setPixmap(QPixmap::fromImage(tempImage));
        scene->addItem(leftimage);
        QVector<QImage> images = animationImages.value(ui->comboBox->currentText());
        images[counter] = tempImage;
        animationImages[ui->comboBox->currentText()]= images;
    }
}

void DgAssets::on_down_button_clicked()
{
    QImage tempImage = shiftPixel((animationImages.value(ui->comboBox->currentText()).at(counter)), 1, down);
    if(getMax(tempImage, down) == false)
    {
        return;
    }
    else
    {
        scene->clear();
        QGraphicsPixmapItem * leftimage = new QGraphicsPixmapItem();
        leftimage->setPixmap(QPixmap::fromImage(tempImage));
        scene->addItem(leftimage);
        QVector<QImage> images = animationImages.value(ui->comboBox->currentText());
        images[counter] = tempImage;
        animationImages[ui->comboBox->currentText()]= images;
    }
}
void DgAssets::on_actionsave_triggered()
{
    QMap<QString,QVector<QImage> >::iterator i;
    QVector<QImage> temporary;
    int x = animationImages.first().first().width();
    int highet = 0;
    QStringList list;
    for(i = animationImages.begin();i!=animationImages.end(); ++i)
    {
        highet += i.value().length();
        //list.append(i.key());
    }
    highet *= x;
    QImage bigImage(x,highet, QImage::Format_ARGB32);
    QPainter painter;
    int currentOfset = 0;
    painter.begin(&bigImage);
    for(int j=0; j <besho.length();j++)
    {
        temporary = animationImages[besho.at(j)];
        for(int m=0; m< temporary.length(); m++)
        {
            painter.drawImage(0,currentOfset,temporary.at(m));
            currentOfset +=x;
        }
    }
    painter.end();
    QString path = curProject +"/img/" + fileName +".png";
    bigImage.save(path,0,100);
    QMessageBox::information(0,"File Saved At",path);
}
