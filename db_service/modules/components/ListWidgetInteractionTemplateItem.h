//
// Created by qq on 18.05.25.
//

#ifndef SCENARIOCLIENT_LISTWIDGETINTERACTIONTEMPLATEITEM_H
#define SCENARIOCLIENT_LISTWIDGETINTERACTIONTEMPLATEITEM_H

#include <QWidget>

#include "../../../models/ObjectScenarioModel.h"
#include "../../../models/FeatureModel.h"


QT_BEGIN_NAMESPACE
namespace Ui { class ListWidgetInteractionTemplateItem; }
QT_END_NAMESPACE

class ListWidgetInteractionTemplateItem : public QWidget {
Q_OBJECT

public:
    explicit ListWidgetInteractionTemplateItem(QWidget *parent = nullptr);

    void setData(const QString &descriptioin, const QString &name, QList<ObjectScenarioModel*> objectScenarioModels, QList<FeatureModel*> featureModels);

    void setCurrentObject(ObjectScenarioModel *currentObject);

    ~ListWidgetInteractionTemplateItem() override;

    void clear();

    QJsonObject getObject();
    QString getName();

signals:
    void changeItem();

private slots:
    void changedIndexObject(int index);

private:
    Ui::ListWidgetInteractionTemplateItem *ui;
};


#endif //SCENARIOCLIENT_LISTWIDGETINTERACTIONTEMPLATEITEM_H
