#include "itemtoolbox.h"
#include "ui_itemtoolbox.h"

ItemToolBox::ItemToolBox(QWidget *parent) :
    QToolBox(parent),
    ui(new Ui::ItemToolBox)
{
    ui->setupUi(this);
//    addItem()
}

ItemToolBox::~ItemToolBox()
{
    delete ui;
}
