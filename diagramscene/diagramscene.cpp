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

#include "diagramscene.h"
#include "arrow.h"

#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>

//! [0]
DiagramScene::DiagramScene(QMenu *itemMenu, QObject *parent)
    : QGraphicsScene(parent)
{
    myItemMenu = itemMenu;
    myMode = MoveItem;
    myItemType = AlgoritmItem::ALGORITM;//DiagramItem::Step;
    line = nullptr;
    textItem = nullptr;
    myItemColor = Qt::white;
    myTextColor = Qt::black;
    myLineColor = Qt::black;
            center = sceneRect().center();
}
//! [0]

//! [1]
void DiagramScene::setLineColor(const QColor &color)
{
    myLineColor = color;
    if (isItemChange(Arrow::Type)) {
        Arrow *item = qgraphicsitem_cast<Arrow *>(selectedItems().first());
        item->setColor(myLineColor);
        update();
    }
}
//! [1]

//! [2]
void DiagramScene::setTextColor(const QColor &color)
{
    myTextColor = color;
    if (isItemChange(DiagramTextItem::Type)) {
        DiagramTextItem *item = qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());
        item->setDefaultTextColor(myTextColor);
    }
}
//! [2]

//! [3]
void DiagramScene::setItemColor(const QColor &color)
{
    myItemColor = color;
    if (isItemChange(DiagramItem::Type)) {
//        DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(selectedItems().first());
        AlgoritmItem *item = qgraphicsitem_cast<AlgoritmItem *>(selectedItems().first());
        item->setBrush(myItemColor);
    }
}
//! [3]

//! [4]
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

void DiagramScene::setView(QGraphicsView *view)
{
    parentView = view;
}
//! [4]

void DiagramScene::setMode(Mode mode)
{
    myMode = mode;
}

void DiagramScene::setItemType(AlgoritmItem::AlgoritmType type)
{
    myItemType = type;
}

//! [5]
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
//! [5]

//! [6]
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
//            item = new DiagramItem(myItemType, myItemMenu);
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
//! [6] //! [7]
        case InsertLine:
            line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(),
                                        mouseEvent->scenePos()));
            line->setPen(QPen(myLineColor, 2));
            addItem(line);
            break;
//! [7] //! [8]
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
//! [8] //! [9]
    default:
        ;
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}
//! [9]

//! [10]
void DiagramScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (myMode == MoveFullScene){

        newCenter = QPointF(center.x() + beginMousePos.x() - QCursor::pos().x(),
                         center.y() + beginMousePos.y() - QCursor::pos().y());
//        qDebug()<<"center"<<center<<" beginMousePos"<<beginMousePos<<" mouseEvent->pos()"<<QCursor::pos();
        parentView->centerOn(newCenter);

    }
    if (myMode == InsertLine && line != nullptr) {
        QLineF newLine(line->line().p1(), mouseEvent->scenePos());
        line->setLine(newLine);
    } else if (myMode == MoveItem) {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}
//! [10]

//! [11]
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
//! [11] //! [12]

        if (startItem && startItem->parentItem()
                && endItem && endItem->parentItem()) {

            Arrow *arrow = new Arrow(startItem, endItem);
            arrow->setColor(myLineColor);
            static_cast<AlgoritmItem*>(startItem->parentItem())->addArrow(arrow);
            static_cast<AlgoritmItem*>(endItem->parentItem())->addArrow(arrow);
//            arrow->setZValue(-1);
            addItem(arrow);
            arrow->updatePosition();
        }
    }
//! [12] //! [13]
    line = nullptr;
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void DiagramScene::wheelEvent(QGraphicsSceneWheelEvent *mouseEvent)
{
    if (mouseEvent->delta()>0)
        emit zoom(1);
    else
        emit zoom(-1);
}
//! [13]

//! [14]
bool DiagramScene::isItemChange(int type) const
{
    const QList<QGraphicsItem *> items = selectedItems();
    const auto cb = [type](const QGraphicsItem *item) { return item->type() == type; };
    return std::find_if(items.begin(), items.end(), cb) != items.end();
}
//! [14]
