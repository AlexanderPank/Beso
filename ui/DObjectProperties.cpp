#include "DObjectProperties.h"
#include "ui_DObjectProperties.h"

#include <QTableWidget>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <limits>

#include "../models/PropertyModel.h"

DObjectProperties::DObjectProperties(const QString &title,
                                     const QMap<QString, PropertyModel*> &properties,
                                     QWidget *parent)
    : QDialog(parent),
      ui(new Ui::DObjectProperties),
      m_properties(properties)
{
    ui->setupUi(this);
    setWindowTitle(title);

    buildTable();
}

DObjectProperties::~DObjectProperties()
{
    delete ui;
}

void DObjectProperties::buildTable()
{
    ui->tableWidget->setColumnCount(2);
    QStringList headers{"Name", "Value"};
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

    int row = 0;
    for (auto prop : m_properties) {
        ui->tableWidget->insertRow(row);
        auto nameItem = new QTableWidgetItem(prop->title().isEmpty() ? prop->name() : prop->title());
        nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget->setItem(row, 0, nameItem);

        QWidget *editor = nullptr;
        QString type = prop->type();
        if (type == PropertyModel::ValueType::BOOL) {
            QCheckBox *cb = new QCheckBox(ui->tableWidget);
            cb->setChecked(prop->value().toBool());
            editor = cb;
        } else if (type == PropertyModel::ValueType::INT) {
            QSpinBox *sb = new QSpinBox(ui->tableWidget);
            sb->setRange(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
            sb->setValue(prop->value().toInt());
            editor = sb;
        } else if (type == PropertyModel::ValueType::FLOAT || type == PropertyModel::ValueType::DOUBLE) {
            QDoubleSpinBox *dsb = new QDoubleSpinBox(ui->tableWidget);
            dsb->setRange(-1e9, 1e9);
            dsb->setDecimals(6);
            dsb->setValue(prop->value().toDouble());
            editor = dsb;
        } else {
            QLineEdit *le = new QLineEdit(prop->stringValue(), ui->tableWidget);
            editor = le;
        }
        ui->tableWidget->setCellWidget(row, 1, editor);
        m_editors.insert(prop, editor);
        row++;
    }
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

