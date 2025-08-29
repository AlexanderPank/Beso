#ifndef SCENARIOCLIENT_SCENARIOTACTICALOBJECTS_H
#define SCENARIOCLIENT_SCENARIOTACTICALOBJECTS_H

#include <QWidget>

#include "../models/TacticalSignModel.h"
#include "services/DataStorageServiceFactory.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ScenarioTacticalObjects; }
QT_END_NAMESPACE

class ScenarioTacticalObjects : public QWidget {
Q_OBJECT

public:
    explicit ScenarioTacticalObjects(QWidget *parent = nullptr);

    ~ScenarioTacticalObjects() override;

signals:
    void addTacticalSign(QString, QJsonObject);

public slots:
    void selectTacticalSign(QString type = "", QString subType = "");
    void onBtnAddTacticalSign();

private:
    Ui::ScenarioTacticalObjects *ui;

    QButtonGroup* buttonGroup = nullptr;

    QList<TacticalSignModel*> m_tacticalSignModels;
};


#endif //SCENARIOCLIENT_SCENARIOTACTICALOBJECTS_H
