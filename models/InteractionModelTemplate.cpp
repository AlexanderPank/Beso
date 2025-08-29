#include "InteractionModelTemplate.h"

InteractionModelTemplate::InteractionModelTemplate(QObject *parent)
        :QObject(parent)
{}

QJsonObject InteractionModelTemplate::toJson() const {
    QJsonObject json;
    json["title"] = m_title;
    json["type_object"] = m_typeObject;
    QJsonArray arrayItems;
    for(InteractionTemplateItem* item: m_items) {
        arrayItems.append(item->toJson());
    }
    json["object_list"] = arrayItems;
    QJsonArray arrayInteractionModel;
    for(InteractionModel* interactionModel: m_interactionModels){
        arrayInteractionModel.append(interactionModel->toJson());
    }
    json["interaction_list"] = arrayInteractionModel;
    json["id"] = m_id;
    return json;
}

InteractionModelTemplate* InteractionModelTemplate::fromJson(const QJsonObject &json, QObject* parent) {
    auto *model = new InteractionModelTemplate(parent);
    model->setId(json["id"].toString());
    model->setTitle(json["title"].toString());
    model->setTypeObject(json["type_object"].toString());
    for(auto iter: json["interaction_list"].toArray()) {
        QJsonObject jsonInteractionModel = iter.toObject();
        model->addInteractionModel(InteractionModel::fromJson(jsonInteractionModel, nullptr));
        model->addCurrentInteractionModel(InteractionModel::fromJson(jsonInteractionModel, nullptr));
    }

    for(QJsonValue item: json["object_list"].toArray()){
        model->addInteractionTemplateItem(InteractionTemplateItem::fromJson(item.toObject(), parent));
    }

    return model;
}

void InteractionModelTemplate::addInteractionTemplateItem(InteractionTemplateItem *interactionTemplateItem) {
    m_items.append(interactionTemplateItem);
    connect(interactionTemplateItem, &InteractionTemplateItem::updateTemplate, this, &InteractionModelTemplate::updateTemplate);
}

QListWidgetItem *InteractionModelTemplate::getListWidgetItem(QListWidget *listWidget) {
    auto listWidgetItem = new QListWidgetItem(m_title,listWidget);
    listWidgetItem->setSizeHint(QSize(132,30));
    listWidgetItem->setData(Qt::UserRole, QVariant::fromValue(const_cast<InteractionModelTemplate*>(this)));
    return listWidgetItem;
}

void InteractionModelTemplate::clear() {
    clearModel();
    for(InteractionTemplateItem* item: m_items) {
        item->clear();
    }
}

void InteractionModelTemplate::updateTemplate() {
    clearModel();
    for(InteractionTemplateItem* templateItem: m_items) {
        if(!templateItem->getValue().isEmpty()) {
            QString replaceName = QString("!%1!").arg(templateItem->getName());
            for(InteractionModel* currentModel: m_currentInteractionModels) {
                currentModel->setTriggerCondition(currentModel->getTriggerCondition().replace(replaceName, templateItem->getValue().value("id").toString()));
                currentModel->setSourceObjectId(currentModel->getSourceObjectId().replace(replaceName, templateItem->getValue().value("id").toString()));
                currentModel->setResponseSetValues(currentModel->getResponseSetValues().replace(replaceName, templateItem->getValue().value("id").toString()));
                currentModel->setResponseAction(currentModel->getResponseAction().replace(replaceName, templateItem->getValue().value("id").toString()));
            }
        }
    }

    emit updateTemplateData();
}

const QString &InteractionModelTemplate::getId() const
{
    return m_id;
}

void InteractionModelTemplate::setId(const QString &newId)
{
    m_id = newId;
}

void InteractionModelTemplate::addInteractionModel(InteractionModel *interactionModel) {
    m_interactionModels.append(interactionModel);
}

void InteractionModelTemplate::addCurrentInteractionModel(InteractionModel *interactionModel) {
    m_currentInteractionModels.append(interactionModel);
}

void InteractionModelTemplate::clearModel() {
    for(int i = 0; i < m_currentInteractionModels.size(); i++) {
        InteractionModel* currentModel = m_currentInteractionModels[i];
        InteractionModel* model = m_interactionModels[i];
        currentModel->setInteractionType(model->getInteractionType());
        currentModel->setResponseAction(model->getResponseAction());
        currentModel->setResponseSetValues(model->getResponseSetValues());
        currentModel->setSourceObjectId(model->getSourceObjectId());
        currentModel->setTriggerCondition(model->getTriggerCondition());
    }
}

void InteractionModelTemplate::currentToLocal() {
    for(int i = 0; i < m_currentInteractionModels.size(); i++) {
        InteractionModel* currentModel = m_currentInteractionModels[i];
        InteractionModel* model = m_interactionModels[i];
        model->setInteractionType(currentModel->getInteractionType());
        model->setResponseAction(currentModel->getResponseAction().replace("\n",""));
        model->setResponseSetValues(currentModel->getResponseSetValues().replace("\n",""));
        model->setSourceObjectId(currentModel->getSourceObjectId());
        model->setTriggerCondition(currentModel->getTriggerCondition().replace("\n",""));
    }
}


