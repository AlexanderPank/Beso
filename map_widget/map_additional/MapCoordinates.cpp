#include "MapCoordinates.h"
#include "ui_map_coordinates.h"
#include <math.h>
#include "../MapWidget.h"
MapCoordinates::MapCoordinates(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MapCoordinates),
    _parent(parent)
{
    ui->setupUi(this);
    // Установка специфичного класса
    setGeoXY(0., 0.);

    ui->xLine->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    showCoords(true);
    updatePosition();
}

MapCoordinates::~MapCoordinates()
{
    delete ui;
}

void MapCoordinates::setGeoXY(double xdegs, double ydegs)
{
    QString coordText = QString("%1, %2").arg(xdegs, 0, 'f', 5).arg(ydegs, 0, 'f', 5);
    ui->xLine->setText(coordText);
    ui->xLine->setFixedWidth(ui->xLine->fontMetrics().horizontalAdvance(coordText) + 20);
}

void MapCoordinates::setPlaneXY(double x, double y)
{
    long num;//, cents;
    num = x;
    auto line_x=QString::number(num) + " m";
    num = y;
    auto line_y =QString::number(num) + " m";
    ui->xLine->setText(QString("%1, %2").arg(line_x, line_y));
}


void MapCoordinates::updatePosition(){
    // Установка геометрии окна
    if (!_parent) return;

    int parentWidth = _parent->width();
    int panelWidth = this->width();
    this->setGeometry(_parent->width() -  this->width() + 10,
                      _parent->height() - 1.5 * this->height(),
                      this->width(),
                      this->height()
    ); // Отступ 10px слева
}
void MapCoordinates::showCoords(bool yes)
{
    ui->xLine->setVisible(yes);
}
