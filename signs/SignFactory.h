// sign_factory.h
#ifndef SIGN_FACTORY_H
#define SIGN_FACTORY_H

#include "SignBase.h"
#include "../models/ObjectScenarioModel.h"
#include "../models/FeatureModel.h"
#include <QList>
#include <QPointF>
#include <QString>
#include <QMap>
#include <functional>



class SignFactory
{
public:
    static SignBase* createSign(const QString& classcode,
                                const QString& name,
                                bool is_own,
                                QList<QPointF>& coord,
                                double course,
                                ObjectScenarioModel* properties
                                );
    static SignBase* createSignFromFeature(FeatureModel* feature);
    static SignBase* updateConnectedSign(SignBase* sign, ObjectScenarioModel *obj);
};

#endif // SIGN_FACTORY_H