#include "diagramscene.h"
#include "arrow.h"

#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>
#include <QPixmap>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

// Конструктор сцены диаграммы
DiagramScene::DiagramScene(QMenu *itemMenu, QObject *parent)
    : QGraphicsScene(parent)
{
    myItemMenu = itemMenu;
    myMode = MoveItem;
    myItemType = AlgoritmItem::ALGORITM;
    line = nullptr;
    textItem = nullptr;
    myItemColor = Qt::white;
    myTextColor = Qt::black;
    myLineColor = QColor(100,149,237);
    center = sceneRect().center();
    // Устанавливаем фон "Сетка" по умолчанию
    setBackgroundBrush(QPixmap(":/images_diag/background2.png"));
}

// Устанавливает цвет стрелок
void DiagramScene::setLineColor(const QColor &color)
{
    myLineColor = color;
    if (isItemChange(Arrow::Type)) {
        Arrow *item = qgraphicsitem_cast<Arrow *>(selectedItems().first());
        item->setColor(myLineColor);
        update();
    }
}

// Устанавливает цвет текста
void DiagramScene::setTextColor(const QColor &color)
{
    myTextColor = color;
    if (isItemChange(DiagramTextItem::Type)) {
        DiagramTextItem *item = qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());
        item->setDefaultTextColor(myTextColor);
    }
}

// Устанавливает цвет элементов алгоритма
void DiagramScene::setItemColor(const QColor &color)
{
    myItemColor = color;
    if (isItemChange(DiagramItem::Type)) {
        AlgoritmItem *item = qgraphicsitem_cast<AlgoritmItem *>(selectedItems().first());
        item->setBrush(myItemColor);
    }
}

// Устанавливает шрифт текстовых элементов
void DiagramScene::setFont(const QFont &font)
{
    myFont = font;

    if (isItemChange(DiagramTextItem::Type)) {
        QGraphicsTextItem *item = qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());
        //At this point the selection can change so the first selected item might not be a DiagramTextItem
        if (item)
            item->setFont(myFont);
    }
}

// Запоминает указатель на отображающий сцену виджет
void DiagramScene::setView(QGraphicsView *view)
{
    parentView = view;
}

// Меняет режим работы сцены
void DiagramScene::setMode(Mode mode)
{
    myMode = mode;
}

// Устанавливает тип добавляемого алгоритмического элемента
void DiagramScene::setItemType(AlgoritmItem::AlgoritmType type)
{
    myItemType = type;
}

// Завершает редактирование текста
void DiagramScene::editorLostFocus(DiagramTextItem *item)
{
    QTextCursor cursor = item->textCursor();
    cursor.clearSelection();
    item->setTextCursor(cursor);

    if (item->toPlainText().isEmpty()) {
        removeItem(item);
        item->deleteLater();
    }
}

// Обрабатывает нажатие кнопок мыши на сцене
void DiagramScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (center.x()==0 && center.y()==0)
        center = sceneRect().center();
    if (mouseEvent->button() == Qt::RightButton && mouseEvent->button() != Qt::LeftButton){
        prevMode = myMode;
        myMode = MoveFullScene;
        beginMousePos = QCursor::pos();

        return;
    }
    if (mouseEvent->button() != Qt::LeftButton)
        return;

    AlgoritmItem *item;
    switch (myMode) {
        case InsertItem:{
            QString title = "";
            QList<QPair<QString,QString>> in;
            QList<QPair<QString,QString>> out;
            switch (myItemType) {
            case AlgoritmItem::ALGORITM:
                title = "Алгоритм";
                in = {QPair<QString,QString> ("Lat","double"),QPair<QString,QString> ("Lon","double")};
                out = {QPair<QString,QString> ("Lat","double"),QPair<QString,QString> ("Lon","double")};
                myItemColor = QColor("#E3E3FD");
                break;
            case AlgoritmItem::CONDITION:{
                    title = "Условие";
                    QPair<QString,QString> f("SeeTarget","bool");
                    in.append(f);
                    out = {QPair<QString,QString> ("Lat","double"),QPair<QString,QString> ("Lon","double")};
                    myItemColor = QColor("#FFFFE3");
                }break;
            case AlgoritmItem::EVENT:
                title = "Событие";
                out = {QPair<QString,QString> ("SeeTarget","bool")};
                myItemColor = QColor("#FFF9A3");
                break;
            case AlgoritmItem::PARAM:
                title = "Параметры";
                in = {QPair<QString,QString> ("Lat","double"),QPair<QString,QString> ("Lon","double")};
                myItemColor = QColor("#CFFFE5");
                break;
            default:
                break;
            }
            item = new AlgoritmItem(myItemType, myItemMenu,title,in,out);
            item->setBrush(myItemColor);
            addItem(item);
            item->setPos(mouseEvent->scenePos());
            emit itemInserted(item);}
            break;
        case InsertLine:
            line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(),
                                        mouseEvent->scenePos()));
            line->setPen(QPen(myLineColor, 2));
            addItem(line);
            break;
        case InsertText:
            textItem = new DiagramTextItem();
            textItem->setFont(myFont);
            textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
            textItem->setZValue(1000.0);
            connect(textItem, &DiagramTextItem::lostFocus,
                    this, &DiagramScene::editorLostFocus);
            connect(textItem, &DiagramTextItem::selectedChange,
                    this, &DiagramScene::itemSelected);
            addItem(textItem);
            textItem->setDefaultTextColor(myTextColor);
            textItem->setPos(mouseEvent->scenePos());
            emit textInserted(textItem);
    default:
        ;
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}

