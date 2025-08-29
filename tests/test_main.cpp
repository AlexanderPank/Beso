#include <QCoreApplication>
#include <QtTest>

#include "test_scenarioparser.h"
#include "test_propertymodeltest.h"
#include "../models/Feature.h"

int main(int argc, char** argv) {
    qRegisterMetaType<Feature*>("Feature*");

    QCoreApplication app(argc, argv);
    int status = 0;
    status |= QTest::qExec(new ScenarioParserTest, argc, argv);
    //status |= QTest::qExec(new PropertyModelTest, argc, argv);
    return status;
}

