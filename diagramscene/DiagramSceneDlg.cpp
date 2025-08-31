
#include "arrow.h"
#include "diagramitem.h"
#include "diagramscene.h"
#include "diagramtextitem.h"
#include "DiagramSceneDlg.h"
#include "ObjectSelectDialog.h"
#include "PropertiesDialog.h"
#include "AlgorithmPropertiesDialog.h"

#include <QtWidgets>
#include <algorithm>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDrag>
#include <QMimeData>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QPainter>
#include <QMap>
#include <QTreeWidget>
#include <QStyle>
#include <QSplitter>
#include "../db_service/services/FileDataStorageService.h"

const int InsertTextButton = 10;

class AlgorithmTreeWidget : public QTreeWidget
{
public:
    using QTreeWidget::QTreeWidget;

protected:
    void startDrag(Qt::DropActions supportedActions) override
    {
        QTreeWidgetItem *item = currentItem();
        if (!item || item->childCount() != 0) {
            QTreeWidget::startDrag(supportedActions);
            return;
        }

        QString filePath = item->data(0, Qt::UserRole).toString();
        QFile f(filePath);
        if (!f.open(QIODevice::ReadOnly)) {
            QTreeWidget::startDrag(supportedActions);
            return;
        }
        QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
        f.close();
        QJsonObject obj = doc.object();

        QString title = obj["title"].toString(item->text(0));
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

        auto *temp = new AlgorithmItem(AlgorithmItem::ALGORITM, nullptr, title, inParams, outParams);
        temp->setBrush(QColor("#E3E3FD"));
        QGraphicsScene tmpScene;
        tmpScene.addItem(temp);
        QRectF br = temp->boundingRect();
        QPixmap pix(br.size().toSize());
        pix.fill(Qt::transparent);
        QPainter painter(&pix);
        tmpScene.render(&painter, QRectF(), br);

        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;
        mimeData->setData("application/x-algorithm", filePath.toUtf8());
        drag->setMimeData(mimeData);
        drag->setPixmap(pix);
        drag->exec(Qt::CopyAction);
    }
};
// Конструктор диалогового окна сцены
DiagramSceneDlg::DiagramSceneDlg()
{
    createActions();
    createToolBox();
    createMenus();

    scene = new DiagramScene(itemMenu, this);
    scene->setSceneRect(QRectF(0, 0, 5000, 5000));
    scene->setBackgroundBrush(QPixmap(":/images_diag/background2.png"));

    connect(scene, &DiagramScene::itemInserted,
            this, &DiagramSceneDlg::itemInserted);
    connect(scene, &DiagramScene::textInserted,
            this, &DiagramSceneDlg::textInserted);
    connect(scene, &DiagramScene::itemSelected,
            this, &DiagramSceneDlg::itemSelected);
    connect(scene, &DiagramScene::lineInserted,
            this, &DiagramSceneDlg::handleLineInserted);
    createToolbars();

    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    toolBox->setMinimumWidth(200);
    splitter->addWidget(toolBox);
    view = new QGraphicsView(scene);
    view->setAcceptDrops(true);
    splitter->addWidget(view);

    setCentralWidget(splitter);
    setWindowTitle(tr("Редактор поведенческого ядра объекта"));
    setUnifiedTitleAndToolBarOnMac(true);
    setWindowState(Qt::WindowMaximized);
    scene->setView(view);
    connect(scene,SIGNAL(zoom(int)),this,SLOT(changeZoom(int)));
}

// Обработчик выбора фона сцены
void DiagramSceneDlg::backgroundButtonGroupClicked(QAbstractButton *button)
{
    const QList<QAbstractButton *> buttons = backgroundButtonGroup->buttons();
    for (QAbstractButton *myButton : buttons) {
        if (myButton != button)
            button->setChecked(false);
    }
    QString text = button->text();
    if (text == tr("Серая сетка"))
        scene->setBackgroundBrush(QPixmap(":/images_diag/background1.png"));
    else if (text == tr("Сетка"))
        scene->setBackgroundBrush(QPixmap(":/images_diag/background2.png"));
    else if (text == tr("Черная сетка"))
        scene->setBackgroundBrush(QPixmap(":/images_diag/background3.png"));
    else
        scene->setBackgroundBrush(QPixmap(":/images_diag/background4.png"));

    scene->update();
    view->update();
}

