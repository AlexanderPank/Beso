#ifndef SCENARIOCLIENT_INTERACTIONTEMPLATEITEM_H
#define SCENARIOCLIENT_INTERACTIONTEMPLATEITEM_H

#include <QObject>
#include <QListWidgetItem>

#include "ObjectScenarioModel.h"
#include "../db_service/modules/components/ListWidgetInteractionTemplateItem.h"
#include "FeatureModel.h"

class InteractionTemplateItem: public QObject {
    Q_OBJECT
public:
    explicit InteractionTemplateItem(QObject *parent = nullptr);

    void clear();

    //Getter
    QString getName(){return  m_name;}
    QString getDescription() {return m_description;}
    QJsonObject getValue() {return m_value; }
    //Setter
    void setName(const QString &name) {m_name = name;}
    void setDescription(const QString &description) {m_description = description;}
    void setValue(QJsonObject value) {m_value = value;}

    QJsonObject toJson() const;
    static InteractionTemplateItem* fromJson(const QJsonObject &json, QObject* parent);

    QListWidgetItem* getListWidgetItem(const QList<ObjectScenarioModel*> &objectScenarioModels, QList<FeatureModel*> &featureModels, QListWidget* parent = nullptr);

    void setCurrentObject(ObjectScenarioModel* currentObject);

signals:
    void updateTemplate();

private slots:
    void changeItem();

private:
    QString m_name;
    QString m_description;
    QJsonObject m_value;

    ListWidgetInteractionTemplateItem* widgetItem;
};

#endif //SCENARIOCLIENT_INTERACTIONTEMPLATEITEM_H
