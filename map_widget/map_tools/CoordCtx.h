#ifndef COORD_CTX_H
#define COORD_CTX_H

#include <QPoint>
#include "../map_enums.h"
#include <maptype.h>



class CoordCtx {
public:
    CoordCtx(HMAP hMap, CoordType type, QPointF p);
    CoordCtx(HMAP hMap, CoordType type, double x, double y);
    QPointF geo() const;
    QPointF deg() const;
    QPointF plane() const;
    QPoint pic() const;

    QPointF plane42() const;

    static double toDegrees(double v) ;
    static double toRadians(double v) ;
    void setGeo(QPointF p) {_geo = p;}

private:
    QPointF _plane;
    QPointF _geo;
    QPoint _pic;
    QPointF _plane42;

};

#endif // COORD_CTX_H