// Обработчик выбора элемента диаграммы
void DiagramSceneDlg::buttonGroupClicked(QAbstractButton *button)
{
    const QList<QAbstractButton *> buttons = buttonGroup->buttons();
    for (QAbstractButton *myButton : buttons) {
        if (myButton != button)
            button->setChecked(false);
    }
    const int id = buttonGroup->id(button);
    if (id == InsertTextButton) {
        scene->setMode(DiagramScene::InsertText);
    } else {
        scene->setItemType(AlgorithmItem::AlgorithmType(id));
        scene->setMode(DiagramScene::InsertItem);
    }
}

// Удаляет выбранные элементы
void DiagramSceneDlg::deleteItem()
{
    QList<QGraphicsItem *> selectedItems = scene->selectedItems();
    for (QGraphicsItem *item : qAsConst(selectedItems)) {
        if (item->type() == Arrow::Type) {
            scene->removeItem(item);
            Arrow *arrow = qgraphicsitem_cast<Arrow *>(item);
            qgraphicsitem_cast<AlgorithmItem *>(arrow->startItem()->parentItem())->removeArrow(arrow);
            qgraphicsitem_cast<AlgorithmItem *>(arrow->endItem()->parentItem())->removeArrow(arrow);
            delete item;
        }
    }

    selectedItems = scene->selectedItems();
    for (QGraphicsItem *item : qAsConst(selectedItems)) {
         if (item->type() == AlgorithmItem::Type)
             qgraphicsitem_cast<AlgorithmItem *>(item)->removeArrows();
         scene->removeItem(item);
         delete item;
     }
}

// Изменяет режим указателя на сцене
void DiagramSceneDlg::pointerGroupClicked()
{
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
}

void DiagramSceneDlg::singleLineButtonClicked()
{
    m_singleLineMode = true;
    if (pointerTypeGroup->button(int(DiagramScene::InsertLine)))
        pointerTypeGroup->button(int(DiagramScene::InsertLine))->setChecked(true);
    scene->setMode(DiagramScene::InsertLine);
}

void DiagramSceneDlg::handleLineInserted()
{
    if (m_singleLineMode) {
        m_singleLineMode = false;
        if (pointerTypeGroup->button(int(DiagramScene::MoveItem)))
            pointerTypeGroup->button(int(DiagramScene::MoveItem))->setChecked(true);
        scene->setMode(DiagramScene::MoveItem);
    }
}

// Перемещает выделенный элемент на передний план
void DiagramSceneDlg::bringToFront()
{
    if (scene->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = scene->selectedItems().first();
    const QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

    qreal zValue = 0;
    for (const QGraphicsItem *item : overlapItems) {
        if (item->zValue() >= zValue && item->type() == DiagramItem::Type)
            zValue = item->zValue() + 0.1;
    }
    selectedItem->setZValue(zValue);
}

// Перемещает выделенный элемент на задний план
void DiagramSceneDlg::sendToBack()
{
    if (scene->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = scene->selectedItems().first();
    const QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

    qreal zValue = 0;
    for (const QGraphicsItem *item : overlapItems) {
        if (item->zValue() <= zValue && item->type() == DiagramItem::Type)
            zValue = item->zValue() - 0.1;
    }
    selectedItem->setZValue(zValue);
}

// Обработчик добавления нового элемента алгоритма
void DiagramSceneDlg::itemInserted(AlgorithmItem *item)
{
    pointerTypeGroup->button(int(DiagramScene::MoveItem))->setChecked(true);
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
}

// Открывает диалог выбора объекта и добавляет его на сцену
void DiagramSceneDlg::openObjectSelectDialog()
{
    ObjectSelectDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        QString id = dlg.selectedId();
        QString title = dlg.selectedTitle();
        QList<AlgorithmItem::PropertyInfo> props;
        QDir dir(QString(MAIN_DIR_DEFAULT) + SUB_DIR_OBJECTS);
        dir.setNameFilters({"*.json"});
        QFileInfoList files = dir.entryInfoList();
        for (const QFileInfo &info : files) {
            QFile f(info.absoluteFilePath());
            if (!f.open(QIODevice::ReadOnly))
                continue;
            QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
            f.close();
            QJsonObject obj = doc.object();
            if (obj["id"].toString() == id) {
                QJsonArray arr = obj["properties"].toArray();
                for (const QJsonValue &val : arr) {
                    QJsonObject o = val.toObject();
                    QString name = o["name"].toString();
                    QString ptitle = o["title"].toString(name);
                    QString type = o["type"].toString();
                    props.append(AlgorithmItem::PropertyInfo{ptitle, name, type, 0});
                }
                break;
            }
        }

        auto *item = new AlgorithmItem(AlgorithmItem::ALGORITM, itemMenu, title, {}, {});
        item->setBrush(QColor("#D3D3D3"));
        item->setProperties(props);
        item->setObjectOutput(true);
        item->setIsObject(true);
        QPointF centerPoint = view->mapToScene(view->viewport()->rect().center());
        item->setPos(centerPoint - QPointF(item->boundingRect().width()/2, item->boundingRect().height()/2));
        scene->addItem(item);
        emit itemInserted(item);
    }
}

// Настраивает фон сцены
void DiagramSceneDlg::openBackgroundSettings()
{
    QDialog dlg(this);
    dlg.setWindowTitle(tr("Базовый фон"));

    backgroundButtonGroup = new QButtonGroup(&dlg);
    connect(backgroundButtonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
            this, &DiagramSceneDlg::backgroundButtonGroupClicked);

    QGridLayout *backgroundLayout = new QGridLayout;
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("Серая сетка"), ":/images_diag/background1.png"), 0, 0);
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("Сетка"), ":/images_diag/background2.png"), 0, 1);
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("Черная сетка"), ":/images_diag/background3.png"), 1, 0);
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("Без сетки"), ":/images_diag/background4.png"), 1, 1);
    backgroundLayout->setRowStretch(2, 10);
    backgroundLayout->setColumnStretch(2, 10);

    dlg.setLayout(backgroundLayout);
    dlg.exec();

    backgroundButtonGroup = nullptr;
}

