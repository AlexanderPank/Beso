/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "arrow.h"
#include "diagramitem.h"
#include "diagramscene.h"
#include "diagramtextitem.h"
#include "DiagramSceneDlg.h"
#include "ObjectSelectDialog.h"

#include <QtWidgets>
#include <QDebug>
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

        AlgoritmItem temp(AlgoritmItem::ALGORITM, nullptr, title, inParams, outParams);
        temp.setBrush(QColor("#E3E3FD"));
        QGraphicsScene tmpScene;
        tmpScene.addItem(&temp);
        QRectF br = temp.boundingRect();
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
    createToolbars();

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(toolBox);
    view = new QGraphicsView(scene);
    view->setAcceptDrops(true);

    layout->addWidget(view);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    setCentralWidget(widget);
    setWindowTitle(tr("Редактор поведенческого ядра объекта"));
    setUnifiedTitleAndToolBarOnMac(true);
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
        scene->setItemType(AlgoritmItem::AlgoritmType(id));
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
            qgraphicsitem_cast<AlgoritmItem *>(arrow->_startItem()->parentItem())->removeArrow(arrow);
            qgraphicsitem_cast<AlgoritmItem *>(arrow->_endItem()->parentItem())->removeArrow(arrow);
            delete item;
        }
    }

    selectedItems = scene->selectedItems();
    for (QGraphicsItem *item : qAsConst(selectedItems)) {
         if (item->type() == AlgoritmItem::Type)
             qgraphicsitem_cast<AlgoritmItem *>(item)->removeArrows();
         scene->removeItem(item);
         delete item;
     }
}

// Изменяет режим указателя на сцене
void DiagramSceneDlg::pointerGroupClicked()
{
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
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
void DiagramSceneDlg::itemInserted(AlgoritmItem *item)
{
    pointerTypeGroup->button(int(DiagramScene::MoveItem))->setChecked(true);
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
}

void DiagramSceneDlg::openObjectSelectDialog()
{
    ObjectSelectDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        QString title = dlg.selectedTitle();
        QGraphicsRectItem *rect = new QGraphicsRectItem(0, 0, 150, 50);
        rect->setBrush(Qt::lightGray);
        rect->setFlag(QGraphicsItem::ItemIsMovable, true);
        rect->setFlag(QGraphicsItem::ItemIsSelectable, true);
        QGraphicsTextItem *text = new QGraphicsTextItem(title, rect);
        QRectF r = rect->rect();
        text->setPos(r.center().x() - text->boundingRect().width() / 2,
                     r.center().y() - text->boundingRect().height() / 2);
        QPointF centerPoint = view->mapToScene(view->viewport()->rect().center());
        rect->setPos(centerPoint - QPointF(r.width() / 2, r.height() / 2));
        scene->addItem(rect);
    }
}

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
    layout->addWidget(createCellWidget(tr("Conditional"), DiagramItem::Conditional), 0, 0);
    layout->addWidget(createCellWidget(tr("Process"), DiagramItem::Step),0, 1);
    layout->addWidget(createCellWidget(tr("Input/Output"), DiagramItem::Io), 1, 0);
    layout->addWidget(createCellWidget(tr("Start/End"), DiagramItem::StartEnd), 2, 0);

    QToolButton *textButton = new QToolButton;
    textButton->setCheckable(true);
    buttonGroup->addButton(textButton, InsertTextButton);
    textButton->setIcon(QIcon(QPixmap(":/images_diag/textpointer.png")));
    textButton->setIconSize(QSize(50, 50));
    QGridLayout *textLayout = new QGridLayout;
    textLayout->addWidget(textButton, 0, 0, Qt::AlignHCenter);
    textLayout->addWidget(new QLabel(tr("Text")), 1, 0, Qt::AlignCenter);
    QWidget *textWidget = new QWidget;
    textWidget->setLayout(textLayout);
    layout->addWidget(textWidget, 1, 1);

    layout->setRowStretch(3, 10);
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
    toolBox->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
    toolBox->setMinimumWidth(itemWidget->sizeHint().width());
    toolBox->addItem(itemWidget, tr("Основные блок-схемы"));
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

    newAction = new QAction(tr("&Новый..."), this);
    newAction->setShortcuts(QKeySequence::New);
    newAction->setStatusTip(tr("Создать новый файл"));
    connect(newAction, &QAction::triggered, this, &QWidget::close);

    openAction = new QAction(tr("&Загрузить"), this);
    openAction->setShortcuts(QKeySequence::Open);
    openAction->setStatusTip(tr("Загрузить сохраненный файл"));
    connect(openAction, &QAction::triggered, this, &QWidget::close);

    saveAction = new QAction(tr("&Сохранить"), this);
    saveAction->setShortcuts(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &QWidget::close);

    saveAsAction = new QAction(tr("&Сохранить как"), this);
    saveAsAction->setShortcuts(QKeySequence::SaveAs);
    connect(saveAsAction, &QAction::triggered, this, &QWidget::close);

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

    modelMenu = menuBar()->addMenu(tr("Создание модели"));
    modelMenu->addAction(addAction);

    itemMenu = menuBar()->addMenu(tr("&Элемент"));
    itemMenu->addAction(deleteAction);
    itemMenu->addSeparator();
    itemMenu->addAction(toFrontAction);
    itemMenu->addAction(sendBackAction);

    settingsMenu = menuBar()->addMenu(tr("Настройки"));
    settingsMenu->addAction(backgroundAction);

    aboutMenu = menuBar()->addMenu(tr("&Помощь"));
    aboutMenu->addAction(aboutAction);
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
    lineColorToolButton->setMenu(createColorMenu(SLOT(lineColorChanged()), Qt::black));
    lineAction = lineColorToolButton->menu()->defaultAction();
    lineColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images_diag/linecolor.png", Qt::black));
    connect(lineColorToolButton, &QAbstractButton::clicked,
            this, &DiagramSceneDlg::lineButtonTriggered);

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
