#include "AlgorithmPropertiesDialog.h"

#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>

AlgorithmPropertiesDialog::AlgorithmPropertiesDialog(const QList<AlgorithmItem::PropertyInfo> &props, QWidget *parent)
    : QDialog(parent), m_inTable(new QTableWidget(this)), m_outTable(new QTableWidget(this))
{
    setWindowTitle(tr("Свойства алгоритма"));

    QStringList headers;
    headers << tr("Параметр") << tr("Тип") << tr("Наименование");

    m_inTable->setColumnCount(3);
    m_inTable->setHorizontalHeaderLabels(headers);
    m_inTable->verticalHeader()->setVisible(false);
    m_inTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_outTable->setColumnCount(3);
    m_outTable->setHorizontalHeaderLabels(headers);
    m_outTable->verticalHeader()->setVisible(false);
    m_outTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    int inCount = 0;
    int outCount = 0;
    for (const auto &p : props) {
        if (p.direction == 1) ++inCount;
        else if (p.direction == 2) ++outCount;
    }
    m_inTable->setRowCount(inCount);
    m_outTable->setRowCount(outCount);

    int row = 0;
    for (const auto &p : props) {
        if (p.direction != 1) continue;
        m_inTable->setItem(row, 0, new QTableWidgetItem(p.name));
        m_inTable->setItem(row, 1, new QTableWidgetItem(p.type));
        m_inTable->setItem(row, 2, new QTableWidgetItem(p.title));
        ++row;
    }
    row = 0;
    for (const auto &p : props) {
        if (p.direction != 2) continue;
        m_outTable->setItem(row, 0, new QTableWidgetItem(p.name));
        m_outTable->setItem(row, 1, new QTableWidgetItem(p.type));
        m_outTable->setItem(row, 2, new QTableWidgetItem(p.title));
        ++row;
    }

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(tr("Входные параметры")));
    layout->addWidget(m_inTable);
    layout->addWidget(new QLabel(tr("Выходные параметры")));
    layout->addWidget(m_outTable);

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Save)->setText(tr("Сохранить"));
    buttons->button(QDialogButtonBox::Cancel)->setText(tr("Отмена"));
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);

    resize(600, 400);
}

QList<AlgorithmItem::PropertyInfo> AlgorithmPropertiesDialog::properties() const
{
    QList<AlgorithmItem::PropertyInfo> res;
    for (int row = 0; row < m_inTable->rowCount(); ++row) {
        AlgorithmItem::PropertyInfo p;
        p.direction = 1;
        p.name = m_inTable->item(row, 0) ? m_inTable->item(row, 0)->text() : QString();
        p.type = m_inTable->item(row, 1) ? m_inTable->item(row, 1)->text() : QString();
        p.title = m_inTable->item(row, 2) ? m_inTable->item(row, 2)->text() : QString();
        res.append(p);
    }
    for (int row = 0; row < m_outTable->rowCount(); ++row) {
        AlgorithmItem::PropertyInfo p;
        p.direction = 2;
        p.name = m_outTable->item(row, 0) ? m_outTable->item(row, 0)->text() : QString();
        p.type = m_outTable->item(row, 1) ? m_outTable->item(row, 1)->text() : QString();
        p.title = m_outTable->item(row, 2) ? m_outTable->item(row, 2)->text() : QString();
        res.append(p);
    }
    return res;
}
