#include "SignAnimator.h"
#include "SignBase.h"
#include "SignController.h"

QHash<QString, QTimer *> SignAnimator::m_timers;

void SignAnimator::startAnimation(SignBase *sign, const QPointF &from,
                                  const QPointF &to, double course,
                                  int durationMs)
{
    if (!sign)
        return;

    // If coordinates are missing, perform immediate update
    if ((from.isNull() || (from.x() == 0 && from.y() == 0)) ||
        (to.isNull() || (to.x() == 0 && to.y() == 0))) {
        sign->setCoordinatesInDegrees({to}, course);
        SignController::getInstance()->updateSignOnMap(sign);
        return;
    }

    QString key = sign->getUuid().toString();
    if (m_timers.contains(key)) {
        QTimer *old = m_timers.take(key);
        old->stop();
        old->deleteLater();
    }

    const int interval = 50;
    QTimer *timer = new QTimer();
    timer->setInterval(interval);
    int elapsed = 0;

    QObject::connect(timer, &QTimer::timeout, [=]() mutable {
        elapsed += interval;
        double t = qMin(1.0, static_cast<double>(elapsed) / durationMs);
        double lat = from.x() + (to.x() - from.x()) * t;
        double lon = from.y() + (to.y() - from.y()) * t;
        sign->setCoordinatesInDegrees({QPointF(lat, lon)}, course);
        SignController::getInstance()->updateSignOnMap(sign);
        if (t >= 1.0) {
            timer->stop();
            timer->deleteLater();
            m_timers.remove(key);
        }
    });

    m_timers.insert(key, timer);
    timer->start();
}
