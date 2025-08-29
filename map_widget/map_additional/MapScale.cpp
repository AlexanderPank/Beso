#include "MapScale.h"
#include "ui_map_scale.h"
#include "../../core/QLogStream.h"
#include <QLabel>
#include <QListView>
#include <QObject>

MapScale::MapScale(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MapScale)
{
    ui->setupUi(this);

    QStringList scalesText;
    foreach (auto scale, scales) { scalesText.append(QString::number(scale)); }

    ui->scaleComboBox->addItems(scalesText);

    connect(ui->scaleComboBox, &QComboBox::currentTextChanged, [=](QString scaletext){
            emit scaleChanged(scaletext.toInt());
        }
    );

}

MapScale::~MapScale(){ delete ui;}


bool MapScale::setScale(long scale)
{
    if (!scales.contains(scale)) return false;
    scaleIndex = scales.indexOf(scale);
    ui->scaleComboBox->setCurrentText(QString::number(scale));
    return true;
}

long MapScale::getScale(int scale)
{
    if (scale == -1)  return scales[scaleIndex];
    else{
        if (scale>=0 && scale < scales.size())
            return scales[scale];
        qLog() << "Ошибка индекс scale выходит за рамки массива " << scale;
        qCritical() << "Ошибка индекс scale выходит за рамки массива ";
    }
    return scales[scaleIndex];
}

bool MapScale::changeScale(bool dec){
    if (dec && scaleIndex-1 < 0) return false;
    if (!dec && scaleIndex+1 >= scales.length()) return false;
    scaleIndex = dec ? scaleIndex -1  : scaleIndex + 1;
    setScale(scales[scaleIndex]);
    return true;
}

void MapScale::changeScale(int zoomIndex){
    if (zoomIndex >= scales.length()) return;
    setScale(scales[zoomIndex]);
}

int MapScale::getZoomIndex(int increment){
    if (scaleIndex + increment < 0) return 0;
    if (scaleIndex + increment >= scales.length()) return scaleIndex;
    return scaleIndex+increment;
}