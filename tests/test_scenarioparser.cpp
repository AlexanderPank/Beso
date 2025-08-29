#include "../db_service/ScenarioParser.h"
#include "../models/ObjectScenarioModel.h"
#include "../models/AlgorithmModel.h"
#include "../models/FileModel.h"
#include "../models/Feature.h"
#include "../models/InteractionModel.h"
#include "../models/SimulationParametersModel.h"
#include "test_scenarioparser.h"
#include <qt5/QtTest/QTest>
#include <qt5/QtWidgets/QTreeWidgetItem>
#include <qt5/QtCore/QString>
#include <qt5/QtCore/QObject>
#include <qt5/QtTest/QSignalSpy>
#include <qt5/QtCore/QArgument>
#include <qt5/QtCore/QJsonObject>
#include <qt5/QtCore/QJsonArray>
#include <qt5/QtCore/QFile>
#include <qt5/QtCore/QIODevice>
#include <qt5/QtCore/QDir>
#include <QTreeWidgetItem>

QJsonDocument ScenarioParserTest::loadJsonFromFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file:" << filePath;
        return QJsonDocument();  // пустой документ
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << parseError.errorString();
        return QJsonDocument();  // ошибка парсинга
    }

    return doc;
}


bool ScenarioParserTest::saveJsonToFile(const QJsonObject& json, const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning("Couldn't open file for writing: %s", qPrintable(file.errorString()));
        return false;
    }

    QJsonDocument doc(json);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}



void ScenarioParserTest::initTestCase() {

//    // Создание временного JSON-файла сценария
//    QJsonObject obj;
//    obj["name"] = "ObjectA";
//
//    QJsonObject alg;
//    alg["name"] = "Algorithm1";
//
//    QJsonObject file;
//    file["filename"] = "file.txt";
//
//    QJsonObject root;
//    QJsonArray objectsArray;
//    QJsonArray algorithmsArray;
//    QJsonArray filesArray;
//    objectsArray.append(obj);
//    algorithmsArray.append(alg);
//    filesArray.append(file);
//    root["objects"] = objectsArray;
//    root["algorithms"] = algorithmsArray;
//    root["files"] = filesArray;
//
//    QJsonDocument doc(root);
//
//    testJsonPath = QDir::temp().filePath("test_scenario.json");
//    QFile f(testJsonPath);
//    f.open(QIODevice::WriteOnly);
//    f.write(doc.toJson());
//    f.close();
}

#include <QDir>

void ScenarioParserTest::cleanupTestCase() {
    QDir tempDir("../temp");
    if (!tempDir.exists()) return;

    QStringList files = tempDir.entryList(QDir::Files);
    for (const QString& fileName : files) {
        tempDir.remove(fileName);
    }
}


void ScenarioParserTest::testAlgorithSection() {
    AlgorithmModel *iter = nullptr;
    auto test_data = [this, &iter](const QString& fileName) {
        auto doc = loadJsonFromFile(fileName).object();
        iter = AlgorithmModel::fromJson(doc, nullptr);
        QVERIFY(iter->getTitle() == QString("алгоритма расчета точки встречи"));
        // qDebug() << m->getInputParameters().size();
        QVERIFY(iter->getInputParameters().size() == 10);
        QVERIFY(iter->getOutputParameters().size() == 4);

        QVERIFY(iter->getOutputParameters()[0]->name() == "_lat");
        QVERIFY(iter->getOutputParameters()[0]->value() == "float");
    };
    test_data("../test_data/algorithm_test.json");
    QVERIFY(saveJsonToFile(iter->toJson(), "../temp/algorithm_test.json"));
    iter->deleteLater();
    test_data("../temp/algorithm_test.json");
}


