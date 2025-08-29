#ifndef DADDSCENARIOOBJECT_H
#define DADDSCENARIOOBJECT_H

#include <QDialog>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QListWidgetItem>
#include <QJsonDocument>
#include <QJsonObject>

#include "../../models/ObjectScenarioModel.h"

#define PATH_TO_DB_OBJECTS "/home/qq/ScenarioClient/data/BD"

namespace Ui {
class DAddScenarioObject;
}

class DAddScenarioObject : public QDialog
{
    Q_OBJECT

public:
    explicit DAddScenarioObject(QWidget *parent = nullptr);
    ~DAddScenarioObject() override;

    /**
     * Получение объекта моделировани JSON
     * @return
     */
    QJsonObject getJsonObjectScenarioModel();

private slots:
    void selectSimulationModelObject(QTreeWidgetItem *item, int);

private:
    Ui::DAddScenarioObject *ui;

    //Выбранный объект моделирования
    ObjectScenarioModel *objectScenarioModel;
    QJsonObject jsonObjectScenarioModel;

    QTreeWidgetItem* rootItem;

    /**
     * Загрузка объектов
     */
    void loadData();
};

#endif // DADDSCENARIOOBJECT_H
