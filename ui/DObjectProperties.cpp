#include "DObjectProperties.h"
#include "ui_DObjectProperties.h"

#include <QTableWidget>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QHeaderView>
#include <QLocale>
#include <QHBoxLayout>
#include <limits>

#include "../models/PropertyModel.h"

class TrimDoubleSpinBox : public QDoubleSpinBox
{
public:
    using QDoubleSpinBox::QDoubleSpinBox;

protected:
    QString textFromValue(double value) const override
    {
        QLocale loc = locale();
        QString text = loc.toString(value, 'f', decimals());
        QChar dec = loc.decimalPoint();
        int idx = text.indexOf(dec);
        if (idx != -1) {
            int end = text.length() - 1;
            while (end > idx && text.at(end) == '0') {
                --end;
            }
            if (end == idx) {
                --end;
            }
            text.truncate(end + 1);
        }
        return text;
    }
};

DObjectProperties::DObjectProperties(const QString &title,
                                     const QMap<QString, PropertyModel*> &properties,
                                     QWidget *parent)
    : QDialog(parent),
      ui(new Ui::DObjectProperties),
      m_properties(properties)
{
    ui->setupUi(this);
    setWindowTitle(title);
    resize(600, 800);

    buildTable();
}

DObjectProperties::~DObjectProperties()
{
    delete ui;
}

void DObjectProperties::buildTable()
{
    ui->tableWidget->setColumnCount(3);
    QStringList headers{"Title", "Parameter", "Value"};
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Interactive);
    ui->tableWidget->setColumnWidth(1, 100);
    ui->tableWidget->setColumnWidth(2, 100);

    int row = 0;
    for (auto prop : m_properties) {
        ui->tableWidget->insertRow(row);
        auto titleItem = new QTableWidgetItem(prop->title().isEmpty() ? prop->name() : prop->title());
        titleItem->setFlags(titleItem->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget->setItem(row, 0, titleItem);

        auto nameItem = new QTableWidgetItem(prop->name());
        nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget->setItem(row, 1, nameItem);

        QString type = prop->type();
        if (type == PropertyModel::ValueType::BOOL) {
            QCheckBox *cb = new QCheckBox(ui->tableWidget);
            cb->setChecked(prop->value().toBool());
            auto container = new QWidget(ui->tableWidget);
            auto layout = new QHBoxLayout(container);
            layout->setContentsMargins(0, 0, 0, 0);
            layout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            layout->addWidget(cb);
            ui->tableWidget->setCellWidget(row, 2, container);
            m_editors.insert(prop, cb);
        } else if (type == PropertyModel::ValueType::INT) {
            QSpinBox *sb = new QSpinBox(ui->tableWidget);
            sb->setRange(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
            sb->setValue(prop->value().toInt());
            sb->setAlignment(Qt::AlignRight);
            ui->tableWidget->setCellWidget(row, 2, sb);
            m_editors.insert(prop, sb);
        } else if (type == PropertyModel::ValueType::FLOAT || type == PropertyModel::ValueType::DOUBLE) {
            TrimDoubleSpinBox *dsb = new TrimDoubleSpinBox(ui->tableWidget);
            dsb->setRange(-1e9, 1e9);
            dsb->setDecimals(6);
            dsb->setValue(prop->value().toDouble());
            dsb->setAlignment(Qt::AlignRight);
            ui->tableWidget->setCellWidget(row, 2, dsb);
            m_editors.insert(prop, dsb);
        } else {
            QLineEdit *le = new QLineEdit(prop->stringValue(), ui->tableWidget);
            le->setAlignment(Qt::AlignRight);
            ui->tableWidget->setCellWidget(row, 2, le);
            m_editors.insert(prop, le);
        }
        row++;
    }

    ui->tableWidget->setColumnHidden(1, true);
}

void DObjectProperties::accept()
{
    for (auto it = m_editors.constBegin(); it != m_editors.constEnd(); ++it) {
        PropertyModel *prop = it.key();
        QWidget *editor = it.value();
        if (auto cb = qobject_cast<QCheckBox*>(editor)) {
            prop->setValue(cb->isChecked());
        } else if (auto sb = qobject_cast<QSpinBox*>(editor)) {
            prop->setValue(sb->value());
        } else if (auto dsb = qobject_cast<QDoubleSpinBox*>(editor)) {
            prop->setValue(dsb->value());
        } else if (auto le = qobject_cast<QLineEdit*>(editor)) {
            prop->setValue(le->text());
        }
    }
    QDialog::accept();
}

void DObjectProperties::on_btnToggleParam_clicked()
{
    bool hidden = ui->tableWidget->isColumnHidden(1);
    ui->tableWidget->setColumnHidden(1, !hidden);
    ui->btnToggleParam->setText(hidden ? tr("Скрыть параметр") : tr("Показать параметр"));
}