void DiagramSceneDlg::openItemProperties()
{
    QList<QGraphicsItem*> sel = scene->selectedItems();
    if (sel.isEmpty())
        return;
    AlgorithmItem *item = qgraphicsitem_cast<AlgorithmItem*>(sel.first());
    if (!item)
        return;
    bool hasDirected = false;
    for (const auto &p : item->properties()) {
        if (p.direction != 0) {
            hasDirected = true;
            break;
        }
    }
    if (hasDirected) {
        AlgorithmPropertiesDialog dlg(item->properties(), this);
        if (dlg.exec() == QDialog::Accepted) {
            item->setProperties(dlg.properties());
        }
    } else {
        PropertiesDialog dlg(item->properties(), this);
        if (dlg.exec() == QDialog::Accepted) {
            item->setProperties(dlg.properties());
        }
    }
}

void DiagramSceneDlg::saveToJson()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить файл"), QString(), tr("JSON (*.json)"));
    if (fileName.isEmpty())
        return;

    QJsonObject root;
    QJsonArray itemsArr;
    QList<AlgorithmItem*> itemsList;
    const auto allItems = scene->items();
    for (QGraphicsItem *gi : allItems) {
        if (auto alg = qgraphicsitem_cast<AlgorithmItem*>(gi)) {
            itemsList.append(alg);
            QJsonObject obj;
            obj["title"] = alg->title();
            obj["x"] = alg->pos().x();
            obj["y"] = alg->pos().y();
            obj["self_out"] = alg->hasObjectOutput();
            obj["is_object"] = alg->isObject();
            QJsonArray props;
            for (const auto &p : alg->properties()) {
                QJsonObject po;
                po["title"] = p.title;
                po["name"] = p.name;
                po["type"] = p.type;
                po["direction"] = p.direction;
                props.append(po);
            }
            obj["properties"] = props;
            itemsArr.append(obj);
        }
    }
    root["items"] = itemsArr;

    QJsonArray arrowsArr;
    for (QGraphicsItem *gi : allItems) {
        if (gi->type() == Arrow::Type) {
            Arrow *arr = static_cast<Arrow*>(gi);
            AlgorithmItem *startAlg = qgraphicsitem_cast<AlgorithmItem*>(arr->startItem()->parentItem());
            AlgorithmItem *endAlg = qgraphicsitem_cast<AlgorithmItem*>(arr->endItem()->parentItem());
            int fromIndex = itemsList.indexOf(startAlg);
            int toIndex = itemsList.indexOf(endAlg);
            if (fromIndex >= 0 && toIndex >= 0) {
                QJsonObject ao;
                ao["from"] = fromIndex;
                ao["to"] = toIndex;
                ao["fromProp"] = startAlg->propertyNameForCircle(arr->startItem());
                ao["toProp"] = endAlg->propertyNameForCircle(arr->endItem());
                arrowsArr.append(ao);
            }
        }
    }
    root["arrows"] = arrowsArr;

    QFile f(fileName);
    if (f.open(QIODevice::WriteOnly)) {
        f.write(QJsonDocument(root).toJson());
        f.close();
    }
}

