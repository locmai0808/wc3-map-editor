#ifndef IMPORTFROMWEB_H
#define IMPORTFROMWEB_H

#include <QWidget>

namespace Ui {
class ImportFromWeb;
}

class ImportFromWeb : public QWidget
{
    Q_OBJECT

public:
    explicit ImportFromWeb(QWidget *parent = 0);
    ~ImportFromWeb();

private:
    Ui::ImportFromWeb *ui;
};

#endif // IMPORTFROMWEB_H
