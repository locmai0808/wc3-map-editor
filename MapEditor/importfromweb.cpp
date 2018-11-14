#include "importfromweb.h"
#include "ui_importfromweb.h"

ImportFromWeb::ImportFromWeb(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImportFromWeb)
{
    ui->setupUi(this);
}

ImportFromWeb::~ImportFromWeb()
{
    delete ui;
}
