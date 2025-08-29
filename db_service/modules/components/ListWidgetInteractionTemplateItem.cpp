#include "ListWidgetInteractionTemplateItem.h"
#include "ui_ListWidgetInteractionTemplateItem.h"

#include <QComboBox>

ListWidgetInteractionTemplateItem::ListWidgetInteractionTemplateItem(QWidget *parent) :
        QWidget(parent), ui(new Ui::ListWidgetInteractionTemplateItem) {
    ui->setupUi(this);

    connect(ui->cbScenarioObjects, SIGNAL(currentIndexChanged(int)), this, SLOT(changedIndexObject(int)));
    ui->lblName->setVisible(false);
}

ListWidgetInteractionTemplateItem::~ListWidgetInteractionTemplateItem() {
    delete ui;
}

void ListWidgetInteractionTemplateItem::setData(const QString &descriptioin, const QString &name, QList<ObjectScenarioModel *> objectScenarioModels, QList<FeatureModel*> featureModels) {
    ui->lblDescription->setText(descriptioin);
    ui->lblName->setText(name);

    for(ObjectScenarioModel* objectScenarioModel: objectScenarioModels) {
        if(objectScenarioModel->containsProperty("is_own") && objectScenarioModel->getPropertyString("is_own") == "1") {
            ui->cbScenarioObjects->addItem(
                    QString("%1 (%2)").arg(objectScenarioModel->getTitle()).arg(objectScenarioModel->getId()),
                    objectScenarioModel->toJson());
        }
    }
    ui->cbScenarioObjects->addItem("-----------------------");
    for(ObjectScenarioModel* objectScenarioModel: objectScenarioModels) {
        if(objectScenarioModel->containsProperty("is_own") && objectScenarioModel->getPropertyString("is_own") == "0") {
            ui->cbScenarioObjects->addItem(
                    QString("%1 (%2)").arg(objectScenarioModel->getTitle()).arg(objectScenarioModel->getId()),
                    objectScenarioModel->toJson());
        }
    }
    ui->cbScenarioObjects->addItem("-----------------------");
    for(FeatureModel* featureModel: featureModels){
        ui->cbScenarioObjects->addItem(QString("%1 (%2)").arg(featureModel->getTitle()).arg(featureModel->getId()),
                                       featureModel->toJson());
    }
    ui->cbScenarioObjects->setCurrentIndex(-1);
}

void ListWidgetInteractionTemplateItem::changedIndexObject(int index) {
    if(ui->cbScenarioObjects->currentIndex() > -1)
        emit changeItem();
}

QJsonObject ListWidgetInteractionTemplateItem::getObject() {
    if(ui->cbScenarioObjects->currentIndex() > -1) {
        return ui->cbScenarioObjects->currentData(Qt::UserRole).toJsonObject();
    }

    return QJsonObject();
}

QString ListWidgetInteractionTemplateItem::getName() {
    return ui->lblName->text();
}

void ListWidgetInteractionTemplateItem::clear() {
//    if(ui->cbScenarioObjects->count() > 0)
//        ui->cbScenarioObjects->setCurrentIndex(-1);
}

void ListWidgetInteractionTemplateItem::setCurrentObject(ObjectScenarioModel *currentObject) {
    QString objTemplId = QString("(%1)").arg(currentObject->getId());
    for(int i = 0; i < ui->cbScenarioObjects->count();i++) {
        QString text = ui->cbScenarioObjects->itemText(i);
        if(text.contains(objTemplId)) {
            ui->cbScenarioObjects->setCurrentIndex(i);
            return;
        }
    }
}