void DiagramSceneDlg::loadFromJson()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Загрузить файл"), QString(), tr("JSON (*.json)"));
    if (fileName.isEmpty())
        return;

    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly))
        return;
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();

    scene->clear();

    QJsonObject root = doc.object();
    QJsonArray itemsArr = root["items"].toArray();
    QList<AlgorithmItem*> itemsList;
    for (const QJsonValue &val : itemsArr) {
        QJsonObject obj = val.toObject();
        QString title = obj["title"].toString();
        QList<AlgorithmItem::PropertyInfo> props;
        QJsonArray propsArr = obj["properties"].toArray();
        for (const QJsonValue &pv : propsArr) {
            QJsonObject po = pv.toObject();
            AlgorithmItem::PropertyInfo p;
            p.title = po["title"].toString();
            p.name = po["name"].toString();
            p.type = po["type"].toString();
            p.direction = po["direction"].toInt();
            props.append(p);
        }
        auto *item = new AlgorithmItem(AlgorithmItem::ALGORITM, itemMenu, title, {}, {});
        item->setBrush(QColor("#D3D3D3"));
        item->setProperties(props);
        item->setIsObject(obj["is_object"].toBool());
        if (obj["self_out"].toBool())
            item->setObjectOutput(true);
        item->setPos(obj["x"].toDouble(), obj["y"].toDouble());
        scene->addItem(item);
        itemsList.append(item);
    }

    QJsonArray arrowsArr = root["arrows"].toArray();
    for (const QJsonValue &val : arrowsArr) {
        QJsonObject ao = val.toObject();
        int from = ao["from"].toInt();
        int to = ao["to"].toInt();
        QString fromProp = ao["fromProp"].toString();
        QString toProp = ao["toProp"].toString();
        if (from >= 0 && from < itemsList.size() && to >= 0 && to < itemsList.size()) {
            AlgorithmItem *startItem = itemsList[from];
            AlgorithmItem *endItem = itemsList[to];
            QGraphicsEllipseItem *startCircle = startItem->circleForProperty(fromProp, 2);
            QGraphicsEllipseItem *endCircle = endItem->circleForProperty(toProp, 1);
            if (startCircle && endCircle) {
                Arrow *arrow = new Arrow(startCircle, endCircle);
                arrow->setColor(scene->lineColor());
                startItem->addArrow(arrow);
                endItem->addArrow(arrow);
                scene->addItem(arrow);
                arrow->updatePosition();
            }
        }
    }
}

// Обработчик вставки текстового элемента
void DiagramSceneDlg::textInserted(QGraphicsTextItem *)
{
    buttonGroup->button(InsertTextButton)->setChecked(false);
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
}

// Обновляет текущий шрифт текста
void DiagramSceneDlg::currentFontChanged(const QFont &)
{
    handleFontChange();
}

// Изменяет размер выбранного шрифта
void DiagramSceneDlg::fontSizeChanged(const QString &)
{
    handleFontChange();
}

// Изменяет масштаб отображения сцены
void DiagramSceneDlg::sceneScaleChanged(const QString &scale)
{
    double newScale = scale.left(scale.indexOf(tr("%"))).toDouble() / 100.0;
    QTransform oldMatrix = view->transform();
    view->resetTransform();
    view->translate(oldMatrix.dx(), oldMatrix.dy());
    view->scale(newScale, newScale);
}

// Изменяет цвет текста
void DiagramSceneDlg::textColorChanged()
{
    textAction = qobject_cast<QAction *>(sender());
    fontColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images_diag/textpointer.png",
                                     qvariant_cast<QColor>(textAction->data())));
    textButtonTriggered();
}

// Изменяет цвет выбранного элемента
void DiagramSceneDlg::itemColorChanged()
{
    fillAction = qobject_cast<QAction *>(sender());
    fillColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images_diag/floodfill.png",
                                     qvariant_cast<QColor>(fillAction->data())));
    fillButtonTriggered();
}

// Изменяет цвет линий соединений
void DiagramSceneDlg::lineColorChanged()
{
    lineAction = qobject_cast<QAction *>(sender());
    lineColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images_diag/linecolor.png",
                                     qvariant_cast<QColor>(lineAction->data())));
    lineButtonTriggered();
}

// Активирует режим ввода текста
void DiagramSceneDlg::textButtonTriggered()
{
    scene->setTextColor(qvariant_cast<QColor>(textAction->data()));
}

// Активирует выбор цвета заполнения
void DiagramSceneDlg::fillButtonTriggered()
{
    scene->setItemColor(qvariant_cast<QColor>(fillAction->data()));
}

