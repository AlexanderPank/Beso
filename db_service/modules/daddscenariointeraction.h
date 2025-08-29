#ifndef SCENARIOCLIENT_DADDSCENARIOINTERACTION_H
#define SCENARIOCLIENT_DADDSCENARIOINTERACTION_H

#include <QDialog>

#include "ui_daddscenariointeraction.h"

#include "../../models/InteractionModelTemplate.h"
#include "../services/models/DataStorageItem.h"
#include "../../models/FeatureModel.h"

namespace Ui {
    class DAddScenarioInteraction;
}

class DAddScenarioInteraction : public QDialog {
Q_OBJECT

public:
    explicit DAddScenarioInteraction(bool isEdit = false, QWidget *parent = nullptr);

    void setObjects(const QList<ObjectScenarioModel*> &objects, const QList<FeatureModel*> features) {m_objects = objects; m_features = features;}
    void setCurrentObject(ObjectScenarioModel* currentObject);

    ~DAddScenarioInteraction() override;

    void setEditInteracionModel(InteractionModel &interactionModel);

    QJsonArray getJsonInteractionModels() {return currentInteractionModelTemplate->toJson()["interaction_list"].toArray();}

private slots:
    void changeTemplate();
    void updateParameter();
    void updateTemplateParameter();

    void bntAdd_Clicked();

    void btnUpdate_Clicked();

    void updateCurrentInteractionModel(int);

private:
    Ui::DAddScenarioInteraction *ui;

    QList<InteractionModelTemplate*> m_templates;

    void loadUi();

    void updateCurrentTemplate();

//    void getParametersFromString(QString idObject, const QString &text, QMap<QString, QString> &parameters);

    InteractionModelTemplate* currentInteractionModelTemplate = nullptr;

    QList<ObjectScenarioModel*> m_objects;
    QList<FeatureModel*> m_features;

    InteractionModel* m_editInteractionModel;

    ObjectScenarioModel* m_currentObject;

    bool m_isEdit = false;
};


#endif //SCENARIOCLIENT_DADDSCENARIOINTERACTION_H