void ScenarioParserTest::testObjectSection() {

    ObjectScenarioModel *iter = nullptr;
    auto test_data = [this, &iter](const QString& fileName) {
        auto doc = loadJsonFromFile(fileName).object();

        iter = ObjectScenarioModel::fromJson(doc, nullptr);
        QVERIFY(iter->getTitle() == QString("Корабль"));
        QVERIFY(iter->getId() == QString("RABBIT_00"));
        auto prop = iter->getProperty("state");
        QVERIFY(prop != nullptr);
        QVERIFY(prop->value() == "moving");
        QVERIFY(prop->type() == "str");
        QVERIFY(iter->actions().size() == 1);
        QVERIFY(iter->actions()[0]->name() == "движение");
        QVERIFY(iter->actions()[0]->outputMapping().size() == 4);

        prop = iter->getProperty("geo_path");
        Feature *feature = qvariant_cast<Feature *>(prop->value());
        QVERIFY(feature != nullptr);
        QVERIFY(feature->getCoordinates().size() == 6);

    };
    test_data("../test_data/object_test.json");
    QVERIFY(saveJsonToFile(iter->toJson(), "../temp/object_test.json" ));
    iter->deleteLater();
    test_data("../temp/object_test.json");
}

void ScenarioParserTest::testInteractionSection() {
    InteractionModel *iter = nullptr;
    auto test_data = [this, &iter](const QString& fileName) {
        auto doc = loadJsonFromFile(fileName).object();

        iter = InteractionModel::fromJson(doc, nullptr);
        QVERIFY(iter->getInteractionType() == "detection");
        QVERIFY(iter->getSourceObjectId() == "Model_00");
        QVERIFY(iter->getTriggerCondition().size() > 10);
        QVERIFY(iter->getResponseSetValues().size() > 10);
        QVERIFY(iter->getResponseAction().size() > 10);
    };
    test_data("../test_data/interactions.json");
    QVERIFY(saveJsonToFile(iter->toJson(), "../temp/interactions.json" ));
    iter->deleteLater();
    test_data("../temp/interactions.json");
}

void ScenarioParserTest::testFileSection() {
    FileModel *iter = nullptr;
    auto test_data = [this, &iter](const QString& fileName) {
        auto doc = loadJsonFromFile(fileName).object();
        iter = FileModel::fromJson(doc, nullptr);
        QVERIFY(iter->getId() == "LIBCPP01");
        QVERIFY(iter->getFileName() == "fox-rabbit.so");
        QVERIFY(iter->getType() == "libcpp");
        QVERIFY(iter->getMd5Hash() == "698d51a19d8a121ce581499d7b701668");
    };

    test_data("../test_data/file.json");
    QVERIFY(saveJsonToFile(iter->toJson(), "../temp/file.json" ));
    iter->deleteLater();
    test_data("../temp/file.json");
}

void ScenarioParserTest::testSimulationParametersSection(){
    SimulationParametersModel *iter= nullptr;
    auto test_data = [this, &iter](const QString& fileName) {
        auto doc = loadJsonFromFile(fileName).object();
        iter = SimulationParametersModel::fromJson(doc, nullptr);

        QVERIFY(iter->getTimeStep() == 1);
        QVERIFY(iter->getEndCondition() == "Model_00.properties['is_caught'] == -1");
        QVERIFY(iter->getMaxIterations() == 100000);
        QVERIFY(iter->getMapCenter() == "43.49387,39.59992");
        QVERIFY(iter->getMapZoom() == 6);

    };

    test_data("../test_data/simulation_parameters.json");
    QVERIFY(saveJsonToFile(iter->toJson(), "../temp/simulation_parameters.json" ));
    iter->deleteLater();
    test_data("../temp/simulation_parameters.json");
}

void ScenarioParserTest::testLoadScenario() {
    ScenarioParser parser;
    QVERIFY(parser.loadScenario("../test_data/scenario_test.json"));
    parser.saveScenario("../temp/scenario_test.json");
    parser.loadScenario("../temp/scenario_test.json");
}


//QTEST_APPLESS_MAIN(ScenarioParserTest)
//#include "tst_scenarioparser.moc"
