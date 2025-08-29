#ifndef SCENARIOCLIENTTESTS_TEST_SCENARIOPARSER_H
#define SCENARIOCLIENTTESTS_TEST_SCENARIOPARSER_H

#include <QObject>
#include <QJsonDocument>

class ScenarioParserTest : public QObject {

    Q_OBJECT

private slots:

    void initTestCase();
    void cleanupTestCase();

    void testAlgorithSection();
    void testObjectSection();
    void testInteractionSection();
    void testFileSection();
    void testSimulationParametersSection();

    void testLoadScenario();

    // сервисные функции
    QJsonDocument loadJsonFromFile(const QString& filePath);
    bool saveJsonToFile(const QJsonObject& json, const QString& filePath);

private:
    QString testJsonPath;
};
#endif //SCENARIOCLIENTTESTS_TEST_SCENARIOPARSER_H
