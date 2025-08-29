//
// Created by qq on 07.04.25.
//

#include "SimulationParametersModel.h"

SimulationParametersModel::SimulationParametersModel(QObject *parent)
        : QObject(parent),
          m_timeStep(1),
          m_maxIterations(0),
          m_mapZoom(0)
{}

int SimulationParametersModel::getTimeStep() const { return m_timeStep; }
QString SimulationParametersModel::getEndCondition() const { return m_endCondition; }
int SimulationParametersModel::getMaxIterations() const { return m_maxIterations; }
QString SimulationParametersModel::getMapCenter() const { return m_mapCenter; }
int SimulationParametersModel::getMapZoom() const { return m_mapZoom; }

void SimulationParametersModel::setTimeStep(int value) { m_timeStep = value; }
void SimulationParametersModel::setEndCondition(const QString &value) { m_endCondition = value; }
void SimulationParametersModel::setMaxIterations(int value) { m_maxIterations = value; }
void SimulationParametersModel::setMapCenter(const QString &value) { m_mapCenter = value; }
void SimulationParametersModel::setMapZoom(int value) { m_mapZoom = value; }

QJsonObject SimulationParametersModel::toJson() const
{
    QJsonObject json;
    json["time_step"] = m_timeStep;
    json["end_condition"] = m_endCondition;
    json["max_iterations"] = m_maxIterations;
    json["map_center"] = m_mapCenter;
    json["map_zoom"] = m_mapZoom;
    return json;
}

SimulationParametersModel* SimulationParametersModel::fromJson(const QJsonObject &json, QObject* parent)
{
    auto *model = new SimulationParametersModel(parent);
    model->setTimeStep(json["time_step"].toInt());
    model->setEndCondition(json["end_condition"].toString());
    model->setMaxIterations(json["max_iterations"].toInt());
    model->setMapCenter(json["map_center"].toString());
    model->setMapZoom(json["map_zoom"].toInt());
    return model;
}

QTreeWidgetItem* SimulationParametersModel::getTreeWidgetItem(QTreeWidget* parent)
{
    treeItem = new QTreeWidgetItem(parent);
    return createTreeWidgetItem(treeItem);
}

QTreeWidgetItem* SimulationParametersModel::getTreeWidgetItem(QTreeWidgetItem* parent)
{
    treeItem = new QTreeWidgetItem(parent);
    return createTreeWidgetItem(treeItem);
}

QTreeWidgetItem* SimulationParametersModel::createTreeWidgetItem(QTreeWidgetItem* item) const{
    item->setText(0, "Параметры симуляции");

    auto addChild = [item](const QString &key, const QString &value) {
        auto *child = new QTreeWidgetItem(item);
        child->setText(0, key);
        child->setText(1, value);
    };

    addChild("time_step", QString::number(m_timeStep));
    addChild("end_condition", m_endCondition);
    addChild("max_iterations", QString::number(m_maxIterations));
    addChild("map_center", m_mapCenter);
    addChild("map_zoom", QString::number(m_mapZoom));

    return item;
}

QPointF SimulationParametersModel::getMapCenterDeg(bool &error) const {
    QStringList coordinates = m_mapCenter.split(",");
    if (coordinates.size() != 2) {error = true; return  QPointF(0,0);}

    bool okLon, okLat;
    float lat = coordinates[0].toFloat(&okLon);
    float lon = coordinates[1].toFloat(&okLat);
    if (!okLon || !okLat) {error = true; return  QPointF(0,0);}
    return QPointF(lat, lon);
}

void SimulationParametersModel::filterTreeItem(const bool &isHidden) {
    treeItem->setHidden(isHidden);
}
