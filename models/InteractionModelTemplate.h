#ifndef SCENARIOCLIENT_INTERACTIONMODELTEMPLATE_H
#define SCENARIOCLIENT_INTERACTIONMODELTEMPLATE_H

#include <QJsonArray>
#include <QListWidgetItem>

#include "InteractionModel.h"
#include "ObjectScenarioModel.h"
#include "InteractionTemplateItem.h"

class InteractionModelTemplate: public QObject {
Q_OBJECT

public:
    explicit InteractionModelTemplate(QObject *parent = nullptr);

    void clear();

    void currentToLocal();

    //Getter
    QList<InteractionTemplateItem*> getItems() const {return m_items;}
    QString getTitle() const {return m_title;}
    QString getTypeObject() const {return  m_typeObject;}
    QList<InteractionModel*> getInteractionModels() { return m_interactionModels; }
    QList<InteractionModel*> getCurrentInteractionModels() { return m_currentInteractionModels; }

    //Setter
   void setItems(const QList<InteractionTemplateItem*> itms) {m_items = itms;}
    void addInteractionTemplateItem(InteractionTemplateItem* interactionTemplateItem);
    void setTitle(const QString &title) {m_title = title;}
    void setTypeObject(const QString &typeObject) {m_typeObject = typeObject;}
    void addInteractionModel(InteractionModel* interactionModel);
    void addCurrentInteractionModel(InteractionModel* interactionModel);

    QJsonObject toJson() const;
    static InteractionModelTemplate* fromJson(const QJsonObject &json, QObject* parent);

    QListWidgetItem* getListWidgetItem(QListWidget* listWidget);

    const QString &getId() const;
    void setId(const QString &newId);

signals:
    void updateTemplateData();

private slots:
    void updateTemplate();

private:
    void clearModel();

    QString m_title;
    QString m_typeObject;
    QList<InteractionTemplateItem*> m_items;
    QList<InteractionModel*> m_interactionModels;
    QList<InteractionModel*> m_currentInteractionModels;
    QString m_id;
};


#endif //SCENARIOCLIENT_INTERACTIONMODELTEMPLATE_H
