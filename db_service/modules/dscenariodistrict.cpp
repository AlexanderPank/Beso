#include <QMessageBox>
#include "dscenariodistrict.h"
#include "ui_dscenariodistrict.h"

#include "../services/DataStorageServiceFactory.h"
#include "../services/models/DataStorageItem.h"

DScenarioDistrict::DScenarioDistrict(QWidget *parent) :
        QDialog(parent), ui(new Ui::DScenarioDistrict) {
    ui->setupUi(this);

    setWindowTitle("Создание сценария моделирования");

    QIcon icon5;
    icon5.addFile(QString::fromUtf8(":/icons/images/reload_white.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui->btnUpdate->setIcon(icon5);
    ui->btnUpdate->setIconSize(QSize(30, 30));

    loadUI();

    connect(ui->btnAdd, &QPushButton::clicked, this, &DScenarioDistrict::createScenario);
    connect(ui->btnCancel, &QPushButton::clicked, this, &DScenarioDistrict::reject);
    connect(ui->btnUpdate, &QPushButton::clicked, [=]() {
        DataStorageServiceFactory::getInstance()->loadData();
        loadUI();
    });
}

DScenarioDistrict::~DScenarioDistrict() {
    delete ui;
}

void DScenarioDistrict::loadUI() {
    ui->lwDistricts->clear();
    ui->btnUpdate->setVisible(false);
    ui->edtTitle->setStyleSheet("padding: 0px 6px; ");
    ui->lwDistricts->setStyleSheet( "QListWidget{ border: 0px solid #ccc;} "
                                    "QListWidget::item {"
                                    "    padding: 10px;"          // Внутренние отступы (padding)
                                    "    border: 0px solid #ccc;" // Граница (опционально)
                                    "}"

                                    // Стиль при наведении (hover)
                                    "QListWidget::item:hover {"
                                    "    background: #3d3d3d;"
                                    "}"

                                    // Стиль при выделении (active/selected)
                                    "QListWidget::item:selected {"
                                    "    background: #4d4d4d;"
                                    "}"

                                    // Стиль при выделении и наведении (selected + hover)
                                    "QListWidget::item:selected:hover {"
                                    "    background: #3d3d3d;"
                                    "}"

                                    );
    for(DataStorageItem* item: DataStorageServiceFactory::getInstance()->getElements(TypeElement::teDISTRICT)) {
        auto * newItem = new QListWidgetItem(item->getTitle());

        newItem->setData(Qt::UserRole, item->getData());
        ui->lwDistricts->addItem(newItem);
    }
}

QJsonObject DScenarioDistrict::getCurrentDistrict() {
    if(ui->lwDistricts->currentItem()) {
        QJsonObject currentJsonDistrict = ui->lwDistricts->currentItem()->data(Qt::UserRole).toJsonObject();
        currentJsonDistrict["scenario_id"] = ui->edtTitle->text();
        return currentJsonDistrict;
    }

    return QJsonObject();
}

void DScenarioDistrict::createScenario() {
    if(ui->edtTitle->text().isEmpty()) {
        QMessageBox::information(this, "Ошибка создания сценария", "Введите название сценария", QMessageBox::Ok);
        return;
    }

    emit accept();
}
