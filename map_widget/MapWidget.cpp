
#include "MapWidget.h"
#include "ui_mapwidget.h"
#include "logapi.h"
#include "map_enums.h"
#include <QDebug>
#include <QPointF>

#define NOMINMAX
#include <valarray>
#include <algorithm>  // Обязательно для std::min и std::max
#include <QFileInfo>


#include "map_tools/CoordCtx.h"

#include "../signs/SignShip.h"
#include "../core/QLogStream.h"

MapWidget::MapWidget(QWidget *parent, QString mapFileName, QString rscPath) :
    QWidget(parent),
    ui(new Ui::MapWidget) {
    ui->setupUi(this);
    mapView = ui->mapView;
    setFocusPolicy(Qt::StrongFocus);
    mapInformer = new MapInformer(this);
    mapInformer->hide();

    coordinateView = new MapCoordinates(this);
    scaleView = new MapScale(this);
    connect(scaleView, &MapScale::scaleChanged, [=](long scale) { setScale(scale); });

    toolPanel = new MapToolPanel(this);

    connect(toolPanel, &MapToolPanel::signLabelsToggled, [=](bool checked) {
        m_showSignLabels = !m_showSignLabels;
        getMapView()->Repaint();
    });

    connect(toolPanel, &MapToolPanel::featureLabelsToggled, [=](bool checked) {
        m_showFeatureLabels = !m_showFeatureLabels;
        getMapView()->Repaint();
    });

//    connect(toolPanel, &MapToolPanel::propertySignClicked, this, &MapWidget::multiSelectStart);
//    connect(toolPanel, &MapToolPanel::addPointClicked, this, &MapWidget::multiSelectStart);
//    connect(toolPanel, &MapToolPanel::removePointClicked, this, &MapWidget::multiSelectStart);
    connect(toolPanel, &MapToolPanel::rotateSignClicked, [=]() {
        if (m_signEditor->hasSelection()) {
            m_signEditor->showEditPoints(EditMode::RotateObject);
            getMapView()->Repaint();
        }
    });
    connect(toolPanel, &MapToolPanel::changeSignClicked, [=]() {
        if (m_signEditor->hasSelection()) {
            m_signEditor->startResizing();
            getMapView()->Repaint();
        }
    });
    connect(toolPanel, &MapToolPanel::removeSignClicked, [=]() {
        if (!m_signEditor->hasSelection()) return;
        m_signEditor->removeSelectedSign();
        getMapView()->Repaint();
    });
    ///////
    toolPanel->show();

    connect(mapView, &QDMapView::SignalAfterPaint, this, &MapWidget::onPainted);
    connect(mapView, &QDMapView::SignalMouseMove, this, &MapWidget::onMouseMove);
    connect(mapView, &QDMapView::SignalMousePress, this, &MapWidget::onMousePressed);
    connect(mapView, &QDMapView::SignalMouseRelease, this, &MapWidget::onMouseReleased);
    connect(mapView, &QDMapView::SignalMouseDoubleClick, this, &MapWidget::onMouseDblClick);
    //connect(mapView, &QDMapView::SignalKeyPress, this,   &MapWidget::onKeyPressed);
    //connect(mapView, &QDMapView::SignalKeyRelease, this, &MapWidget::onKeyReleased);
    connect(this, &MapWidget::mouseMovedGeoDeg, [=](QPointF p) {
        coordinateView->setGeoXY(p.x(), p.y());
    });
    setMouseTracking(true);
    // важно чтоб было внизу т.к. работает с scaleView который может еще не существовать
    //_hMap =
    loadMap(mapFileName);
    // _hSite_violit = loadRsc("violit2016_mod", rscPath + "/violit2016_mod.rsc");
    //_hSite_kvo2 = loadRsc("kvo2_mod", rscPath + "/kvo2_mod.rsc");
    // _hSite_kvo2 = loadRsc("operator", rscPath + "/operator.rsc");
//    _hSite_kvo2 = loadRsc("operator", rscPath + "/operator.rsc");
//    //_hSite_kvo2 = loadRsc("operator", rscPath + "/mgk.rsc");
//
//    _signController = SignController::getInstance(_hMap, _hSite_kvo2, this);


    m_signDrawer = new SignDrawer(_hMap, this);
    connect(m_signDrawer, &SignDrawer::signDrawing, [=](const QVector<QPointF> &points, const double distance) {
        mapInformer->setText(QString("%1 м.").arg(distance, 3));
    });

    connect(m_signDrawer, &SignDrawer::signDrawn, [=](SignBase::SignType, const QVector<QPointF> &points) {
        if (toolPanel->isRouletteOn()) {
            m_signDrawer->clear();
            mapInformer->hideLabel();
            getMapView()->update();
            toolPanel->clearState();
        }
    });

    ///////
    connect(toolPanel, &MapToolPanel::rouletteClicked, [=]() {
        if (toolPanel->isRouletteOn()) {
            m_signDrawer->clear();
            mapInformer->hideLabel();
            getMapView()->update();
            toolPanel->clearState();
        } else {
            m_signDrawer->startDrawing(SignBase::SignType::LOCAL_LINE);
            mapInformer->show();
        }
    });

    m_signEditor = new SignEditor(this);

    m_repaintTimer.setInterval(300);  // задержка 300 мс
    m_repaintTimer.setSingleShot(true);  // одноразовый запуск
    connect(&m_repaintTimer, &QTimer::timeout, this, &MapWidget::doRepaint);
}

