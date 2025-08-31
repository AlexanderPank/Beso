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

#ifndef DIAGRAMSCENE_H
#define DIAGRAMSCENE_H

#include "diagramitem.h"
#include "diagramtextitem.h"
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "algoritmitem.h"

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QMenu;
class QPointF;
class QGraphicsLineItem;
class QFont;
class QGraphicsTextItem;
class QColor;
class QGraphicsSceneDragDropEvent;
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

public slots:
    // Меняет текущий режим работы сцены
    void setMode(Mode mode);

    // Устанавливает тип добавляемого алгоритмического элемента
    void setItemType(AlgoritmItem::AlgoritmType type);

    // Обрабатывает потерю фокуса текстовым редактором
    void editorLostFocus(DiagramTextItem *item);

signals:
    // Сигнал о вставке нового элемента
    void itemInserted(AlgoritmItem *item);
    void textInserted(QGraphicsTextItem *item);
    void itemSelected(QGraphicsItem *item);
    void zoom(int i);

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

    QGraphicsView *parentView = new QGraphicsView();
    QPointF beginMousePos;
    QPointF center;
    QPointF newCenter;

    AlgoritmItem::AlgoritmType myItemType;
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
