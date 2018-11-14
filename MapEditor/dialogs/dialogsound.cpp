#include "dialogsound.h"
#include "ui_dialogsound.h"


DialogSound::DialogSound(QWidget *parent,  Sound *sound , QString name) :
    QDialog(parent),
    ui(new Ui::DialogSound)
{
    soundLoaded = sound;
    tempSound = sound;

    nameType = name;
    ui->setupUi(this);
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

    pushButton = new QPushButton("Test Sound", this);


    connect(buttonBox, SIGNAL(accepted()), this, SLOT(saveComboBoxTopChoices()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(pushButton,SIGNAL(pressed()) , this, SLOT(pushButton_clicked()));


    mainLayout = new QVBoxLayout();
    horizontalInput();
    horizontalColumns();
    setComboBoxList(name.toLower());
    pushButton->setMaximumWidth(100);
    mainLayout->addWidget(pushButton);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);


}


DialogSound::~DialogSound()
{
    delete ui;
}


void DialogSound::horizontalInput(){
    QGroupBox *hGroupBox = new QGroupBox(tr("Horizontal layout"));
    QHBoxLayout *hLayout = new QHBoxLayout;
    foldersBox = new QComboBox;
    QDir dir(soundLoaded->getProjectDir());
    QStringList folders;
    QStringList files = dir.entryList();
    for(int i = 0; i < files.length(); i++ ){
        QFileInfo file(dir.absolutePath()+ "/"+files.at(i));
        if (file.isDir()){    QString filePath = "qrc:/data/default/snd";

            folders.append(files.at(i));
        }
    }
    foldersBox->addItems(folders);


    hLayout->addWidget(foldersBox);
    hGroupBox->setLayout(hLayout);

    mainLayout->addWidget(hGroupBox);
    connect(foldersBox,SIGNAL( currentIndexChanged(QString)) , this, SLOT(directoryChange(QString)) );



}

void DialogSound::directoryChange(QString name){
    QDir dir(soundLoaded->getProjectDir() + "/" + name);
    QStringList soundfound = dir.entryList();
    soundfound.removeAt(0);
    soundfound.removeAt(0);

    for(int i = 0; i < soundfound.length(); i++){
        soundfound[i] = "/" + name + "/" +soundfound.at(i);
    }


    updateComboBoxList(soundfound);
}

void DialogSound::horizontalColumns(){
    QGroupBox *hGroupBox = new QGroupBox(tr("Horizontal layout"));
    QHBoxLayout *hLayout = new QHBoxLayout;
    for(int i = 0;  i < columnList.length();  i++){
        QLabel *label = new QLabel(columnList.at(i));
        label->setAlignment(Qt::AlignRight);

        hLayout->addWidget(label);
    }
    hGroupBox->setLayout(hLayout);
    hGroupBox->setAlignment(Qt::AlignRight);
    hGroupBox->setFixedHeight(50);

    mainLayout->addWidget(hGroupBox);
}

void DialogSound::setComboBoxList(QString name){

   QGroupBox* gridGroupBox = new QGroupBox(tr("list"));
   QGridLayout *layout = new QGridLayout;

   int columns = 2*columnList.length();
   for( int j = 0; j < columns; j++ ){
       // columns label
       QStringList soundCategory = soundLoaded->getSoundTypes( name + "-" + columnList.at(j/2),true );
       QVector<QComboBox*> newComboBoxes;
       QVector<QRadioButton*> radios;
       for(int i = 0; i < soundCategory.length(); i++){
           QStringList  tempList = soundCategory;
           QComboBox *box = new QComboBox();
           QRadioButton *rbutton = new QRadioButton();
           rbutton->setMaximumWidth(20);
           // places items sorted
           tempList.removeAt(i);
           tempList.insert(0,soundCategory.at(i));
           box->addItems(tempList);
           // adding objects
           layout->addWidget(rbutton,i+1,j);
           layout->addWidget(box,i+1,j+1);
           // storing to have access to them
           radios.append(rbutton);
           newComboBoxes.append(box);

       }
       j++;
       comboBoxlist.insert(columnList.at(j/2),newComboBoxes);
       radioButtons.insert(columnList.at(j/2),radios);
   }
   gridGroupBox->setLayout(layout);

   mainLayout->addWidget(gridGroupBox);


}


void DialogSound::setName(QString name){
    nameType = name;
}

void DialogSound::saveComboBoxTopChoices(){

    for(int i = 0; i < columnList.length(); i++){
        QVector<QComboBox*> saveBoxes = comboBoxlist.value(columnList.at(i));
//        qDebug() << columnList.at(i);
        for(int j = 0; j < saveBoxes.length(); j++){
             QString itemText = saveBoxes.at(j)->currentText();
             QString type =  itemText.split("/")[1];
             if (  (itemText.split("/")[0] + "-" + type.remove(".wav") )  == nameType.toLower() + "-" + columnList.at(i) + QString().setNum(j) ) continue;
             QString newSound = soundLoaded->getSound( itemText.split("/")[0] + "-" + type.remove(".wav")  );
             if (soundLoaded->soundList.contains(itemText.split("/")[0] + "-" + type.remove(".wav")) ){
                 soundLoaded->soundList.remove(nameType.toLower() + "-" + columnList.at(i) + QString().setNum(j));
                 soundLoaded->soundList.insert(nameType.toLower() + "-" + columnList.at(i) + QString().setNum(j) , newSound );
             }


        }
    }

    soundLoaded->writeSound();
}

void DialogSound::updateComboBoxList(QStringList newItems){
    for(int i = 0; i < columnList.length(); i++){
        QVector<QComboBox*> setBoxes = comboBoxlist.value(columnList.at(i));

        for(int j = 0; j < setBoxes.length(); j++){
            setBoxes.at(j)->addItems(newItems);
        }
    }
}


void DialogSound::openDirectory(){
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setFileMode(QFileDialog::DirectoryOnly);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    if(dialog.exec() != QDialog::Accepted)
        return ;

    readDirectory(dialog.selectedFiles().first());


}

void DialogSound::readDirectory(QString path){
    QDir openDir(path);
    QStringList newSounds = openDir.entryList();
    newSounds.removeFirst();
    newSounds.removeFirst();
    updateComboBoxList(newSounds);
    QString dir = path.split("/").last();
    for(int i = 0; i < newSounds.length(); i++){
        QString str = newSounds.at(i);
        openFiles.insert(newSounds.at(i), path);
    }
}



void DialogSound::pushButton_clicked()
{

    for(int i = 0; i < columnList.length(); i++){
        QString category = columnList.at(i);
        QVector<QRadioButton*> radioList =radioButtons.value(category);
        QString path = "";
        for( int j = 0; j < radioList.length(); j++ ){

            if ( radioList.at(j)->isChecked() ){

                QString selectSound =  comboBoxlist.value(category).at(j)->currentText();
                if ( soundLoaded->getSound(selectSound) == ""){
                    path = "";
                }
                soundLoaded->playSound( "./" + selectSound  );
                break;
            }
        }
    }

}