MapWidget::~MapWidget(){ delete ui;}


int MapWidget::loadMap(QString mapFileName) {

    if (mapFileName == currentMapFile) return 1;

    currentMapFile = mapFileName;
    mapView->SetMapFileName(currentMapFile);
    mapView->SetMapActive(TRUE);
    mapView->SetMapVisible(TRUE);
    HMAP hMap = getMapView()->GetMapHandle();
    mapLogAccess(hMap, hMap, 0);
    setScale(25000);
    scaleView->setScale(25000);
    _hMap = hMap;

    QFileInfo fileInfo(mapFileName);
    QString mapFilePath = fileInfo.absolutePath();


    _hSite_kvo2 = loadRsc("operator", mapFilePath + "/operator.rsc");
    if (_hSite_kvo2 == 0)  return -1;
    //_hSite_kvo2 = loadRsc("operator", rscPath + "/mgk.rsc");

    _signController = SignController::getInstance(_hMap, _hSite_kvo2, this);
    if (m_signDrawer!= nullptr) m_signDrawer->setHMap(hMap);

    //    setRscFolder(newRscFolder);
    /*    int layerCount = mapGetLayerCount(hMap);
        for (int i = 0; i < layerCount; i++) {
            QString layerName =  gis::utils::GetCoreCodec()->toUnicode(mapGetLayerName(hMap, i));
            layers << layerName;
        }
    */
    return 1;
}

HSITE MapWidget::loadRsc(QString mapName, QString rscFile) {

    CREATESITE createsite;

    memset(&createsite, 0, sizeof(CREATESITE));
    HSITE hSite = mapCreateAndAppendTempSite(_hMap,  rscFile.toUtf8());

    return hSite;
}

void MapWidget::resizeEvent(QResizeEvent *){
    coordinateView->updatePosition();
    toolPanel->updatePosition();
    mapInformer->updatePosition();
    scaleView->move(10, rect().height() - 1.5 * scaleView->height());
}

void MapWidget::wheelEvent(QWheelEvent* event) {
    int zoomDelta = event->angleDelta().y() > 0 ? -1 : 1;
    int zoomIndex = scaleView->getZoomIndex(zoomDelta);
    onSetMapCenter(m_mousePosDeg.x(), m_mousePosDeg.y(), zoomIndex);
}

/*void MapWidget::wheelEvent(QWheelEvent *event) {
    qLog() << "wheelEvent";

    // 1. Получаем текущие координаты курсора в пикселях карты
    QPointF mousePosPic = m_mousePosPic;

    // 2. Получаем текущий масштаб и вычисляем новый
    long currentScale = mapView->GetViewScale();
    bool zoomIn = (event->angleDelta().y() < 0);
    int newScale = scaleView->getZoomIndex(zoomIn ? 1 : -1);

    // 3. Вычисляем коэффициент изменения масштаба
    double scaleRatio = static_cast<double>(currentScale) / newScale;

    // 4. Получаем текущее смещение карты
    int currentX, currentY;
    mapView->GetMapLeftTop(&currentX, &currentY);

    // 5. Вычисляем смещение курсора относительно текущего центра
    QRect viewRect = mapView->rect();
    QPointF viewCenter(viewRect.width() / 2.0, viewRect.height() / 2.0);
    QPointF mouseOffset = mousePosPic - QPointF(currentX, currentY) - viewCenter;

    // 6. Вычисляем новое положение карты с учетом масштабирования
    int newX = mousePosPic.x() - viewCenter.x() - mouseOffset.x() * scaleRatio;
    int newY = mousePosPic.y() - viewCenter.y() - mouseOffset.y() * scaleRatio;

    // 7. Применяем изменения
    mapView->SetMapLeftTop(newX, newY);
    scaleView->changeScale(newScale);
    mapView->update();
}*/

bool MapWidget::setScale(long newscale) {
    if (newscale < 0) return false;
    mapView->SetViewScale(newscale);
    return true;
}



