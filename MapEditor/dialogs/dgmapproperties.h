#ifndef DGMAPPROPERTIES_H
#define DGMAPPROPERTIES_H

#include <QDialog>
#include "mapview2.h"

/* Name: dgmapproperties.h
 * Class: DgMapProperties
 *
 * This file includes the class for the Map Properties dialog box.
 * Create an instance and then run exec()
 *
 */

namespace Ui {
class DgMapProperties;
}

class DgMapProperties : public QDialog
{
    Q_OBJECT

public:
    explicit DgMapProperties(MapView2* map, QWidget *parent = 0);
    ~DgMapProperties();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::DgMapProperties *ui;
    MapView2* curMap;
};

#endif // DGMAPPROPERTIES_H
