#include "PropertiesDialog.h"

#include <QTableWidget>
#include <QHeaderView>
#include <QComboBox>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>

PropertiesDialog::PropertiesDialog(const QList<AlgorithmItem::PropertyInfo> &props, QWidget *parent)
    : QDialog(parent), m_table(new QTableWidget(props.size(), 4, this))
{
    setWindowTitle(tr("Свойства"));
    QVBoxLayout *layout = new QVBoxLayout(this);

    QStringList headers;
    headers << "" << tr("Title") << tr("Name") << tr("Type");
    m_table->setHorizontalHeaderLabels(headers);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->verticalHeader()->setVisible(false);

    int row = 0;
    for (const auto &p : props) {
        auto combo = new QComboBox;
        combo->addItems({tr("нет"), tr("вход"), tr("выход")});
        combo->setCurrentIndex(p.direction);
        m_table->setCellWidget(row, 0, combo);
        m_table->setItem(row, 1, new QTableWidgetItem(p.title));
        m_table->setItem(row, 2, new QTableWidgetItem(p.name));
        m_table->setItem(row, 3, new QTableWidgetItem(p.type));
        ++row;
    }

    layout->addWidget(m_table);

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Save)->setText(tr("Сохранить"));
    buttons->button(QDialogButtonBox::Cancel)->setText(tr("Отмена"));
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);

    resize(400, 300);
}

QList<AlgorithmItem::PropertyInfo> PropertiesDialog::properties() const
{
    QList<AlgorithmItem::PropertyInfo> res;
    for (int row = 0; row < m_table->rowCount(); ++row) {
        auto combo = qobject_cast<QComboBox*>(m_table->cellWidget(row,0));
        AlgorithmItem::PropertyInfo p;
        p.direction = combo ? combo->currentIndex() : 0;
        p.title = m_table->item(row,1)->text();
        p.name = m_table->item(row,2)->text();
        p.type = m_table->item(row,3)->text();
        res.append(p);
    }
    return res;
}