// Активирует выбор цвета линий
void DiagramSceneDlg::lineButtonTriggered()
{
    if (lineAction)
        scene->setLineColor(qvariant_cast<QColor>(lineAction->data()));
}

// Обрабатывает изменение параметров шрифта
void DiagramSceneDlg::handleFontChange()
{
    QFont font = fontCombo->currentFont();
    font.setPointSize(fontSizeCombo->currentText().toInt());
    font.setWeight(boldAction->isChecked() ? QFont::Bold : QFont::Normal);
    font.setItalic(italicAction->isChecked());
    font.setUnderline(underlineAction->isChecked());

    scene->setFont(font);
}

// Реагирует на выбор элемента сцены
void DiagramSceneDlg::itemSelected(QGraphicsItem *item)
{
    DiagramTextItem *textItem =
    qgraphicsitem_cast<DiagramTextItem *>(item);

    QFont font = textItem->font();
    fontCombo->setCurrentFont(font);
    fontSizeCombo->setEditText(QString().setNum(font.pointSize()));
    boldAction->setChecked(font.weight() == QFont::Bold);
    italicAction->setChecked(font.italic());
    underlineAction->setChecked(font.underline());
}

// Меняет масштаб сцены через колесо мыши
void DiagramSceneDlg::changeZoom(int i)
{
    int curIndex = sceneScaleCombo->currentIndex()+i;
    if (curIndex<0)
        curIndex =0;
    if (curIndex>4)
        curIndex = 4;
    sceneScaleCombo->setCurrentIndex(curIndex);
}

// Показывает информацию о приложении
void DiagramSceneDlg::about()
{
    QMessageBox::about(this, tr("О программе"),
                       tr("<b>Редактор поведенческого ядра объекта</b> версии <b>0.1</b> "
                          "Позволяет создавать поведенческое ядро объекта посредством "
                          "описания графа состояний и реакции на внешние события."
                          " Предоставляет графический интерфейс для создания алгоритмов, "
                          "и показывающий поток данных между ними."));
}

// Создаёт набор инструментов
void DiagramSceneDlg::createToolBox()
{
    buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(false);
    connect(buttonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
            this, &DiagramSceneDlg::buttonGroupClicked);
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(createCellWidget(tr("Условие"), DiagramItem::Conditional), 0, 0);
    layout->addWidget(createCellWidget(tr("Событие"), DiagramItem::StartEnd), 0, 1);

    QToolButton *textButton = new QToolButton;
    textButton->setCheckable(true);
    buttonGroup->addButton(textButton, InsertTextButton);
    textButton->setIcon(QIcon(QPixmap(":/images_diag/textpointer.png")));
    textButton->setIconSize(QSize(50, 50));
    QGridLayout *textLayout = new QGridLayout;
    textLayout->addWidget(textButton, 0, 0, Qt::AlignHCenter);
    textLayout->addWidget(new QLabel(tr("Текст")), 1, 0, Qt::AlignCenter);
    QWidget *textWidget = new QWidget;
    textWidget->setLayout(textLayout);
    layout->addWidget(textWidget, 1, 0);

    QToolButton *singleLineButton = new QToolButton;
    singleLineButton->setIcon(QIcon(":/images_diag/linepointer.png"));
    singleLineButton->setIconSize(QSize(50, 50));
    connect(singleLineButton, &QToolButton::clicked,
            this, &DiagramSceneDlg::singleLineButtonClicked);
    QGridLayout *lineLayout = new QGridLayout;
    lineLayout->addWidget(singleLineButton, 0, 0, Qt::AlignHCenter);
    lineLayout->addWidget(new QLabel(tr("Связь")), 1, 0, Qt::AlignCenter);
    QWidget *lineWidget = new QWidget;
    lineWidget->setLayout(lineLayout);
    layout->addWidget(lineWidget, 1, 1);

    layout->setRowStretch(2, 10);
    layout->setColumnStretch(2, 10);

    QWidget *itemWidget = new QWidget;
    itemWidget->setLayout(layout);

    // Load algorithms from storage
    AlgorithmTreeWidget *algTree = new AlgorithmTreeWidget;
    algTree->setHeaderHidden(true);
    algTree->setDragEnabled(true);

    QDir dir(QString(MAIN_DIR_DEFAULT) + SUB_DIR_ALGORITHMS);
    dir.setNameFilters({"*.json"});
    QFileInfoList files = dir.entryInfoList();

    QMap<QString, QTreeWidgetItem*> typeItems;
    QMap<QString, QMap<QString, QTreeWidgetItem*>> subTypeItems;

    QIcon folderIcon = QApplication::style()->standardIcon(QStyle::SP_DirIcon);
    QIcon fileIcon = QApplication::style()->standardIcon(QStyle::SP_FileIcon);

    for (const QFileInfo &info : files) {
        QFile f(info.absoluteFilePath());
        if (!f.open(QIODevice::ReadOnly))
            continue;
        QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
        f.close();
        QJsonObject obj = doc.object();

        QString type = obj["type"].toString();
        QString subType = obj["subType"].toString();
        QString title = obj["title"].toString(info.completeBaseName());

        QTreeWidgetItem *typeItem = nullptr;
        if (!typeItems.contains(type)) {
            typeItem = new QTreeWidgetItem(algTree);
            typeItem->setText(0, type);
            typeItem->setIcon(0, folderIcon);
            typeItems.insert(type, typeItem);
        } else {
            typeItem = typeItems.value(type);
        }

        QTreeWidgetItem *subTypeItem = nullptr;
        QMap<QString, QTreeWidgetItem*> &subMap = subTypeItems[type];
        if (!subMap.contains(subType)) {
            subTypeItem = new QTreeWidgetItem(typeItem);
            subTypeItem->setText(0, subType);
            subTypeItem->setIcon(0, folderIcon);
            subMap.insert(subType, subTypeItem);
        } else {
            subTypeItem = subMap.value(subType);
        }

        QTreeWidgetItem *algItem = new QTreeWidgetItem(subTypeItem);
        algItem->setText(0, title);
        algItem->setIcon(0, fileIcon);
        algItem->setData(0, Qt::UserRole, info.absoluteFilePath());
    }

    algTree->expandAll();

    toolBox = new QToolBox;
    toolBox->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored));
    toolBox->setMinimumWidth(200);
    toolBox->addItem(itemWidget, tr("Элементы блок-схемы"));
    toolBox->addItem(algTree, tr("Доступные ИРЗ"));
}

