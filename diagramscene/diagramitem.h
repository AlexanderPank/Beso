#ifndef DIAGRAMITEM_H
#define DIAGRAMITEM_H

#include <QGraphicsPixmapItem>
#include <QList>

QT_BEGIN_NAMESPACE
class QPixmap;
class QGraphicsSceneContextMenuEvent;
class QMenu;
class QPolygonF;
QT_END_NAMESPACE

class Arrow;

//! [0]
class DiagramItem : public QGraphicsPolygonItem
{
public:
    enum { Type = UserType + 15 };
    enum DiagramType { Start, End, Conditional, Event };

    // Конструктор элемента диаграммы
    DiagramItem(DiagramType diagramType, QMenu *contextMenu, QGraphicsItem *parent = nullptr);

    // Удаляет конкретную стрелку
    void removeArrow(Arrow *arrow);
    // Удаляет все стрелки
    void removeArrows();
    // Возвращает тип диаграммы
    DiagramType diagramType() const { return myDiagramType; }
    // Возвращает текущий полигон
    QPolygonF polygon() const { return myPolygon; }
    // Добавляет стрелку
    void addArrow(Arrow *arrow);
    // Возвращает изображение элемента
    QPixmap image() const;
    // Тип элемента
    int type() const override { return Type; }

protected:
    // Отображает контекстное меню
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    // Обрабатывает изменение положения
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    DiagramType myDiagramType;
    QPolygonF myPolygon;
    QMenu *myContextMenu;
    QList<Arrow *> arrows;
};
//! [0]

#endif // DIAGRAMITEM_H
