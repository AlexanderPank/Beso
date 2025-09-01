#pragma once
#ifndef SCENARIOCLIENT_SIMULATIONEVENTWIDGETINMAP_H
#define SCENARIOCLIENT_SIMULATIONEVENTWIDGETINMAP_H

#include <QWidget>
#include <QTextEdit>
#include <QJsonArray>
#include <QTimer>
#include <QScrollBar>

class SimulationEventWidgetInMap : public QWidget {
Q_OBJECT
public:
    explicit SimulationEventWidgetInMap(QWidget *parent = nullptr);
    ~SimulationEventWidgetInMap();
    void clearEvents() { m_textEdit->clear(); m_wasScroll = 0; m_filter_text = ""; m_log_text_list.clear();}

public slots:
    void updateEventList(const QJsonArray &events);
    void filterLog();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    QTextEdit *m_textEdit;
    QTextEdit *m_textEditFilter;
    QPoint m_dragStartPosition;
    bool m_isDragging = false;
    bool m_isResizing = false;
    QColor getColorForType(const QString &type) const;
    const int RESIZE_HANDLE_SIZE = 10;
    int m_wasScroll = 0;
    QTimer* updateTimer;
    QStringList m_log_text_list;
    QString m_log_text="";
    QString m_filter_text="";
};


#endif //SCENARIOCLIENT_SIMULATIONEVENTWIDGETINMAP_H
