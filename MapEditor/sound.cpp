#include "sound.h"

Sound::Sound(QString location, QString PathToSave)
{
    fileLocation = location;
    projectPath = PathToSave + "snd/";
    loadSound();
    music = new QMediaPlayer();

    readProjectDir();


}

QString Sound::getProjectDir(){
    return projectPath;
}

void Sound::readProjectDir(){
    QDir dir(projectPath);
    foreach( QString file ,dir.entryList()){
        QFileInfo fileInfo(projectPath+file );
        if(fileInfo.isDir() && file  != "." && file != ".." ){
            foreach( QString soundFile , QDir(projectPath+file).entryList()){
//                qDebug() << projectPath + file + "/" +  soundFile;
                if( !QFileInfo(projectPath + file + "/" +  soundFile).isDir() && soundFile != "." && soundFile != ".."){
                    if(file == "music"){
                        continue;
                    }
                    QString filekey = file + "-" + soundFile.remove(".wav");
                    if( !soundList.contains(filekey) && soundFile != "music" ){
//                        qDebug() << "found a new file: " << filekey;
                        soundList.insert(filekey, "./"+file + "/" + soundFile);
                    }
                }
            }
        }

    }

}

void Sound::loadSound(){
    QFile file(fileLocation);
    if ( !file.open(QIODevice::ReadOnly) ){
         QMessageBox::information(0,"error open file" , "Sound");
    }
    int lineN = 0;
    QTextStream in(&file);
    while(!in.atEnd()){
        int assetNum = in.readLine().toInt();
        QString soundName = "";
        QString soundPath = "";
        for(int i = 0; i < assetNum; ++i){
             soundName = in.readLine();
             soundPath = in.readLine();
            soundList.insert(soundName,soundPath);
        }

        // skip generalsound.sf2
        in.readLine();

        int gameSoundN = in.readLine().toInt();
        for(int i = 0; i <  gameSoundN; ++i){
            soundName = in.readLine();
            soundPath = in.readLine();

            soundList2.insert(soundName,soundPath);
        }

    }

}

void Sound::playSound(QString name){
    QString file = "";
//    QString filePath = projectPath;
//    QString filePath = ":/data/default/snd";
    QString filePath = "qrc:/data/default/snd";




    file = name; // soundList.value(name);
    filePath += file.remove(0,2);
    qDebug() << filePath;

//    filePath = "/home/aesoria/blaba/snd/basic/annoyed1.wav";
    music->setMedia(QUrl(filePath));
    music->play();
//    music->stop();
}

void Sound::addSound(QString src_key, QString file){
    if (!soundList.contains(src_key)){
        soundList.insert(src_key,file);
    }
    else{
        qDebug() << "sound source name key already exists";
    }

}

void Sound::writeSound(){
//    qDebug() << "here is the path " << projectPath + "SoundClips.dat";
    QFile file( projectPath+"SoundClips.dat");
    file.open(QIODevice::ReadWrite);

    QTextStream stream(&file);
    stream << soundList.size() << endl;
    QList<QString> list = soundList.keys();
    for(int i = 0; i < soundList.size();  i++){
        stream << list.at(i) << endl;
        stream << soundList[list.at(i)] << endl;

    }
    stream << "./generalsoundfont.sf2" << endl;
    stream << soundList2.size() << endl;
    QList<QString> list2 = soundList2.keys();
    for(int i = 0; i < soundList2.size();  i++){
        stream << list2.at(i) << endl;
        stream << soundList2[list2.at(i)] << endl;

    }


}

QStringList Sound::getSoundTypes(QString soundtype, bool nameOnly){

    QStringList soundclips;
    if(soundList.empty())
        return soundclips;

    int limit = 20;
    for(int i = 0; i < limit; i++){
        if(i == 0){
            if(soundList.contains(soundtype)){
                soundclips.append(soundList.value(soundtype) );
                break;
            }
        }
        else {
            QString findSound = soundtype + QString().setNum(i);
            if (soundList.contains(findSound) )
                soundclips.append(soundList.value(findSound));
        }

    }

    for( int i = 0; i < soundclips.length() && nameOnly; i++ ){
        QString rename = soundclips[i];
        soundclips[i] =  rename.remove(0,1);

    }

    return soundclips;

}

QString Sound::getSound(QString name){
    return soundList.value(name);
}
