#include <QtTest>
#include <qt5/QtWidgets/QTreeWidgetItem>
#include <qt5/QtCore/QString>

#include <QSignalSpy>
#include "../models/PropertyModel.h"
#include "test_propertymodeltest.h"


void PropertyModelTest::testJsonSerialization()
{
    // Тест сериализации в JSON
    PropertyModel model("testName", "int", QVariant(42));
    QJsonObject json = model.toJson();
    qDebug() << json;
    QVERIFY(json.contains("name"));
    QVERIFY(json.contains("value"));
    QVERIFY(json.contains("getType"));

    QCOMPARE(json["name"].toString(), QString("testName"));
    QCOMPARE(json["value"].toInt(), 42);
    QCOMPARE(json["getType"].toString(), QString("int"));
}
//
void PropertyModelTest::testJsonDeserialization()
{
    // Тест десериализации из JSON
    QJsonObject json;
    json["name"] = "testName";
    json["value"] = 3.14;
    json["getType"] = "float";

    PropertyModel* model = PropertyModel::fromJson(json, nullptr);

    QVERIFY(model != nullptr);
    QCOMPARE(model->name(), QString("testName"));
    QCOMPARE(model->value().toDouble(), 3.14);
    QCOMPARE(model->type(), QString("float"));

    delete model;
}

void PropertyModelTest::testJsonRoundTrip()
{
    // Тест "туда-обратно" - сериализация с последующей десериализацией
    PropertyModel original("roundTrip", "array", QVariantList{1, 2, "three"});
    QJsonObject json = original.toJson();

    PropertyModel* restored = PropertyModel::fromJson(json, nullptr);

    QVERIFY(restored != nullptr);
    QCOMPARE(restored->name(), original.name());
    QCOMPARE(restored->value(), original.value());
    QCOMPARE(restored->type(), original.type());

    delete restored;
}

void PropertyModelTest::testJsonValueTypes()
{
    // Тест определения типов JSON значений
    QCOMPARE(PropertyModel::getJsonValueType(QJsonValue(true)), QString("bool"));
    QCOMPARE(PropertyModel::getJsonValueType(QJsonValue(42)), QString("int"));
    QCOMPARE(PropertyModel::getJsonValueType(QJsonValue(3.14)), QString("float"));
    QCOMPARE(PropertyModel::getJsonValueType(QJsonValue("string")), QString("string"));
    QCOMPARE(PropertyModel::getJsonValueType(QJsonValue(QJsonArray())), QString("array"));
    QCOMPARE(PropertyModel::getJsonValueType(QJsonValue(QJsonObject())), QString("object"));
}

void PropertyModelTest::testTreeWidgetItemCreation()
{
    PropertyModel model("treeItem", "str", QVariant("testValue"));
    QTreeWidgetItem* item = model.getTreeWidgetItem(nullptr);

    QVERIFY(item != nullptr);
    QCOMPARE(item->text(0), QString("treeItem"));
    QCOMPARE(item->text(1), QString("testValue"));

    delete item;
}

void PropertyModelTest::testValueChangeSignals()
{
    PropertyModel model("signalTest", "int", QVariant(0));


    QSignalSpy typeSpy(&model, &PropertyModel::typeChanged);
    QSignalSpy valueSpy(&model, SIGNAL(valueChanged(QVariant)));

    model.setValue("newValue");

    QCOMPARE(valueSpy.count(), 1);
    QCOMPARE(typeSpy.count(), 1);

    // Проверка, что сигналы не отправляются при установке того же значения
    model.setValue("newValue");
    QCOMPARE(valueSpy.count(), 1);
    QCOMPARE(typeSpy.count(), 1);
}

//QTEST_APPLESS_MAIN(PropertyModelTest)
//#include "tst_propertymodeltest.moc"