#ifndef SIGNANIMATOR_H
#define SIGNANIMATOR_H

#include <QHash>
#include <QObject>
#include <QPointF>
#include <QTimer>
#include <QString>

class SignBase;

class SignAnimator : public QObject
{
    Q_OBJECT
public:
    explicit SignAnimator(QObject *parent = nullptr) : QObject(parent) {}

    static void startAnimation(SignBase *sign, const QPointF &from,
                               const QPointF &to, double course,
                               int durationMs);

private:
    static QHash<QString, QTimer *> m_timers;
};

#endif // SIGNANIMATOR_H
