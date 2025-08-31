#include "PropertiesDialog.h"

#include <QTableWidget>
#include <QHeaderView>
#include <QComboBox>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLineEdit>
#include <QColor>

PropertiesDialog::PropertiesDialog(const QList<AlgorithmItem::PropertyInfo> &props, QWidget *parent)
    : QDialog(parent), m_table(new QTableWidget(props.size(), 4, this))
{
    setWindowTitle(tr("Свойства"));
    QVBoxLayout *layout = new QVBoxLayout(this);

    auto filterEdit = new QLineEdit(this);
    filterEdit->setPlaceholderText(tr("Фильтр"));
    layout->addWidget(filterEdit);

    QStringList headers;
    headers << "" << tr("Title") << tr("Name") << tr("Type");
    m_table->setHorizontalHeaderLabels(headers);
    m_table->verticalHeader()->setVisible(false);

    m_table->setStyleSheet(
            "QTableView { border: 1px solid #444; gridline-color: #444; color: #fefefe;  }"
            "QTableView:hover { border: 1px solid #444;  }"
            "QTableView QLineEdit { background: transparent; border: none; }"
            "QTableView QSpinBox { background: transparent; border: none; }"
            "QTableView QComboBox { background: transparent; border: none; }"
            "QTableView QDoubleSpinBox { background: transparent; border: none; }"
            "QTableView QHeaderView::section {"
            "  background-color: #2a2a2a;"
            "  color: #aaa;"
            "  padding: 4px 6px;"
            "}"
    );

    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Interactive);
    m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Interactive);
    m_table->setColumnWidth(0, 100);
    m_table->setColumnWidth(2, 180);
    m_table->setColumnWidth(3, 100);

    int row = 0;
    for (const auto &p : props) {
        auto combo = new QComboBox;
        combo->addItems({tr("нет"), tr("вход"), tr("выход")});
        combo->setCurrentIndex(p.direction);
        m_table->setCellWidget(row, 0, combo);
        m_table->setItem(row, 1, new QTableWidgetItem(p.title));
        m_table->setItem(row, 2, new QTableWidgetItem(p.name));
        m_table->setItem(row, 3, new QTableWidgetItem(p.type));

        auto updateRow = [this, row]() {
            auto combo = qobject_cast<QComboBox*>(m_table->cellWidget(row,0));
            int dir = combo ? combo->currentIndex() : 0;
            QColor bg = (dir == 0) ? Qt::transparent : QColor("#333");
            QColor fg = (dir == 0) ? m_table->palette().text().color() : QColor("#fff");
            for (int col = 1; col < m_table->columnCount(); ++col) {
                if (auto item = m_table->item(row, col)) {
                    item->setBackground(bg);
                    item->setForeground(fg);
                }
            }
            if (combo)
                combo->setStyleSheet(dir == 0 ? "" : "QComboBox { background:#333; color:#fff; }");
        };
        updateRow();
        connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                [updateRow](int){ updateRow(); });

        ++row;
    }

    connect(filterEdit, &QLineEdit::textChanged, this, [this](const QString &text){
        for (int row = 0; row < m_table->rowCount(); ++row) {
            bool match = m_table->item(row,1)->text().contains(text, Qt::CaseInsensitive) ||
                         m_table->item(row,2)->text().contains(text, Qt::CaseInsensitive);
            m_table->setRowHidden(row, !match);
        }
    });

    layout->addWidget(m_table);

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Save)->setText(tr("Сохранить"));
    buttons->button(QDialogButtonBox::Cancel)->setText(tr("Отмена"));
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);

    resize(800, 600);
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