// Создаёт действия интерфейса
void DiagramSceneDlg::createActions()
{
    toFrontAction = new QAction(QIcon(":/images_diag/bringtofront.png"),
                                tr("На &передний план"), this);
    toFrontAction->setShortcut(tr("Ctrl+F"));
    toFrontAction->setStatusTip(tr("Вывести элемент на передний план"));
    connect(toFrontAction, &QAction::triggered, this, &DiagramSceneDlg::bringToFront);

    sendBackAction = new QAction(QIcon(":/images_diag/sendtoback.png"), tr("На &задний план"), this);
    sendBackAction->setShortcut(tr("Ctrl+T"));
    sendBackAction->setStatusTip(tr("Вывести элемент на задний план"));
    connect(sendBackAction, &QAction::triggered, this, &DiagramSceneDlg::sendToBack);

    deleteAction = new QAction(QIcon(":/images_diag/delete.png"), tr("&Удалить"), this);
    deleteAction->setShortcut(tr("Delete"));
    deleteAction->setStatusTip(tr("Delete item from diagram"));
    connect(deleteAction, &QAction::triggered, this, &DiagramSceneDlg::deleteItem);

    propertiesAction = new QAction(tr("Свойства"), this);
    connect(propertiesAction, &QAction::triggered, this, &DiagramSceneDlg::openItemProperties);

    newAction = new QAction(tr("&Новый..."), this);
    newAction->setShortcuts(QKeySequence::New);
    newAction->setStatusTip(tr("Создать новый файл"));
    connect(newAction, &QAction::triggered, this, &QWidget::close);

    openAction = new QAction(tr("&Загрузить"), this);
    openAction->setShortcuts(QKeySequence::Open);
    openAction->setStatusTip(tr("Загрузить сохраненный файл"));
    connect(openAction, &QAction::triggered, this, &DiagramSceneDlg::loadFromJson);

    saveAction = new QAction(tr("&Сохранить"), this);
    saveAction->setShortcuts(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &DiagramSceneDlg::saveToJson);

    saveAsAction = new QAction(tr("&Сохранить как"), this);
    saveAsAction->setShortcuts(QKeySequence::SaveAs);
    connect(saveAsAction, &QAction::triggered, this, &DiagramSceneDlg::saveToJson);

    addAction = new QAction(tr("&Выбрать объект"), this);
    addAction->setStatusTip(tr("Выбор объекта из базы данных"));
    connect(addAction, &QAction::triggered, this, &DiagramSceneDlg::openObjectSelectDialog);

    backgroundAction = new QAction(tr("&Базовый фон"), this);
    backgroundAction->setStatusTip(tr("Настройка базового фона"));
    connect(backgroundAction, &QAction::triggered, this, &DiagramSceneDlg::openBackgroundSettings);

    exitAction = new QAction(tr("&Выход"), this);
    exitAction->setShortcuts(QKeySequence::Quit);
    exitAction->setStatusTip(tr("Закрыть программу"));
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    boldAction = new QAction(tr("Bold"), this);
    boldAction->setCheckable(true);
    QPixmap pixmap(":/images_diag/bold.png");
    boldAction->setIcon(QIcon(pixmap));
    boldAction->setShortcut(tr("Ctrl+B"));
    connect(boldAction, &QAction::triggered, this, &DiagramSceneDlg::handleFontChange);

    italicAction = new QAction(QIcon(":/images_diag/italic.png"), tr("Italic"), this);
    italicAction->setCheckable(true);
    italicAction->setShortcut(tr("Ctrl+I"));
    connect(italicAction, &QAction::triggered, this, &DiagramSceneDlg::handleFontChange);

    underlineAction = new QAction(QIcon(":/images_diag/underline.png"), tr("Underline"), this);
    underlineAction->setCheckable(true);
    underlineAction->setShortcut(tr("Ctrl+U"));
    connect(underlineAction, &QAction::triggered, this, &DiagramSceneDlg::handleFontChange);

    aboutAction = new QAction(tr("О программе"), this);
    aboutAction->setShortcut(tr("F1"));
    connect(aboutAction, &QAction::triggered, this, &DiagramSceneDlg::about);
}

