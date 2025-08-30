#include "DObjectProperties.h"
#include "ui_DObjectProperties.h"

#include <QTableWidget>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QHeaderView>
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
        ui->tableWidget->setCellWidget(row, 2, editor);
        m_editors.insert(prop, editor);
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

