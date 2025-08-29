#include <QButtonGroup>
#include <QPushButton>

#include "ScenarioTacticalObjects.h"
#include "ui_ScenarioTacticalObjects.h"


ScenarioTacticalObjects::ScenarioTacticalObjects(QWidget *parent) :
        QWidget(parent), ui(new Ui::ScenarioTacticalObjects) {
    ui->setupUi(this);

    ui->lwTacticalSigns->setViewMode    (QListWidget::IconMode);
//    ui->lwTacticalSigns->setResizeMode  (QListWidget::Adjust);
    ui->lwTacticalSigns->setIconSize    (QSize(100,100));
    ui->lwTacticalSigns->setAcceptDrops (true);
    ui->lwTacticalSigns->setDragEnabled (false);
    ui->lwTacticalSigns->setWordWrap    (true);

    buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->btnBPLA);
    buttonGroup->addButton(ui->btnBEK);
    ui->btnBPLA->setChecked(true);

    ui->btnBEK->setVisible(false);
    ui->btnBPLA->setVisible(false);

    selectTacticalSign();

//    connect(buttonGroup, &QButtonGroup::idClicked, this, &ScenarioTacticalObjects::selectTacticalSign);
    connect(ui->btnAddTacticalSign, &QPushButton::clicked, this, &ScenarioTacticalObjects::onBtnAddTacticalSign);
}

ScenarioTacticalObjects::~ScenarioTacticalObjects() {
    delete ui;
}

void ScenarioTacticalObjects::selectTacticalSign(QString type, QString subType) {
    ui->lwTacticalSigns->clear();
    m_tacticalSignModels.clear();

    for(DataStorageItem* dataStorageItem: DataStorageServiceFactory::getInstance()->getElements(TypeElement::teTACTICAL_SIGN)) {
        TacticalSignModel* tacticalSignModel = TacticalSignModel::fromJson(dataStorageItem->getData(), this);
        QString groupSign = tacticalSignModel->getGroup();
        if(groupSign != "*") {
            QStringList listTypes = groupSign.split('|');
            bool isShow = false;
            for(QString itemType: listTypes) {
                QString iType = "*";
                QString iSubType = "*";
                QStringList spliItemType = itemType.split(":");
                iType = spliItemType.at(0);
                if(spliItemType.size() > 1)
                    iSubType = spliItemType.at(1);
                if(iType == type && (iSubType == "*" || iSubType == subType)) {
                    isShow = true;
                    break;
                }
            }
            if(!isShow)
                continue;
        }
        m_tacticalSignModels.append(tacticalSignModel);
        ui->lwTacticalSigns->addItem(tacticalSignModel->getListWidgetItem(ui->lwTacticalSigns));
    }
}

void ScenarioTacticalObjects::onBtnAddTacticalSign() {
    QListWidgetItem * item = ui->lwTacticalSigns->currentItem();
    if(item) {
        auto * tacticalSignModel = item->data(Qt::UserRole).value<TacticalSignModel*>();
        emit addTacticalSign(tacticalSignModel->getClassCod(), tacticalSignModel->getData());
    }
}
