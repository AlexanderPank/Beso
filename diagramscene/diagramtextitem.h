#ifndef DIAGRAMTEXTITEM_H
#define DIAGRAMTEXTITEM_H

#include <QGraphicsTextItem>

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

//! [0]
class DiagramTextItem : public QGraphicsTextItem
{
    Q_OBJECT

public:
    enum { Type = UserType + 3 };

    // Конструктор текстового элемента
    DiagramTextItem(QGraphicsItem *parent = nullptr);

    // Тип элемента
    int type() const override { return Type; }

signals:
    void lostFocus(DiagramTextItem *item);
    void selectedChange(QGraphicsItem *item);

protected:
    // Отслеживает изменения выделения
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    // Обрабатывает потерю фокуса
    void focusOutEvent(QFocusEvent *event) override;
    // Включает редактирование по двойному клику
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
};
//! [0]

#endif // DIAGRAMTEXTITEM_H
