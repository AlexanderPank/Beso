//
// Created by qq on 07.04.25.
//
#ifndef SIMULATIONPARAMETERSMODEL_H
#define SIMULATIONPARAMETERSMODEL_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QTreeWidgetItem>

class SimulationParametersModel : public QObject
{
Q_OBJECT

public:
    explicit SimulationParametersModel(QObject *parent = nullptr);

    int getTimeStep() const;
    QString getEndCondition() const;
    int getMaxIterations() const;
    QString getMapCenter() const;
    int getMapZoom() const;

    void setTimeStep(int value);
    void setEndCondition(const QString &value);
    void setMaxIterations(int value);
    void setMapCenter(const QString &value);
    void setMapZoom(int value);

    QJsonObject toJson() const;
    static SimulationParametersModel* fromJson(const QJsonObject &json,QObject* parent);

    QTreeWidgetItem* getTreeWidgetItem(QTreeWidgetItem* parent = nullptr);
    QTreeWidgetItem* getTreeWidgetItem(QTreeWidget* parent = nullptr);

    QPointF getMapCenterDeg(bool &error) const;

    void filterTreeItem(const bool &isHidden);

private:
    int m_timeStep;
    QString m_endCondition;
    int m_maxIterations;
    QString m_mapCenter;
    int m_mapZoom;

    QTreeWidgetItem *treeItem;

    QTreeWidgetItem* createTreeWidgetItem(QTreeWidgetItem* item) const;
};

#endif // SIMULATIONPARAMETERSMODEL_H
