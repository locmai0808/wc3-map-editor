#ifndef SOUND_H
#define SOUND_H
#include <QMediaPlayer>
#include <QFile>
#include <QMessageBox>
#include <QMediaPlayer>
#include <QDir>

struct Soundfile{
    QString filename;
    QString path_src;
};

class Sound
{
public:
    Sound(QString location,QString PathToSave );
    QMap<QString,QString> soundList;



    QString getSound(QString name);
    QStringList getSoundTypes(QString type, bool nameOnly );
    void setSound();
    void loadSound();
    void playSound(QString file);
    void addSound(QString src_key, QString file);
    void writeSound();
    QString getProjectDir();
    void readProjectDir();

private:
    QMap<QString,QString> soundList2;
    QString fileLocation;
    QMediaPlayer * music;
    QString projectPath;


};

#endif // SOUND_H
