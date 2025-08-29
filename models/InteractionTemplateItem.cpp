#include "InteractionTemplateItem.h"
//===================InteractionTemplateItem===============
InteractionTemplateItem::InteractionTemplateItem(QObject *parent)
        : QObject(parent) {
}

QJsonObject InteractionTemplateItem::toJson() const {
    QJsonObject json;
    json["name"] = m_name;
    json["description"] = m_description;
    return json;
}

InteractionTemplateItem* InteractionTemplateItem::fromJson(const QJsonObject &json, QObject* parent) {
    auto *model = new InteractionTemplateItem(parent);
    model->setName(json["name"].toString());
    model->setDescription(json["description"].toString());
    return model;
}

QListWidgetItem *
InteractionTemplateItem::getListWidgetItem(const QList<ObjectScenarioModel *> &objectScenarioModels, QList<FeatureModel*> &featureModels, QListWidget *parent) {
    auto listWidgetItem = new QListWidgetItem(parent);
    listWidgetItem->setSizeHint(QSize(132,75));
    parent->addItem(listWidgetItem);
    widgetItem = new ListWidgetInteractionTemplateItem();
    widgetItem->setData(m_description, m_name, objectScenarioModels, featureModels);
    connect(widgetItem, SIGNAL(changeItem()), this, SLOT(changeItem()));

    parent->setItemWidget(listWidgetItem, widgetItem);
    return listWidgetItem;
}

void InteractionTemplateItem::changeItem() {
    m_value = widgetItem->getObject();
    emit updateTemplate();
}

void InteractionTemplateItem::clear() {
    m_value = QJsonObject();
    widgetItem->clear();
}

void InteractionTemplateItem::setCurrentObject(ObjectScenarioModel *currentObject) {
    widgetItem->setCurrentObject(currentObject);
}
