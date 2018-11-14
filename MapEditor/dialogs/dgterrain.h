#ifndef DGTERRAIN_H
#define DGTERRAIN_H

#include <QDialog>

namespace Ui {
class dgterrain;
}

class dgterrain : public QDialog
{
    Q_OBJECT

public:
    explicit dgterrain(QWidget *parent = 0);
    ~dgterrain();

private:
    Ui::dgterrain *ui;
};

#endif // DGTERRAIN_H
