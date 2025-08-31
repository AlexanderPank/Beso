#ifndef DIAGRAMSCENE_H
#define DIAGRAMSCENE_H

#include "diagramitem.h"
#include "diagramtextitem.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include "algorithmitem.h"
#include <QJsonObject>

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QMenu;
class QPointF;
class QGraphicsLineItem;
class QFont;
class QGraphicsTextItem;
class QColor;
class QGraphicsSceneDragDropEvent;
class QString;
QT_END_NAMESPACE

class DiagramScene : public QGraphicsScene
{
    Q_OBJECT

public:
    enum Mode { InsertItem, InsertLine, InsertText, MoveItem, MoveFullScene };

    // Конструктор сцены диаграммы
    explicit DiagramScene(QMenu *itemMenu, QObject *parent = nullptr);

    // Возвращает текущий шрифт текста
    QFont font() const { return myFont; }

    // Возвращает цвет текста
    QColor textColor() const { return myTextColor; }

    // Возвращает цвет элементов
    QColor itemColor() const { return myItemColor; }

    // Возвращает цвет линий
    QColor lineColor() const { return myLineColor; }

    // Устанавливает цвет линий
    void setLineColor(const QColor &color);

    // Устанавливает цвет текста
    void setTextColor(const QColor &color);

    // Устанавливает цвет элементов
    void setItemColor(const QColor &color);

    // Устанавливает шрифт текста
    void setFont(const QFont &font);

    // Запоминает указатель на отображающий сцену виджет
    void setView(QGraphicsView *view);

    // Возвращает JSON-объект с описанием элементов сцены
    QJsonObject toJson() const;

    // Сохраняет элементы сцены в указанный файл
    bool saveToFile(const QString &fileName) const;

public slots:
    // Меняет текущий режим работы сцены
    void setMode(Mode mode);

    // Устанавливает тип добавляемого алгоритмического элемента
    void setItemType(AlgorithmItem::AlgorithmType type);

    // Обрабатывает потерю фокуса текстовым редактором
    void editorLostFocus(DiagramTextItem *item);

signals:
    // Сигнал о вставке нового элемента
    void itemInserted(AlgorithmItem *item);
    void textInserted(QGraphicsTextItem *item);
    void itemSelected(QGraphicsItem *item);
    void zoom(int i);
    void lineInserted();

protected:
    // Обработка нажатия кнопок мыши на сцене
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

    // Обработка перемещения мыши
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

    // Обработка отпускания кнопок мыши
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

    // Обработка прокрутки колесом мыши
    void wheelEvent(QGraphicsSceneWheelEvent *mouseEvent) override;

    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;

private:
    // Проверяет, изменился ли элемент указанного типа
    bool isItemChange(int type) const;

    void addArrowFromLine(const QPointF &endPoint);

    QGraphicsView *parentView = new QGraphicsView();
    QPointF beginMousePos;
    QPointF center;
    QPointF newCenter;

    AlgorithmItem::AlgorithmType myItemType;
    QMenu *myItemMenu;
    Mode prevMode;
    Mode myMode;
    bool leftButtonDown;
    QPointF startPoint;
    QGraphicsLineItem *line;
    QFont myFont;
    DiagramTextItem *textItem;
    QColor myTextColor;
    QColor myItemColor;
    QColor myLineColor;
};

#endif // DIAGRAMSCENE_H