// Обрабатывает перемещение мыши
void DiagramScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (myMode == MoveFullScene){

        newCenter = QPointF(center.x() + beginMousePos.x() - QCursor::pos().x(),
                             center.y() + beginMousePos.y() - QCursor::pos().y());
        parentView->centerOn(newCenter);

    }
    if (myMode == InsertLine && line != nullptr) {
        QLineF newLine(line->line().p1(), mouseEvent->scenePos());
        line->setLine(newLine);
    } else if (myMode == MoveItem) {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}

// Обрабатывает отпускание кнопок мыши
void DiagramScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (myMode == MoveFullScene){
        myMode = prevMode;
        center = newCenter;
    }
    if (line != nullptr && myMode == InsertLine) {
        QList<QGraphicsItem *> startItems = items(line->line().p1());
        if (startItems.count() && startItems.first() == line)
            startItems.removeFirst();
        QList<QGraphicsItem *> endItems = items(line->line().p2());
        if (endItems.count() && endItems.first() == line)
            endItems.removeFirst();

        removeItem(line);
        delete line;
        QGraphicsEllipseItem * startItem = nullptr;
        QGraphicsEllipseItem * endItem = nullptr;
        for (QGraphicsItem * var : startItems) {
            if (static_cast<QGraphicsEllipseItem *>(var)->data(Qt::UserRole).toString().contains("out"))
                startItem = static_cast<QGraphicsEllipseItem *>(var);
        }
        for (QGraphicsItem * var : endItems) {
            if (static_cast<QGraphicsEllipseItem *>(var)->data(Qt::UserRole).toString().contains("in"))
                endItem = static_cast<QGraphicsEllipseItem *>(var);
        }

        if (startItem && startItem->parentItem()
                && endItem && endItem->parentItem()) {

            Arrow *arrow = new Arrow(startItem, endItem);
            arrow->setColor(myLineColor);
            static_cast<AlgoritmItem*>(startItem->parentItem())->addArrow(arrow);
            static_cast<AlgoritmItem*>(endItem->parentItem())->addArrow(arrow);
            addItem(arrow);
            arrow->updatePosition();
            emit lineInserted();
        }
    }
    line = nullptr;
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

// Обрабатывает прокрутку колесом мыши
void DiagramScene::wheelEvent(QGraphicsSceneWheelEvent *mouseEvent)
{
    if (mouseEvent->delta()>0)
        emit zoom(1);
    else
        emit zoom(-1);
}

void DiagramScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-algorithm"))
        event->acceptProposedAction();
    else
        QGraphicsScene::dragEnterEvent(event);
}

void DiagramScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-algorithm"))
        event->acceptProposedAction();
    else
        QGraphicsScene::dragMoveEvent(event);
}

void DiagramScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-algorithm")) {
        QString filePath = QString::fromUtf8(event->mimeData()->data("application/x-algorithm"));
        QFile f(filePath);
        if (f.open(QIODevice::ReadOnly)) {
            QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
            f.close();
            QJsonObject obj = doc.object();
            QString title = obj["title"].toString();
            QList<QPair<QString,QString>> inParams;
            QList<QPair<QString,QString>> outParams;
            QJsonArray inArr = obj["input_parameters"].toArray();
            for (const QJsonValue &val : inArr) {
                QJsonObject o = val.toObject();
                if (!o.isEmpty()) {
                    QString key = o.keys().first();
                    inParams.append({key, o.value(key).toString()});
                }
            }
            QJsonArray outArr = obj["output_parameters"].toArray();
            for (const QJsonValue &val : outArr) {
                QJsonObject o = val.toObject();
                if (!o.isEmpty()) {
                    QString key = o.keys().first();
                    outParams.append({key, o.value(key).toString()});
                }
            }
            AlgoritmItem *item = new AlgoritmItem(AlgoritmItem::ALGORITM, myItemMenu, title, inParams, outParams);
            item->setBrush(QColor("#E3E3FD"));
            addItem(item);
            item->setPos(event->scenePos());
            emit itemInserted(item);
        }
        event->acceptProposedAction();
    } else {
        QGraphicsScene::dropEvent(event);
    }
}

// Проверяет, изменился ли элемент указанного типа
bool DiagramScene::isItemChange(int type) const
{
    const QList<QGraphicsItem *> items = selectedItems();
    const auto cb = [type](const QGraphicsItem *item) { return item->type() == type; };
    return std::find_if(items.begin(), items.end(), cb) != items.end();
}
