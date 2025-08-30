#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#define HIDEMAXMIN
#include <QWidget>
#include <QPointF>
#include <QPainter>

#include "qmapcomponents/qdmcmp.h"
#include "map_additional/MapCoordinates.h"
#include "map_additional/MapScale.h"
#include "map_additional/MapToolPanel.h"
#include "../signs/SignController.h"
#include "qdmlayr.h"
#include "qdmobj.h"
#include "map_tools/CoordCtx.h"
#include "map_tools/SignEditor.h"
#include "map_enums.h"
#include "map_tools/SignDrawer.h"
#include "map_additional/MapInformer.h"


namespace Ui {
class MapWidget;
}

class MapWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MapWidget(QWidget *parent, QString mapFileName, QString rscPath);
    ~MapWidget();
    int loadMap(QString file);
    QDMapView *getMapView() const {return mapView;}
    void addSignToMap();

    bool setScale(long scale);
    QPointF getMapCenter() const;

signals:
    void mouseMovedGeoDeg(QPointF);
    void mouseMoved(long picx, long picy);
    void mouseMovedGeo(double geox, double geoy);


public slots:
    void onSetMapCenter(float lat, float lon, int zoom);
    void onClearMap();
    void onRepaint();
    void onSelectSignOnMap(SignBase *sign);


protected:
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void onPainted(QPainter *p, int cx, int cy, int cw, int ch);

    void onMouseMove(int x, int y, int modkeys);
    void onMousePressed(int x, int y, int modkeys);
    void onMouseReleased(int x, int y, int modkeys);
    void onMouseDblClick(int x, int y, int modkeys);

  //  void onKeyPressed(int key, int count, int state);

private slots:
    void doRepaint(); // вызывается с задержкой

private:



    void wheelEvent(QWheelEvent *event);
    void showEditPoints(int mode);


    SignEditor* m_signEditor = nullptr;
    SignDrawer* m_signDrawer = nullptr;

    int m_editMode = EditMode::None;
    int m_selectedPointIndex = -1;
    QVector<QPointF> m_editPoints;  // Точки редактирования текущего объекта
    QRectF m_boundingRect;          // Ограничивающий прямоугольник
    QPoint m_dragEditPoint;

    Ui::MapWidget *ui;
    QDMapView *mapView;
    HMAP _hMap;
    QString currentMapFile;

    int scaleIndex = 5;
    MapCoordinates *coordinateView;
    MapScale *scaleView;

    MapToolPanel *toolPanel = nullptr;
    MapInformer *mapInformer = nullptr;
    bool m_showSignLabels = true;
    bool m_showFeatureLabels = false;

    bool mousePressed = false;
    int pressedX = 0;
    int pressedY = 0;
    CoordCtx *_pressedCoord;
    QPointF m_mousePosPic;
    QPointF m_mousePosDeg;

    void moveMap(int x, int y, int xLeftTop, int yLeftTop) const;
    HSITE loadRsc(QString mapName, QString rscFile);

    // Функции редактирования

    void updateBoundingRect();
    void rotateSelectedObject(qreal angle);

    HSITE _hSite_violit;
    HSITE _hSite_kvo2;


    SignController* _signController= nullptr;

    int _selectedSignID = -1; ///< id выбранного знака
    bool _isCtrlPressed = false; ///< нажата ли клавиша Ctrl для перемещения объектов на карте
    QTimer m_repaintTimer;
    int m_pendingRepaintCount = 0;
    const int MAX_REPAINT_COUNT = 100;

};

#endif // MAPWIDGET_H