// Создаёт меню приложения
void DiagramSceneDlg::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&Файл"));
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addAction(exitAction);

    settingsMenu = menuBar()->addMenu(tr("Настройки"));
    settingsMenu->addAction(backgroundAction);

    itemMenu = menuBar()->addMenu(tr("&Элемент"));
    itemMenu->addAction(propertiesAction);
    itemMenu->addAction(deleteAction);
    itemMenu->addSeparator();
    itemMenu->addAction(toFrontAction);
    itemMenu->addAction(sendBackAction);

    modelMenu = menuBar()->addMenu(tr("Создание модели"));
    modelMenu->addAction(addAction);
}

// Создаёт панели инструментов
void DiagramSceneDlg::createToolbars()
{
    editToolBar = addToolBar(tr("Редактировать"));
    editToolBar->addAction(deleteAction);
    editToolBar->addAction(toFrontAction);
    editToolBar->addAction(sendBackAction);

    fontCombo = new QFontComboBox();
    connect(fontCombo, &QFontComboBox::currentFontChanged,
            this, &DiagramSceneDlg::currentFontChanged);

    fontSizeCombo = new QComboBox;
    fontSizeCombo->setEditable(true);
    for (int i = 8; i < 30; i = i + 2)
        fontSizeCombo->addItem(QString().setNum(i));
    QIntValidator *validator = new QIntValidator(2, 64, this);
    fontSizeCombo->setValidator(validator);
    connect(fontSizeCombo, &QComboBox::currentTextChanged,
            this, &DiagramSceneDlg::fontSizeChanged);

    fontColorToolButton = new QToolButton;
    fontColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    fontColorToolButton->setMenu(createColorMenu(SLOT(textColorChanged()), Qt::black));
    textAction = fontColorToolButton->menu()->defaultAction();
    fontColorToolButton->setIcon(createColorToolButtonIcon(":/images_diag/textpointer.png", Qt::black));
    fontColorToolButton->setAutoFillBackground(true);
    connect(fontColorToolButton, &QAbstractButton::clicked,
            this, &DiagramSceneDlg::textButtonTriggered);

    fillColorToolButton = new QToolButton;
    fillColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    fillColorToolButton->setMenu(createColorMenu(SLOT(itemColorChanged()), Qt::white));
    fillAction = fillColorToolButton->menu()->defaultAction();
    fillColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images_diag/floodfill.png", Qt::white));
    connect(fillColorToolButton, &QAbstractButton::clicked,
            this, &DiagramSceneDlg::fillButtonTriggered);

    lineColorToolButton = new QToolButton;
    lineColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    QColor defaultLineColor = Qt::white;
    lineColorToolButton->setMenu(createColorMenu(SLOT(lineColorChanged()), defaultLineColor));
    lineAction = lineColorToolButton->menu()->defaultAction();
    lineColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images_diag/linecolor.png", defaultLineColor));
    connect(lineColorToolButton, &QAbstractButton::clicked,
            this, &DiagramSceneDlg::lineButtonTriggered);
    scene->setLineColor(defaultLineColor);

    textToolBar = addToolBar(tr("Font"));
    textToolBar->addWidget(fontCombo);
    textToolBar->addWidget(fontSizeCombo);
    textToolBar->addAction(boldAction);
    textToolBar->addAction(italicAction);
    textToolBar->addAction(underlineAction);

    colorToolBar = addToolBar(tr("Color"));
    colorToolBar->addWidget(fontColorToolButton);
    colorToolBar->addWidget(fillColorToolButton);
    colorToolBar->addWidget(lineColorToolButton);

    QToolButton *pointerButton = new QToolButton;
    pointerButton->setCheckable(true);
    pointerButton->setChecked(true);
    pointerButton->setIcon(QIcon(":/images_diag/pointer.png"));
    QToolButton *linePointerButton = new QToolButton;
    linePointerButton->setCheckable(true);
    linePointerButton->setIcon(QIcon(":/images_diag/linepointer.png"));

    pointerTypeGroup = new QButtonGroup(this);
    pointerTypeGroup->addButton(pointerButton, int(DiagramScene::MoveItem));
    pointerTypeGroup->addButton(linePointerButton, int(DiagramScene::InsertLine));
    connect(pointerTypeGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
            this, &DiagramSceneDlg::pointerGroupClicked);

    sceneScaleCombo = new QComboBox;
    QStringList scales;
    scales << tr("50%") << tr("75%") << tr("100%") << tr("125%") << tr("150%");
    sceneScaleCombo->addItems(scales);
    sceneScaleCombo->setCurrentIndex(2);
    connect(sceneScaleCombo, &QComboBox::currentTextChanged,
            this, &DiagramSceneDlg::sceneScaleChanged);

    pointerToolbar = addToolBar(tr("Pointer type"));
    pointerToolbar->addWidget(pointerButton);
    pointerToolbar->addWidget(linePointerButton);
    pointerToolbar->addWidget(sceneScaleCombo);
}

