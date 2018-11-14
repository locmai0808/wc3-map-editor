#include "dgmapproperties.h"
#include "ui_dgmapproperties.h"

DgMapProperties::DgMapProperties(MapView2* map, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::DgMapProperties)
{
    ui->setupUi(this);
    curMap = map;

    ui->txt_name->setText(curMap->getMapName());
    ui->txt_width->setText(QString::number(curMap->getMapDim().width()));
    ui->txt_height->setText(QString::number(curMap->getMapDim().height()));
    ui->txt_description->document()->setPlainText(curMap->getMapDescription());
}

DgMapProperties::~DgMapProperties()
{
    delete ui;
}

void DgMapProperties::on_buttonBox_accepted()
{
    curMap->setMapName(ui->txt_name->text().trimmed());
    curMap->setMapDescription(ui->txt_description->document()->toPlainText());
}
