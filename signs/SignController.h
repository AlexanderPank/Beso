#ifndef SIGNCONTROLLER_H
#define SIGNCONTROLLER_H

#include "GeoUtil.h"
#include "SignBase.h"

#include <QMap>
#include <QPair>
#include <QObject>
#include <mapapi.h>
#include <maptype.h>

class SignController: public QObject
{
    Q_OBJECT
public:


    static SignController* getInstance(HMAP hMap, HSITE hSite, QObject* parent = nullptr) ;
    static SignController *getInstance();
    static void destroyInstance();

    int addSignToMap(SignBase *sign);
    void highlightSignByID(int singId);
    int getHighlightSignID(){return _highlightSignID;} ;
    int findNearestSignID(QPointF coord, int zoom);
    void DragCurrentObject(double dx, double dy, int singId);
    void clearHighlight();
    void resizeSelectedObject(int objId, QVector<QPointF> m_editPoints);
    SignBase *getSignByID(int singId) const;
    void rotateSelectedObject(int singId, double _angle, QVector<QPointF> &m_editPoints, QPointF center);
    HMAP mapHandle(){return _hMap;}
    SignBase::SignType getSignType(int signId) const;
    void removeSign(int signId);
    void clear();

    void updateSignOnMap(int id,double lonDeg, double latDeg, double courseDeg);
    bool updateSignOnMap(int singId,  QList<QPointF> points);
    int updateSignOnMap(SignBase *);
    int redrawSignOnMap(SignBase *sign);

private:
    SignController(HMAP  hMap, HSITE hSite, QObject *parent=nullptr);

    const int NO_SIGN = -1;
    const int GIS_COORDINATE_SYSTEM_TYPE = 1;
    HMAP _hMap;
    HSITE _hSite;
    QMap<int, QPair<HOBJ, SignBase*>> signs;
    GeoUtil *_geoUtil;
    int getFreeId();
    void highlightObject(SignBase *sign, HOBJ hObj);
    const double EARTH_RADIUS_KM = 6371.0; ///< Радиус Земли в километрах

    void setHSite( HSITE hSite);
    void setHMap( HMAP hMap);

    /**
     * @brief Функция для вычисления расстояния между двумя точками на сфере (Хаверсин).
     * @param coord1 Первая точка (долгота, широта).
     * @param coord2 Вторая точка (долгота, широта).
     * @return Расстояние между точками в километрах.
     */
    double haversineDistanceInKM(const QPointF& coord1, const QPointF& coord2);
    int _highlightSignID = -1;
};



#endif // SIGNCONTROLLER_H
