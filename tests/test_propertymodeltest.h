#ifndef SCENARIOCLIENTTESTS_TEST_PROPERTYMODELTEST_H
#define SCENARIOCLIENTTESTS_TEST_PROPERTYMODELTEST_H

#include <qt5/QtCore/QObject>

class PropertyModelTest : public QObject
{
    Q_OBJECT

private slots:
    // Тестовые функции
    static void testJsonSerialization();
    static  void testJsonDeserialization();
    static  void testJsonRoundTrip();
    static  void testJsonValueTypes();
    static  void testTreeWidgetItemCreation();
    static  void testValueChangeSignals();
};

#endif //SCENARIOCLIENTTESTS_TEST_PROPERTYMODELTEST_H