void MapWidget::onMouseMove(int x, int y, int buttons) {


    int xLeftTop, yLeftTop;
    mapView->GetMapLeftTop(&xLeftTop, &yLeftTop);
    auto  coords = CoordCtx(_hMap, PIC, xLeftTop + x, yLeftTop + y);

    // нужно для центрирования когда крутиться колесо мыши, чтобы избежать тремора мыши
    if ( abs(m_mousePosPic.x() - coords.pic().x()) > 10 || abs(m_mousePosPic.y() - coords.pic().y()) > 10 ){
        m_mousePosDeg = QPointF(coords.deg());
        m_mousePosPic = QPointF(coords.pic());
    }

    //emit mouseMoved(xLeftTop + x, yLeftTop + y);
    emit mouseMovedGeoDeg(coords.deg());

    if (m_signDrawer->isDrawing()) {
        m_signDrawer->handleMouseMove(QPoint(x + xLeftTop, y + yLeftTop));
        update();
        return;
    }

    if (!mousePressed) return;

    QPoint currentPos(x + xLeftTop, y + yLeftTop);

    // Если нажат Ctrl и выделен знак то двигаем знак
//    if ( _isCtrlPressed && _selectedSignID != -1) {
//        auto dx = coords.geo().x() - _pressedCoord->geo().x();
//        auto dy = coords.geo().y() - _pressedCoord->geo().y();
//        qLog() << dx << dy;
//        if (  dx  != 0  ||  dy  != 0) {
//            _signController->DragCurrentObject(dx, dy, _selectedSignID);
//            getMapView()->Repaint();
//            _pressedCoord->setGeo(coords.geo());
//        }
//
//    }
//    else
    //        if (m_selectedPointIndex >=0 and m_editMode == EditMode::RotateObject ) {
    //        QPointF center = m_editPoints.last();
    //        CoordCtx ctx(_hMap, PIC, currentPos);
    //        QPointF newPos = ctx.geo();
    //
    //        qreal angle = atan2(newPos.y() - center.y(), newPos.x() - center.x()) -
    //                      atan2(m_editPoints[m_selectedPointIndex].y() - center.y(),
    //                            m_editPoints[m_selectedPointIndex].x() - center.x());
    //        _signController->rotateSelectedObject(_selectedSignID, angle * 180 / M_PI, m_editPoints);
    //        updateBoundingRect();
    //        getMapView()->Repaint();
    //    }

    if (m_signEditor->handleMouseMove(QPoint(x + xLeftTop, y + yLeftTop), _isCtrlPressed)) {
        getMapView()->Repaint();
    }
    else
    {
        // двигаем крату
        auto dx = std::abs(pressedX - x), dy = std::abs(pressedY - y);
        if (dx > 3 || dy > 3) moveMap(x, y, coords.pic().x()-x, coords.pic().y()-y);
    }
    pressedX = x;
    pressedY = y;

}


void MapWidget::onMousePressed(int x, int y, int button) {

    if (button != Qt::LeftButton) return;


    mousePressed = true;
    pressedX = x;
    pressedY = y;


    setCursor(Qt::PointingHandCursor);
    int xLeftTop, yLeftTop;
    getMapView()->GetMapLeftTop(&xLeftTop, &yLeftTop);
    _pressedCoord = new CoordCtx(_hMap, PIC, xLeftTop + x, yLeftTop + y);

    QPoint clickPos(x + xLeftTop, y + yLeftTop);

    if (m_signDrawer->isDrawing()) {
        m_signDrawer->handleMouseClick(QPointF(xLeftTop + x, yLeftTop + y));
        getMapView()->Repaint();
    }

    if (!m_signEditor->hasSelection()) {
        _signController->clearHighlight();
        getMapView()->Repaint();
        toolPanel->setSignSelected(false);
    }
    if (m_signEditor->handleMousePress(clickPos)) {
        toolPanel->setSignSelected(m_signEditor->hasSelection());
        getMapView()->Repaint();
    } else if (m_signEditor->editMode() == EditMode::Select) {
        m_signEditor->clearSelection();
        _signController->clearHighlight();
        toolPanel->setSignSelected(false);
        getMapView()->Repaint();
    }

}

void MapWidget::onMouseReleased(int x, int y, int ) {
    mousePressed = false;
    pressedX = x;
    pressedY = y;
    setCursor(Qt::ArrowCursor);
    m_signEditor->handleMouseRelease();
}

void MapWidget::onMouseDblClick(int x, int y, int modkeys){

    if (m_signDrawer->isDrawing()) {
        m_signDrawer->handleMouseDoubleClick();
        getMapView()->Repaint();
    }
}