// Создаёт элемент выбора фонового изображения
QWidget *DiagramSceneDlg::createBackgroundCellWidget(const QString &text, const QString &image)
{
    QToolButton *button = new QToolButton;
    button->setText(text);
    button->setIcon(QIcon(image));
    button->setIconSize(QSize(50, 50));
    button->setCheckable(true);
    backgroundButtonGroup->addButton(button);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(button, 0, 0, Qt::AlignHCenter);
    layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    return widget;
}

// Создаёт элемент выбора стандартной блок-схемы
QWidget *DiagramSceneDlg::createCellWidget(const QString &text, DiagramItem::DiagramType type)
{

    DiagramItem item(type, itemMenu);
    QIcon icon(item.image());

    QToolButton *button = new QToolButton;
    button->setIcon(icon);
    button->setIconSize(QSize(50, 50));
    button->setCheckable(true);
    buttonGroup->addButton(button, int(type));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(button, 0, 0, Qt::AlignHCenter);
    layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    return widget;
}

// Создаёт меню выбора цвета
QMenu *DiagramSceneDlg::createColorMenu(const char *slot, QColor defaultColor)
{
    QList<QColor> colors;
    colors << Qt::black << Qt::white << Qt::red << Qt::blue << Qt::yellow;
    QStringList names;
    names << tr("black") << tr("white") << tr("red") << tr("blue")
          << tr("yellow");

    QMenu *colorMenu = new QMenu(this);
    for (int i = 0; i < colors.count(); ++i) {
        QAction *action = new QAction(names.at(i), this);
        action->setData(colors.at(i));
        action->setIcon(createColorIcon(colors.at(i)));
        connect(action, SIGNAL(triggered()), this, slot);
        colorMenu->addAction(action);
        if (colors.at(i) == defaultColor)
            colorMenu->setDefaultAction(action);
    }
    return colorMenu;
}

// Создаёт иконку для кнопки выбора цвета
QIcon DiagramSceneDlg::createColorToolButtonIcon(const QString &imageFile, QColor color)
{
    QPixmap pixmap(50, 80);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    QPixmap image(imageFile);
    // Draw icon centred horizontally on button.
    QRect target(4, 0, 42, 43);
    QRect source(0, 0, 42, 43);
    painter.fillRect(QRect(0, 60, 50, 80), color);
    painter.drawPixmap(target, image, source);

    return QIcon(pixmap);
}

// Создаёт одноцветную иконку
QIcon DiagramSceneDlg::createColorIcon(QColor color)
{
    QPixmap pixmap(20, 20);
    QPainter painter(&pixmap);
    painter.setPen(Qt::NoPen);
    painter.fillRect(QRect(0, 0, 20, 20), color);

    return QIcon(pixmap);
}
