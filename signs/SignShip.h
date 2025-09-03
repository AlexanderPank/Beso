#ifndef SIGNSHIP_H
#define SIGNSHIP_H

#include "SignBase.h"

class SignShip: public SignBase{
    public: SignShip(QList<QPointF> coord, QString name);
};

class SignShipEnemy: public SignShip{
    public: SignShipEnemy(QList<QPointF> coord, QString name);
};
class SignRegion: public SignBase{
    public: SignRegion(QList<QPointF> coord, QString name);
};

class SignWay: public SignBase{
    public: SignWay(QList<QPointF> coord, QString name);
};


class SignWayEnemy: public SignWay{
    public: SignWayEnemy(const QList<QPointF> coord,const QString name);
};


class SignIntelligencePlane: public SignBase{
    public: SignIntelligencePlane(QList<QPointF> coord, QString name);
};

class SignAntiBoatPlane: public SignBase{
    public: SignAntiBoatPlane(QList<QPointF> coord, QString name, bool is_own);
};

class SignAntiSubmarineBoat: public SignBase{
    public: SignAntiSubmarineBoat(QList<QPointF> coord, QString name);
};
class SignAntiSubmarineBoatEnemy: public SignAntiSubmarineBoat{
    public: SignAntiSubmarineBoatEnemy(QList<QPointF> coord, QString name);
};

class SignNavalBase: public SignBase{
    public: SignNavalBase(QList<QPointF> coord, QString name);
};

class SignNavalBaseEnemy: public SignBase{
    public: SignNavalBaseEnemy(QList<QPointF> coord, QString name);
};

class SignInterceptPoint: public SignBase{
    public: SignInterceptPoint(QList<QPointF> coord, QString name);
};

class SignWingsRocket: public SignBase{
    public: SignWingsRocket(QList<QPointF> coord, QString name);
};


class SignWingsRocketEnemy: public SignWingsRocket{
    public: SignWingsRocketEnemy(QList<QPointF> coord, QString name);
};

class SignRocketLaunchLine: public SignBase{
    public: SignRocketLaunchLine(QList<QPointF> coord, QString name);
};

class SignRegionSearch: public SignBase{
    public: SignRegionSearch(QList<QPointF> coord, QString name);
};
class SignRegionRound: public SignBase{
    public: SignRegionRound(QList<QPointF> coord, QString name);
};

class SignCircle: public SignBase{
public:
    SignCircle(QList<QPointF> coord, QString name, double radius, QString classCode="31261500011300000000");
    void setCoordinatesInDegrees(QList<QPointF> coord, double radius, bool emit_signals=true);
    void setCoordinatesInRadians(QList<QPointF> coord, double radius, bool emit_signals=true);
    float getRadius() const {return m_radius;}
private:
    double m_radius;
};

class SignSector: public SignBase{
public:
    SignSector(QList<QPointF> coordRad, QString name,  QPointF targetRad, double angleWidth,double distance);
};

class SignWaitingRegion: public SignBase{
public:
    SignWaitingRegion(QList<QPointF> coordRad, QString name,  bool isOwn);
};

class SignDirectionAction: public SignBase{
public:
    SignDirectionAction(QList<QPointF> coordRad, QString name,  bool isOwn);
};

class SignRestrictedArea: public SignBase{
public:
    SignRestrictedArea(QList<QPointF>  coordRad, QString name);
};

class SignTitle: public SignBase{
public:
    SignTitle(QList<QPointF> coordRad, QString name, QString text, float fontSize = 12);
    float getFontSize();
    void setFontSize( float size);
    void setTitleText(const QString text);

private:
    float m_font_size;
};

class SignBorderOES: public SignBase{
public:
    SignBorderOES(QList<QPointF>  coordRad, QString name);
    void setWidthAndLength(double sensor_width_mm, double sensor_height_mm, double f_mm, double h);
    void redraw(QPointF centerInDegrees, double bearing);

private:
    QPair<double, double> CalculateWidthAndLength();
    QList<QPointF>  buildCoveragePolygon(double lat0,     // центральная точка, град.
                                           double lon0,     // центральная точка, град.
                                           double widthM,   // ширина FOV, м
                                           double heightM,  // высота FOV, м
                                           double bearing); // направление «верха» кадра, град.

    double m_width = 0;
    double m_height = 0;
};


class SignRLS: public SignCircle{
public:
    SignRLS(QList<QPointF> coord, QString name, double radius, QString classCode="L0000003641u");
};

class SignBPLADistance: public SignCircle{
public:
    SignBPLADistance(QList<QPointF> coord, QString name, double radius, QString classCode="C121055000334");
};

class SignAKDistance: public SignCircle{
public:
    SignAKDistance(QList<QPointF> coord, QString name, double radius);
};




#endif // SIGNSHIP_H
