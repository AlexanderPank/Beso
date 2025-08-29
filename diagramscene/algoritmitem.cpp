#include "algoritmitem.h"

#include "diagramitem.h"
#include "arrow.h"

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>

AlgoritmItem::AlgoritmItem(AlgoritmType diagramType, QMenu *contextMenu, QString title, QList<QPair<QString,QString>> in , QList<QPair<QString,QString>> out ,
                           QGraphicsItem *parent)
      : QGraphicsItem(parent), myDiagramType(diagramType)
      , myContextMenu(contextMenu)
  {
      QPainterPath path;

      int koef_x = 1;
      int koef_y = 1;

      titleItem = new QGraphicsTextItem(title,this);
      QFont font = titleItem->font();
      font.setBold(true);
      font.setUnderline(true);
      titleItem->setFont(font);

      int inTextsize = 0;
      int outTextsize = 0;

      foreach (auto pair, in) {
          auto inItem = new QGraphicsTextItem(pair.first+" ("+pair.second+")",this);
          inObjText.insert(pair,inItem);
          if (inItem->boundingRect().width()>inTextsize)
              inTextsize = inItem->boundingRect().width();
          auto inObj = new QGraphicsEllipseItem(0, 0, 10,10,this);
          inObj->setData(Qt::UserRole,QString("in"));
          inObjCircle.insert(pair,inObj);
          inObj->setBrush( QBrush(Qt::green));
      }
      foreach (auto pair, out) {
          auto outItem = new QGraphicsTextItem(pair.first+" ("+pair.second+")",this);
          outObjText.insert(pair,outItem);
          if (outItem->boundingRect().width()>outTextsize)
              outTextsize = outItem->boundingRect().width();
          auto outObj = new QGraphicsEllipseItem(0, 0, 10,10,this);
          outObj->setData(Qt::UserRole,QString("out"));
          outObjCircle.insert(pair,outObj);
          outObj->setBrush( QBrush(Qt::blue));
      }


      int width = titleItem->boundingRect().width() + 25;
      if (width==0)
          width = 50;
      if (width<inTextsize+outTextsize)
          width = inTextsize+outTextsize+25;
      int h_size = in.size()>out.size() ? in.size() : out.size();
      int height = ( h_size + 1 ) * 24;

      path.addRoundedRect(0-width/2.0*koef_x,0-height/2.0*koef_y,width*koef_x,height*koef_y,10,10);
      myPolygon = path.toFillPolygon();

//      setPolygon(myPolygon);
      polygonItem = new QGraphicsPolygonItem(myPolygon,this);

        polygonItem->setZValue(-10);
      titleItem->setPos(0-width/2.0*koef_x+5,0-height/2.0*koef_y);
      int i = 1;
      for (auto var : inObjCircle) {
          var->setPos(0-width/2.0*koef_x+5, 0-height/2.0*koef_y+5 + i * 24);
          i++;
      }
      i = 1;
      for (auto var : inObjText) {
          var->setPos(0-width/2.0*koef_x+15, 0-height/2.0*koef_y-5 + i * 24);
          i++;
      }
      i = 1;
      for (auto var : outObjCircle) {
          var->setPos(width*koef_x/2.0-15, 0-height/2.0*koef_y+5 + i * 24);
          i++;
      }
      i = 1;
      for (auto var : outObjText) {
          var->setPos(width*koef_x/2.0-15-var->boundingRect().width(), 0-height/2.0*koef_y-5 + i * 24);
          i++;
      }

//      switch (myDiagramType) {
//          case ALGORITM:
//                setBrush();
//              break;
//          case CONDITION:
//                setBrush();
//              break;
//          case EVENT:
//                setBrush();
//              break;
//          default:

//              break;
//      }


      setFlag(QGraphicsItem::ItemIsMovable, true);
      setFlag(QGraphicsItem::ItemIsSelectable, true);
      setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
  }
  //! [0]

  //! [1]
  void AlgoritmItem::removeArrow(Arrow *arrow)
  {
      arrows.removeAll(arrow);
  }
  //! [1]

  //! [2]
  void AlgoritmItem::removeArrows()
  {
      // need a copy here since removeArrow() will
      // modify the arrows container
      const auto arrowsCopy = arrows;
      for (Arrow *arrow : arrowsCopy) {
          static_cast<AlgoritmItem*>(arrow->_startItem()->parentItem())->removeArrow(arrow);
          static_cast<AlgoritmItem*>(arrow->_endItem()->parentItem())->removeArrow(arrow);
          scene()->removeItem(arrow);
          delete arrow;
      }
  }
  //! [2]

  //! [3]
  void AlgoritmItem::addArrow(Arrow *arrow)
  {
      arrows.append(arrow);
  }
  //! [3]

  //! [4]
  QPixmap AlgoritmItem::image(AlgoritmType type)
  {
      QPixmap pixmap(250, 250);
      switch (type) {
      case ALGORITM:
//          pixmap.fill(Qt::cyan);
          setBrush(QColor("#E3E3FD"));
          break;
      case CONDITION:
//          pixmap.fill(Qt::green);
          setBrush(QColor("#FFFFE3"));
          break;
      case EVENT:
//          pixmap.fill(Qt::magenta);
          setBrush(QColor("#FFF9A3"));
          break;
      case PARAM:
//          pixmap.fill(Qt::magenta);
          setBrush( QColor("#CFFFE5"));
          break;
      default:
          pixmap.fill(Qt::transparent);
          break;
      }

      QPainter painter(&pixmap);
      painter.setPen(QPen(Qt::black, 8));
      painter.translate(125, 125);
      painter.drawPolyline(myPolygon);

      return pixmap;
  }

  void AlgoritmItem::setBrush(QColor color)
  {
      polygonItem->setBrush(color);
  }

  QList<QGraphicsEllipseItem *> AlgoritmItem::getOutItems()
  {
      return outObjCircle.values();
  }

  QList<QGraphicsEllipseItem *> AlgoritmItem::getInItems()
  {
    return inObjCircle.values();
  }
  //! [4]

  //! [5]
  void AlgoritmItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
  {
      scene()->clearSelection();
      setSelected(true);
      myContextMenu->exec(event->screenPos());
  }
  //! [5]

  //! [6]
  QVariant AlgoritmItem::itemChange(GraphicsItemChange change, const QVariant &value)
  {
      if (change == QGraphicsItem::ItemPositionChange) {
          for (Arrow *arrow : qAsConst(arrows))
              arrow->updatePosition();
      }

      return value;
  }

  QRectF AlgoritmItem::boundingRect() const
  {
      return polygonItem->boundingRect();
  }

  void AlgoritmItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
  {

  }
  //! [6]
