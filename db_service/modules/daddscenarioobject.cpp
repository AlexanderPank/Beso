#include "daddscenarioobject.h"
#include "ui_daddscenarioobject.h"

#include "../services/DataStorageServiceFactory.h"

DAddScenarioObject::DAddScenarioObject(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DAddScenarioObject)
{
    ui->setupUi(this);

    this->setWindowTitle("Выбор объекта моделирования");

    ui->trwScenarioObject->setColumnCount(2);
    ui->trwScenarioObject->setHeaderLabels({"Название", "Значение"});
    ui->trwScenarioObject->setFont(QFont("Arial", 10));
    ui->trwScenarioObject->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->trwScenarioObject->setColumnWidth(0, 300);

    connect(ui->wObjects, &ModelObjectsForm::itemClicked, this, &DAddScenarioObject::selectSimulationModelObject);

    connect(ui->btnAddObject, &QPushButton::clicked, this, &DAddScenarioObject::accept);
    connect(ui->btnCancel, &QPushButton::clicked, this, &DAddScenarioObject::reject);

    connect(ui->btnUpdate, &QPushButton::clicked, [this](){
        ui->wObjects->loadData();
    });

    ui->wObjects->loadData();
    ui->wObjects->onlyObjects();
}

DAddScenarioObject::~DAddScenarioObject()
{
    delete ui;
}

QJsonObject DAddScenarioObject::getJsonObjectScenarioModel() {
    return jsonObjectScenarioModel;
}

void DAddScenarioObject::selectSimulationModelObject(QTreeWidgetItem *item, int) {
    ui->trwScenarioObject->clear();

    objectScenarioModel = item->data(0, Qt::UserRole).value<ObjectScenarioModel *>();
    ui->btnAddObject->setEnabled(objectScenarioModel!= nullptr);

    if (objectScenarioModel==nullptr) return;

    jsonObjectScenarioModel = objectScenarioModel->toJson();

    ObjectScenarioModel* selectObjectModel = ObjectScenarioModel::fromJson(jsonObjectScenarioModel,this);

    ui->trwScenarioObject->addTopLevelItem(selectObjectModel->getTreeWidgetItem(ui->trwScenarioObject->invisibleRootItem()));
    ui->trwScenarioObject->expandAll();
}