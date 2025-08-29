#ifndef MAP_SCALE_H
#define MAP_SCALE_H
#include <QAction>

#include <QWidget>

namespace Ui {
class MapScale;
}

class MapScale : public QWidget
{
    Q_OBJECT

public:
    explicit MapScale(QWidget *parent = nullptr);
    ~MapScale();
    bool setScale(long scale);
    long getScale(int scale = -1);
    bool changeScale(bool dec);
    void changeScale(int zoomIndex);
    int getZoomIndex(int icrement = 0);

signals:
    void scaleChanged(long scale);

private:
    Ui::MapScale *ui;
    const QList<int> scales = { 1000, 2500, 5000, 10000, 25000, 50000, 75000, 100000,150000, 200000, 500000};
    int scaleIndex = 2;
};

#endif // MAP_SCALE_H
