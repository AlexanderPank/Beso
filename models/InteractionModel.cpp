#include "InteractionModel.h"

#include <QHBoxLayout>

InteractionModel::InteractionModel(QObject *parent)
        : QObject(parent)
{}
QString InteractionModel::getTitle() const { return m_title; }
QString InteractionModel::getSourceObjectId() const { return m_sourceObjectId; }
QString InteractionModel::getInteractionType() const { return m_interactionType; }
QString InteractionModel::getTriggerCondition() const { return m_triggerCondition; }
QString InteractionModel::getResponseSetValues() const { return m_responseSetValues; }
QString InteractionModel::getResponseAction() const { return m_responseAction; }

QString InteractionModel::getId() const{ return m_id; }

void InteractionModel::setTitle(const QString &value) { m_title = value; }
void InteractionModel::setSourceObjectId(const QString &value) { m_sourceObjectId = value; }
void InteractionModel::setInteractionType(const QString &value) { m_interactionType = value; }
void InteractionModel::setTriggerCondition(const QString &value) { m_triggerCondition = value; }
void InteractionModel::setResponseSetValues(const QString &value) { m_responseSetValues = value; }
void InteractionModel::setResponseAction(const QString &value) { m_responseAction = value; }

void InteractionModel::setId(const QString &value){    m_id = value;}

QJsonObject InteractionModel::toJson() const
{
    QJsonObject json;
    json["title"] = m_title;
    json["source_object_id"] = m_sourceObjectId;
    json["interaction_type"] = m_interactionType;
    json["trigger_condition"] = m_triggerCondition;
    json["response_set_values"] = m_responseSetValues;
    json["response_action"] = m_responseAction;
    json["id"] = m_id;
    return json;
}

InteractionModel* InteractionModel::fromJson(const QJsonObject &json, QObject* parent)
{
    auto *model = new InteractionModel(parent);
    model->setTitle(json["title"].toString());
    model->setSourceObjectId(json["source_object_id"].toString());
    model->setInteractionType(json["interaction_type"].toString());
    model->setTriggerCondition(json["trigger_condition"].toString());
    model->setResponseSetValues(json["response_set_values"].toString());
    model->setResponseAction(json["response_action"].toString());
    model->setId(json["id"].toString());
    return model;
}

QTreeWidgetItem* InteractionModel::getTreeWidgetItem(QTreeWidgetItem* parent)
{
    if(treeWidgetItem == nullptr) {
        treeWidgetItem = new QTreeWidgetItem(parent);
        addEditButton();
    }else{
        for(auto child: treeWidgetItem->takeChildren())
            treeWidgetItem->removeChild(child);
    }

    treeWidgetItem->setText(0,   m_title.trimmed().size() > 0 ? m_title : m_interactionType);
    treeWidgetItem->setText(1, m_sourceObjectId);
    treeWidgetItem->setData(0, Qt::UserRole, QVariant::fromValue(const_cast<InteractionModel *>(this)));

    auto addChild = [this](const QString &key, const QString &value) {
        auto *child = new QTreeWidgetItem(treeWidgetItem);
        child->setText(0, key);
        child->setText(1, value);
    };

    addChild("trigger_condition", m_triggerCondition);
    addChild("response_set_values", m_responseSetValues);
    addChild("response_action", m_responseAction);

    return treeWidgetItem;
}

// Метод для отображения в дереве
void InteractionModel::addEditButton() {
    btnEdit = new QPushButton();
    btnEdit->setFixedSize(20, 20);
    QPixmap pixmap(":/icons/EDIT_ICON");
    QIcon icon(pixmap);
    btnEdit->setIcon(icon);
    btnEdit->setIconSize(QSize(15, 15));

    auto *container = new QWidget(treeWidgetItem->treeWidget());
    auto *layout = new QHBoxLayout(container);
    layout->setAlignment(Qt::AlignCenter);  // Выравнивание по центру
    layout->setContentsMargins(0, 0, 0, 0); // Убираем отступы
    layout->addWidget(btnEdit);
    treeWidgetItem->treeWidget()->setItemWidget(treeWidgetItem, 2, container);

    connect(btnEdit, &QPushButton::clicked, [this]() {
            emit editInteractionModel(this);
    });
}
