#pragma once
#include <QWidget>
#include <QPixmap>

class MapCompass : public QWidget
{
Q_OBJECT
public:
    explicit MapCompass(QWidget* parent = nullptr);


protected:
    void paintEvent(QPaintEvent* e) override;

private:

    QPixmap m_pm;
    int     m_topMargin   = 10;
};
