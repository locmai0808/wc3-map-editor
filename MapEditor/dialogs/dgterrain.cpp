#include "dgterrain.h"
#include "ui_dgterrain.h"

dgterrain::dgterrain(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dgterrain)
{
    ui->setupUi(this);
}

dgterrain::~dgterrain()
{
    delete ui;
}
