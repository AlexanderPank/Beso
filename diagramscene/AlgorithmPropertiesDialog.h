#ifndef ALGORITHMPROPERTIESDIALOG_H
#define ALGORITHMPROPERTIESDIALOG_H

#include <QDialog>
#include <QList>
#include "algorithmitem.h"

class QTableWidget;

class AlgorithmPropertiesDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AlgorithmPropertiesDialog(const QList<AlgorithmItem::PropertyInfo> &props, QWidget *parent = nullptr);
    QList<AlgorithmItem::PropertyInfo> properties() const;

private:
    QTableWidget *m_inTable;
    QTableWidget *m_outTable;
};

#endif // ALGORITHMPROPERTIESDIALOG_H
