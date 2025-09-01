#include "MapCompass.h"
#include <QPainter>
#include <QEvent>
#include <QResizeEvent>

MapCompass::MapCompass(QWidget* parent)
        : QWidget(parent)
{
    // Прозрачный фон и не перехватываем мышь
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_TransparentForMouseEvents, true);

    // Загружаем картинку из ресурса с алиасом ":/COMPAS"
    QPixmap pm(":/icons/COMPAS");
    if (!pm.isNull()) {
        m_pm = pm;
        setFixedSize(m_pm.width(), m_pm.height());
        move(20, 10);
    }

    show();
}

void MapCompass::paintEvent(QPaintEvent*)
{
    if (m_pm.isNull()) return;
    QPainter p(this);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);
    p.drawPixmap(0, 0, m_pm);

}