#ifndef PROPERTIESDIALOG_H
#define PROPERTIESDIALOG_H

#include <QDialog>
#include <QList>
#include "algorithmitem.h"

class QTableWidget;

class PropertiesDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PropertiesDialog(const QList<AlgorithmItem::PropertyInfo> &props, QWidget *parent = nullptr);
    QList<AlgorithmItem::PropertyInfo> properties() const;

private:
    QTableWidget *m_table;
};

#endif // PROPERTIESDIALOG_H