void MapWidget::moveMap(int x, int y, int xLeftTop, int yLeftTop) const {
    // Вычисляем смещение курсора
    int dx = pressedX - x;
    int dy = pressedY - y;

    // Получаем размеры карты
    long mapHeight, mapWidth;
    getMapView()->GetMapHW(&mapHeight, &mapWidth);

    // Вычисляем новые координаты левого верхнего угла
    long newXLeftTop = xLeftTop + dx;
    long newYLeftTop = yLeftTop + dy;

    // Ограничиваем новые координаты, чтобы не выйти за границы карты
    xLeftTop = (std::max)(0L, (std::min)(newXLeftTop, mapWidth - getMapView()->contentsRect().width() - 1));
    yLeftTop = (std::max)(0L, (std::min)(newYLeftTop, mapHeight - getMapView()->contentsRect().height() - 1));

    // Устанавливаем новые координаты левого верхнего угла
    getMapView()->SetMapLeftTop(xLeftTop, yLeftTop);
}



void MapWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Control) { _isCtrlPressed = true;}
}

void MapWidget::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Control) { _isCtrlPressed = false;}
}

void MapWidget::updateBoundingRect() {
    // Функция обновления ограничивающего прямоугольника
    if (m_editPoints.isEmpty()) {
        m_boundingRect = QRectF();
        return;
    }

    m_boundingRect = QRectF(m_editPoints.first(), QSizeF(0, 0));

    for (const QPointF& pt : m_editPoints) {
        if (pt.x() < m_boundingRect.left()) m_boundingRect.setLeft(pt.x());
        if (pt.x() > m_boundingRect.right()) m_boundingRect.setRight(pt.x());
        if (pt.y() < m_boundingRect.top()) m_boundingRect.setTop(pt.y());
        if (pt.y() > m_boundingRect.bottom()) m_boundingRect.setBottom(pt.y());
    }
}

void MapWidget::onPainted(QPainter *p, int cx, int cy, int cw, int ch) {
    p->setRenderHint(QPainter::Antialiasing,true);

    m_signEditor->onPaint(p, cx, cy);
    if (m_signDrawer->isDrawing()) {
        m_signDrawer->handlePaint(p, cx, cy, cw, ch);
    }
    m_signDrawer->setFeatureLabelVisibility(m_showFeatureLabels);
    if (m_showSignLabels)
        m_signDrawer->drawNameLabels(p, cx, cy);

}

void MapWidget::onSetMapCenter(float lat, float lon, int zoom) {
    qLog() << lon << lat << zoom;
    auto coord = GeoUtil::toRadians({ {lat, lon} });

    CoordCtx coords(_hMap, GEO, coord[0].x(), coord[0].y());
    long h = getMapView()->rect().height();
    long w = getMapView()->rect().width();

    getMapView()->SetMapLeftTop(coords.pic().x() - w / 2, coords.pic().y() - h / 2);
    getMapView()->update();
    if (zoom == -1) zoom = scaleView->getZoomIndex();
    scaleView->changeScale(zoom);
    m_signEditor->setZoom(zoom);
}

QPointF MapWidget::getMapCenter() const {
    QPointF centerMap(0, 0);
    if (!_hMap) return centerMap;

    if (mapView == NULL) return centerMap;

    if (mapView->GetMapHandle() == 0) return centerMap;

    POINT leftTop;
    mapView->GetMapLeftTop(&leftTop.x, &leftTop.y);
    mapView->TranslatePicturePointFromViewFrame(&leftTop);

    centerMap = QPointF(leftTop.x + mapView->rect().width() / 2,  leftTop.y + mapView->rect().height() / 2);
    auto  coords = CoordCtx(_hMap, PIC, centerMap.x(), centerMap.y());
    centerMap = coords.deg();
    return centerMap;
}

void  MapWidget::onSelectSignOnMap(SignBase *sign){
    if (!sign) return;
    m_signEditor->selectSign(sign);
    getMapView()->Repaint();
    toolPanel->setSignSelected(true);
}


void MapWidget::onRepaint(){
    m_pendingRepaintCount++;

    // если таймер не работает — запускаем
    if (!m_repaintTimer.isActive())
        m_repaintTimer.start();

    // если накопилось много вызовов — рисуем немедленно
    if (m_pendingRepaintCount >= MAX_REPAINT_COUNT) {
        m_repaintTimer.stop();
        doRepaint();
    }
}

void MapWidget::doRepaint()
{
    qDebug() << "doRepaint";
    m_pendingRepaintCount = 0;
    getMapView()->Repaint();
}

void MapWidget::onClearMap(){
    _signController->clear();
    getMapView()->Repaint();
}


