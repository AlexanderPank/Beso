#ifndef MAP_COORDINATES_H
#define MAP_COORDINATES_H

#include <QWidget>


namespace Ui {
class MapCoordinates;
}

class MapCoordinates : public QWidget
{
    Q_OBJECT

public:
    explicit MapCoordinates(QWidget *parent);
    ~MapCoordinates();
    void updatePosition();
public slots:
    void setGeoXY(double x, double y);
    void setPlaneXY(double x, double y);
    void showCoords(bool yes);
private:
    Ui::MapCoordinates *ui;
    QWidget *_parent= nullptr;
};

#endif // MAP_COORDINATES_H
