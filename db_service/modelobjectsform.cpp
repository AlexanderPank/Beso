#include "modelobjectsform.h"
#include "ui_modelobjectsform.h"

ModelObjectsForm::ModelObjectsForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModelObjectsForm)
{
    ui->setupUi(this);
    ui->toolBox->removeItem(1);
    ui->toolBox->removeItem(0);

    createTree(treeObjects);
    createTree(treeAlgorithms);
    createTree(treeLibFiles);
    createTree(treeTacticalSign);
    createTree(treeIterations);

    //меняем на табы
    ui->toolBox->addItem(treeObjects,"Объекты");
    ui->toolBox->addItem(treeAlgorithms,"Алгоритмы");
    ui->toolBox->addItem(treeLibFiles,"Библиотеки");
    ui->toolBox->addItem(treeTacticalSign,"Знаки");
//    ui->toolBox->addItem(treeIterations,"Взаимодействия");

    // Добавляем сигнал слот на отображение данных в таблице
    connect(treeObjects,SIGNAL(itemClicked(QTreeWidgetItem *, int)),this,SLOT(m_itemClicked(QTreeWidgetItem *, int)));
    connect(treeAlgorithms,SIGNAL(itemClicked(QTreeWidgetItem *, int)),this,SLOT(m_itemClicked(QTreeWidgetItem *, int)));
    connect(treeLibFiles,SIGNAL(itemClicked(QTreeWidgetItem *, int)),this,SLOT(m_itemClicked(QTreeWidgetItem *, int)));
    connect(treeTacticalSign,SIGNAL(itemClicked(QTreeWidgetItem *, int)),this,SLOT(m_itemClicked(QTreeWidgetItem *, int)));
    connect(treeIterations,SIGNAL(itemClicked(QTreeWidgetItem *, int)),this,SLOT(m_itemClicked(QTreeWidgetItem *, int)));

    connect(ui->toolBox,&QToolBox::currentChanged,this,[=]( int page){
        switch (page) {
        case 0:
            if(treeObjects->currentItem()!=nullptr)
                m_itemClicked(treeObjects->currentItem(),page);
            else
                m_itemClicked(nullptr,-1);
            break;
        case 1:
            if(treeAlgorithms->currentItem()!=nullptr)
                m_itemClicked(treeAlgorithms->currentItem(),page);
            else
                m_itemClicked(nullptr,-1);
            break;
        case 2:
            if(treeLibFiles->currentItem()!=nullptr)
                m_itemClicked(treeLibFiles->currentItem(),page);
            else
                m_itemClicked(nullptr,-1);
            break;
        case 3:
            if(treeTacticalSign->currentItem()!=nullptr)
                m_itemClicked(treeTacticalSign->currentItem(),page);
            else
                m_itemClicked(nullptr,-1);
            break;
        default:
            break;
        }
    });
}

ModelObjectsForm::~ModelObjectsForm()
{
    delete ui;
}

void ModelObjectsForm::loadData()
{
    DataStorageServiceFactory::getInstance()->loadData();

    listObjects = DataStorageServiceFactory::getInstance()->getElements(TypeElement::teOBJECT);
    QList<DataStorageItem *> _listAlgorithms = DataStorageServiceFactory::getInstance()->getElements(TypeElement::teALGORITHM);
    QStringList _listik={};
    for (auto var : _listAlgorithms) {
        _listik.append(var->getId());
    }
    _listik.sort();
    listAlgorithms.clear();
    for (auto var : _listik) {
        for (auto var2 : _listAlgorithms) {
            if (var == var2->getId()){
                listAlgorithms.append(var2);
                _listAlgorithms.removeOne(var2);
                break;
            }
        }
    }
    listLibFiles = DataStorageServiceFactory::getInstance()->getElements(TypeElement::teLIBFILE);
    QList<DataStorageItem *> _listTacticalSign  = DataStorageServiceFactory::getInstance()->getElements(TypeElement::teTACTICAL_SIGN);
    QStringList listik = {};
    listik.clear();
    for (auto var : _listTacticalSign) {
        listik.append(var->getTitle());
    }
    listik.sort();
    listTacticalSign.clear();
    for (auto var : listik) {
        for (auto var2 : _listTacticalSign) {
            if (var == var2->getTitle()){
                listTacticalSign.append(var2);
                _listTacticalSign.removeOne(var2);
                break;
            }
        }
    }
    listIterations = DataStorageServiceFactory::getInstance()->getElements(TypeElement::teINTERACTION);

    subParents.clear();

    treeObjects->clear();
    treeAlgorithms->clear();
    treeTacticalSign->clear();
    treeLibFiles->clear();
    treeIterations->clear();

    //Грузим шаблоны
    templateListObjects = DataStorageServiceFactory::getInstance()->getElements(TypeElement::teOBJECT_TEMPLATE);
    templateListAlgorithms = DataStorageServiceFactory::getInstance()->getElements(TypeElement::teALGORITHM_TEMPLATE);
    templateListLibFiles = DataStorageServiceFactory::getInstance()->getElements(TypeElement::teLIBFILE_TEMPLATE);
    templateListTacticalSign = DataStorageServiceFactory::getInstance()->getElements(TypeElement::teTACTICAL_SIGN_TEMPLATE);
    templateListIterations = DataStorageServiceFactory::getInstance()->getElements(TypeElement::teINTERACTION_TEMPLATE);

    QTreeWidgetItem *ItemObjects = treeObjects->invisibleRootItem();// new QTreeWidgetItem(treeObjects->invisibleRootItem(),{"Объекты"});
    ItemObjects->setData(0,Qt::UserRole,TypeElement::teOBJECT);
    QTreeWidgetItem *ItemAlgorithms = treeAlgorithms->invisibleRootItem();// new QTreeWidgetItem(treeAlgorithms->invisibleRootItem(),{"Алгоритмы"});
    ItemAlgorithms->setData(0,Qt::UserRole,TypeElement::teALGORITHM);
    QTreeWidgetItem *ItemLibFiles = treeLibFiles->invisibleRootItem();// new QTreeWidgetItem(treeLibFiles->invisibleRootItem(),{"Библиотеки"});
    ItemLibFiles->setData(0,Qt::UserRole,TypeElement::teLIBFILE);
    QTreeWidgetItem *ItemTacticalSign = treeTacticalSign->invisibleRootItem();// new QTreeWidgetItem(treeLibFiles->invisibleRootItem(),{"Библиотеки"});
    ItemTacticalSign->setData(0,Qt::UserRole,TypeElement::teTACTICAL_SIGN);
    QTreeWidgetItem *ItemIteration = treeIterations->invisibleRootItem();// new QTreeWidgetItem(treeLibFiles->invisibleRootItem(),{"Библиотеки"});
    ItemIteration->setData(0,Qt::UserRole,TypeElement::teINTERACTION);

    fullTypeParents.clear();

    QStringList ids;
    QStringList types;
    QStringList subTypes;
    for (auto var : listObjects) {

        ObjectScenarioModel *objModel = ObjectScenarioModel::fromJson(var->getData(), nullptr);
        QTreeWidgetItem *item = objModel->getTreeWidgetItem(ItemObjects,false);
        item->setData(1,Qt::UserRole,var->getData());

        QString subType = objModel->getPropertyString("subType");
        QString type = objModel->getPropertyString("type");

        createTreeItem(type,subType,item,TypeElement::teOBJECT,ItemObjects);
        ids.append(objModel->getId());
        if (!types.contains(type))
            types.append(type);
        if (!subTypes.contains(subType))
            subTypes.append(subType);
    }

    QPair<int,QStringList> ids_pair(3,ids);
    QPair<int,QStringList> type_pair(1,types);
    QPair<int,QStringList> subTypes_pair(2,subTypes);
    QList<QPair<int,QStringList>> list = {ids_pair,type_pair,subTypes_pair};

    mapOfTypesAndIds.insert(teOBJECT,list);

    ids.clear();
    types.clear();
    subTypes.clear();
    list.clear();

    for (auto var : listAlgorithms) {

        AlgorithmModel *algModel = AlgorithmModel::fromJson(var->getData(), nullptr);
        //        QTreeWidgetItem *item = new QTreeWidgetItem({algModel->getTitle(),algModel->getId()});
        QTreeWidgetItem *item = algModel->getTreeWidgetItem(ItemAlgorithms,false);
        //        item->setData(0,Qt::UserRole,algModel);
        item->setData(1,Qt::UserRole,var->getData());
        //Работаем с родителем
        QString subType = algModel->getMSubType();
        QString type = algModel->getType();
        if (type=="libcpp")
            type = "Библиотеки C++";
        if (type=="inner")
            type = "Cистемные алгоритмы";
        createTreeItem(type,subType,item,TypeElement::teALGORITHM,ItemAlgorithms);
        ids.append(algModel->getId());
        if (!types.contains(type))
            types.append(type);
        if (!subTypes.contains(subType))
            subTypes.append(subType);
    }

    ids_pair = QPair<int,QStringList> (3,ids);
    type_pair = QPair<int,QStringList> (1,types);
    subTypes_pair = QPair<int,QStringList> (2,subTypes);
    list = {ids_pair,type_pair,subTypes_pair};

    mapOfTypesAndIds.insert(teALGORITHM,list);

    ids.clear();
    types.clear();
    subTypes.clear();

    for (auto var : listLibFiles) {

        FileModel *fileModel = FileModel::fromJson(var->getData(), nullptr);
        QTreeWidgetItem *item = new QTreeWidgetItem({fileModel->getFileName(),fileModel->getId()});
        item->setData(0, Qt::UserRole, QVariant::fromValue(const_cast<FileModel*>(fileModel)));
        item->setData(1,Qt::UserRole,var->getData());
        QString type = fileModel->getType();
        if (type=="libcpp")
            type = "Библиотеки C++ ";
        if (type=="inner")
            type = "Cистемные алгоритмы ";
        //        ItemLibFiles->addChild(item);
        createTreeItem(type,nullptr,item,TypeElement::teLIBFILE,ItemLibFiles);
        ids.append(fileModel->getId());
        if (!types.contains(type))
            types.append(type);
    }

    ids_pair = QPair<int,QStringList> (3,ids);
    type_pair = QPair<int,QStringList> (1,types);
    subTypes_pair = QPair<int,QStringList> (2,subTypes);
    list = {ids_pair,type_pair,subTypes_pair};

    mapOfTypesAndIds.insert(teLIBFILE,list);

    ids.clear();
    types.clear();
    subTypes.clear();

    for (auto var : listTacticalSign) {
        TacticalSignModel *signModel = TacticalSignModel::fromJson(var->getData(), nullptr);
        QTreeWidgetItem *item = new QTreeWidgetItem({signModel->getTitle(),signModel->getId()});

        item->setData(0, Qt::UserRole, QVariant::fromValue(const_cast<TacticalSignModel*>(signModel)));
        item->setData(1,Qt::UserRole,var->getData());
        QString type = signModel->getGroup();
        createTreeItem(type,nullptr,item,TypeElement::teTACTICAL_SIGN,ItemTacticalSign);

        ids.append(signModel->getId());
        if (!types.contains(type))
            types.append(type);
    }

    ids_pair = QPair<int,QStringList> (3,ids);
    type_pair = QPair<int,QStringList> (1,types);
    subTypes_pair = QPair<int,QStringList> (2,subTypes);
    list = {ids_pair,type_pair,subTypes_pair};

    mapOfTypesAndIds.insert(teTACTICAL_SIGN,list);

    ids.clear();
    types.clear();
    subTypes.clear();

    for (auto var : listIterations) {
        InteractionModelTemplate *iterModel = InteractionModelTemplate::fromJson(var->getData(), nullptr);
        QTreeWidgetItem *item = new QTreeWidgetItem({iterModel->getTitle(),iterModel->getId()});

        item->setData(0, Qt::UserRole, QVariant::fromValue(const_cast<InteractionModelTemplate*>(iterModel)));
        item->setData(1,Qt::UserRole,var->getData());
        createTreeItem("Взаимодействия",nullptr,item,TypeElement::teINTERACTION,ItemIteration);

        ids.append(iterModel->getId());
        if (!types.contains("Взаимодействия"))
            types.append("Взаимодействия");
    }
    ids_pair = QPair<int,QStringList> (3,ids);
    type_pair = QPair<int,QStringList> (1,types);
    subTypes_pair = QPair<int,QStringList> (2,subTypes);
    list = {ids_pair,type_pair,subTypes_pair};

    mapOfTypesAndIds.insert(teINTERACTION,list);

    treeObjects->expandAll();
    treeAlgorithms->expandAll();
    treeLibFiles->expandAll();
    treeTacticalSign->expandAll();
    treeIterations->expandAll();
}

QTreeWidgetItem *ModelObjectsForm::currentItem()
{
    QTreeWidgetItem *currItem = nullptr;
    switch (ui->toolBox->currentIndex()) {
    case 0:
        currItem = treeObjects->currentItem();
        break;
    case 1:
        currItem = treeAlgorithms->currentItem();
        break;
    case 2:
        currItem = treeLibFiles->currentItem();
        break;
    case 3:
        currItem = treeTacticalSign->currentItem();
        break;
    case 4:
        currItem = treeIterations->currentItem();
        break;
    default:
        break;
    }
    return currItem;
}

const QList<DataStorageItem *> &ModelObjectsForm::getListObjects() const
{
    return listObjects;
}

void ModelObjectsForm::setListObjects(const QList<DataStorageItem *> &newListObjects)
{
    listObjects = newListObjects;
}

const QList<DataStorageItem *> &ModelObjectsForm::getListAlgorithms() const
{
    return listAlgorithms;
}

void ModelObjectsForm::setListAlgorithms(const QList<DataStorageItem *> &newListAlgorithms)
{
    listAlgorithms = newListAlgorithms;
}

const QList<DataStorageItem *> &ModelObjectsForm::getListLibFiles() const
{
    return listLibFiles;
}

void ModelObjectsForm::setListLibFiles(const QList<DataStorageItem *> &newListLibFiles)
{
    listLibFiles = newListLibFiles;
}

TypeElement ModelObjectsForm::getcurrentTypeElement()
{
    return static_cast<TypeElement>(ui->toolBox->currentIndex());
}

void ModelObjectsForm::createTree(QTreeWidget *treeWidget)
{
    treeWidget->setColumnCount(2);
    treeWidget->setHeaderLabels({"Параметр", "Значение"});
    treeWidget->setFont(QFont("Arial", 13));
    treeWidget->setColumnWidth(0, 300);
}

void ModelObjectsForm::createTreeItem(QString subType, QString type, QTreeWidgetItem *item, TypeElement typeElement, QTreeWidgetItem *ItemObjects)
{
    if (subType!= nullptr && !subType.isEmpty()) {
        if (subParents.keys().contains(subType)){
            QMap<QString,QTreeWidgetItem*> typeParents = fullTypeParents.value(subType);
            ItemObjects->removeChild(item);
            auto itemSubParent = subParents.value(subType);
            if (type!= nullptr && !type.isEmpty()) {
                if (typeParents.keys().contains(type)){
                    auto itemTypeParent = typeParents.value(type);
                    itemTypeParent->addChild(item);
                }else{
                    auto itemTypeParent = new QTreeWidgetItem();
                    itemTypeParent->setText(0, type);
                    itemTypeParent->setData(0, Qt::UserRole, typeElement);
                    itemTypeParent->addChild(item);
                    itemSubParent->addChild(itemTypeParent);
                    typeParents.insert(type,itemTypeParent);
                    fullTypeParents.remove(subType);
                    fullTypeParents.insert(subType,typeParents);
                }
            }else
                itemSubParent->addChild(item);
        }else {
            ItemObjects->removeChild(item);
            auto itemSubParent = new QTreeWidgetItem();
            itemSubParent->setText(0, subType);
            itemSubParent->setData(0, Qt::UserRole, typeElement);
            if (type!= nullptr && !type.isEmpty()) {
                QMap<QString,QTreeWidgetItem*> typeParents = fullTypeParents.value(subType);
                if (typeParents.keys().contains(type)){
                    auto itemTypeParent = typeParents.value(type);
                    if (type!= nullptr && !type.isEmpty()) {
                        if (typeParents.keys().contains(type)){
                            auto itemTypeParent = typeParents.value(type);
                            itemTypeParent->addChild(item);
                        }else{
                            auto itemTypeParent = new QTreeWidgetItem();
                            itemTypeParent->setText(0, type);
                            itemTypeParent->setData(0, Qt::UserRole, typeElement);
                            itemTypeParent->addChild(item);
                            itemSubParent->addChild(itemTypeParent);
                            typeParents.insert(type,itemTypeParent);
                            fullTypeParents.remove(subType);
                            fullTypeParents.insert(subType,typeParents);
                        }
                    }else
                        itemTypeParent->addChild(item);
                }else{
                    auto itemTypeParent = new QTreeWidgetItem();
                    itemTypeParent->setText(0, type);
                    itemTypeParent->setData(0, Qt::UserRole, typeElement);
                    itemTypeParent->addChild(item);
                    itemSubParent->addChild(itemTypeParent);
                    typeParents.insert(type,itemTypeParent);
                    fullTypeParents.remove(subType);
                    fullTypeParents.insert(subType,typeParents);
                }
            }else
                itemSubParent->addChild(item);
            ItemObjects->addChild(itemSubParent);
            subParents.insert(subType,itemSubParent);

        }
    }else {
        ItemObjects->addChild(item);
    }
}

void ModelObjectsForm::onAddClicked(QString subType)
{

    QString text = "";
    QString id = "";
    QStringList ids = {};
    QString type = "";
    QStringList allTypes = {};
//    QString sub_type = "";
    QStringList allSubTypes = {};
    bool isFile = false;
    bool isSubType = false;
    bool isfunction = false;
    switch (getcurrentTypeElement()) {
    case teOBJECT:
        text = "Новый обьект";
        id = "Object";
        isFile = false;
        isSubType = true;
        break;
    case teALGORITHM:
        text = "Новый алгоритм";
        id = "ald_id_";
        isFile = false;
        isSubType = true;
        isfunction = true;
        break;
    case teTACTICAL_SIGN:
        text = "Новый знак";
        id = "Sign";
        isFile = false;
        break;
    case teLIBFILE:
        text = "Новая библиотека";
        id = "Lib";
        isFile = true;
        break;
    case teINTERACTION:
        text = "Новое взаимодействие";
        id = "Interaction";
        isFile = false;
        break;
    default:
        break;
    }
    for (auto var : mapOfTypesAndIds.value(getcurrentTypeElement())) {
        switch (var.first) {
        case 1:
            allTypes = var.second;
            break;
        case 2:
            allSubTypes = var.second;
            break;
        case 3:
            ids = var.second;
            break;
        default:
            break;
        }
    }
    addModelObjDialog = new AddModelObjectsDialog(text,id,allTypes,allSubTypes,ids,isFile,true,isSubType,subType,isfunction);
    connect(addModelObjDialog,&AddModelObjectsDialog::addNewModelObject,this,&ModelObjectsForm::addNewObject);
    addModelObjDialog->exec();
}

void ModelObjectsForm::onRemoveClicked()
{

}

void ModelObjectsForm::onCopyClicked()
{
    QTreeWidgetItem *currentItem = this->currentItem();
    if (!currentItem) return;
    QTreeWidgetItem *parent = currentItem->parent();
    if (!parent) return;
    QStringList ids = {};
    QString type = "";
    QStringList allTypes = {};
    QString sub_type = "";
    QStringList allSubTypes = {};
    bool isFile = false;

    switch (getcurrentTypeElement()) {
    case teOBJECT:
        isFile = false;
        break;
    case teALGORITHM:
        isFile = false;
        break;
    case teTACTICAL_SIGN:
        isFile = false;
        break;
    case teLIBFILE:
        isFile = true;
        break;
    case teINTERACTION:
        isFile = false;
        break;
    default:
        break;
    }
    for (auto var : mapOfTypesAndIds.value(getcurrentTypeElement())) {
        switch (var.first) {
        case 1:
            allTypes = var.second;
            break;
        case 2:
            allSubTypes = var.second;
            break;
        case 3:
            ids = var.second;
            break;
        default:
            break;
        }
    }
    addModelObjDialog = new AddModelObjectsDialog(currentItem->text(0),currentItem->text(1),allTypes,allSubTypes,ids,isFile,false,false);
    connect(addModelObjDialog,&AddModelObjectsDialog::addNewModelObject,this,&ModelObjectsForm::copyNewObject);
    addModelObjDialog->exec();
}

void ModelObjectsForm::m_itemClicked(QTreeWidgetItem * treeItem, int c)
{
    if (treeItem != nullptr){

    currentTitle = treeItem->text(0);
    currentId = treeItem->text(1);
    currentTab = ui->toolBox->currentIndex();
    }
    emit itemClicked(treeItem,ui->toolBox->currentIndex());
}

void ModelObjectsForm::addNewObject(QString text, QString id_text, QString type, QString sub_type, QString function)
{
    disconnect(addModelObjDialog,&AddModelObjectsDialog::addNewModelObject,this,&ModelObjectsForm::addNewObject);
    TypeElement te = (TypeElement)getcurrentTypeElement();

    switch (te) {
    case teOBJECT: {
        auto *newModel = new ObjectScenarioModel();
        if (templateListObjects.size()>0)
            newModel = newModel->fromJson(templateListObjects.at(0)->getData(),nullptr);
        newModel->setTitle(text);
        newModel->setId(id_text);
        newModel->setPropertyValue("subType",sub_type);
        newModel->setPropertyValue("type",type);
        QJsonObject jo = newModel->toJson();
        DataStorageServiceFactory::getInstance()->addElement(te, jo);
    }
        break;
    case teALGORITHM:{
        auto *newModel = new AlgorithmModel();
        if (templateListObjects.size()>0)
            newModel = newModel->fromJson(templateListAlgorithms.at(0)->getData(),nullptr);
        newModel->setTitle(text);
        newModel->setId(id_text);
        newModel->setMSubType(sub_type);
        if (type=="Cистемные алгоритмы"){
            newModel->setType("inner");
            newModel->setFunction("ModelTasks."+function);
        }
        else{
            if (type=="Библиотеки C++"){
                newModel->setType("libcpp");
                newModel->setFunction(function);
            }
            else{
                newModel->setType(type);
                newModel->setFunction(function);
            }
        }

        QJsonObject jo = newModel->toJson();
        DataStorageServiceFactory::getInstance()->addElement(te, jo);
    }
        break;
    case teLIBFILE:{
        auto *newModel = new FileModel();
        if (templateListObjects.size()>0)
            newModel = newModel->fromJson(templateListLibFiles.at(0)->getData(),nullptr);
        newModel->setFileName(text);
        newModel->setId(id_text);
        newModel->setType("libcpp");

        QJsonObject jo = newModel->toJson();
        DataStorageServiceFactory::getInstance()->addElement(te, jo);
    }
        break;
    case teTACTICAL_SIGN:{
        auto *newModel = new TacticalSignModel();
        if (templateListTacticalSign.size()>0)
            newModel = newModel->fromJson(templateListTacticalSign.at(0)->getData(),nullptr);
        newModel->setTitle(text);
        newModel->setId(id_text);

        QJsonObject jo = newModel->toJson();
        DataStorageServiceFactory::getInstance()->addElement(te, jo);

    }   break;
    case teINTERACTION:{
        auto *newModel = new InteractionModelTemplate();
        if (templateListIterations.size()>0)
            newModel = newModel->fromJson(templateListTacticalSign.at(0)->getData(),nullptr);
        newModel->setTitle(text);
        newModel->setId(id_text);

        QJsonObject jo = newModel->toJson();
        DataStorageServiceFactory::getInstance()->addElement(te, jo);

    }   break;
    default:
        break;
    }
    loadData();
    currentId = id_text;
    currentTab = ui->toolBox->currentIndex();
    currentTitle = text;
    updateSelection();

    addModelObjDialog= nullptr;
}

void ModelObjectsForm::copyNewObject(QString title, QString id, QString type, QString sub_type)
{

    QString text  = title;
    QTreeWidgetItem *parent = currentItem()->parent();
    QString id_text  = id;
    if (!id_text.isEmpty()){
        QTreeWidgetItem *newItem = currentItem()->clone();
        newItem->setText(0,text);
        newItem->setText(1,id_text);

        TypeElement te = (TypeElement)parent->data(0,Qt::UserRole).toInt();
        QJsonObject jo;
        switch (te) {
        case teOBJECT:
        {
            ObjectScenarioModel *model = newItem->data(0, Qt::UserRole).value<ObjectScenarioModel *>();
            ObjectScenarioModel *newModel = new ObjectScenarioModel();
            newModel->setTitle(text);
            newModel->setId(id_text);
            newModel->setProperties(model->properties());
            newModel->setActions(model->actions());
            newItem->setData(0,Qt::UserRole,QVariant::fromValue(const_cast<ObjectScenarioModel*>(newModel)));
            jo = newModel->toJson();
        }
            break;
        case teALGORITHM:
        {
            AlgorithmModel *model = newItem->data(0, Qt::UserRole).value<AlgorithmModel *>();
            AlgorithmModel *newModel = new AlgorithmModel();
            newModel->setTitle(text);
            newModel->setId(id_text);
            newModel->setDescription(model->getDescription());
            newModel->setFunction(model->getFunction());
            newModel->setType(model->getType());
            newModel->setMSubType(model->getMSubType());
            newModel->setInputParameters(model->getInputParameters());
            newModel->setOutputParameters(model->getOutputParameters());
            newItem->setData(0,Qt::UserRole,QVariant::fromValue(const_cast<AlgorithmModel*>(newModel)));
            jo = newModel->toJson();
        }
            break;
        case teLIBFILE:
        {
            FileModel *model = newItem->data(0, Qt::UserRole).value<FileModel *>();
            FileModel *newModel = new FileModel();
            newModel->setFileName(text);
            newModel->setId(id_text);
            newModel->setType(model->getType());
            newModel->setMd5Hash(model->getMd5Hash());
            newItem->setData(0,Qt::UserRole,QVariant::fromValue(const_cast<FileModel*>(newModel)));
            jo = newModel->toJson();
        }
            break;
        case teTACTICAL_SIGN:
        {
            TacticalSignModel *model = newItem->data(0, Qt::UserRole).value<TacticalSignModel *>();
            TacticalSignModel *newModel = new TacticalSignModel();

            newModel->setTitle(text);
            newModel->setId(id_text);

            newModel->setDescription(model->getDescription());
            newModel->setType(model->type());
            newModel->setParentObjectId(model->getParentObjectId());
            newModel->setIs_own(model->is_own());
            newModel->setClassCod(model->getClassCod());
            newModel->setLine_width(model->line_width());
            newModel->setScale(model->scale());
            newModel->setSpline(model->spline());
            newModel->setColor(model->color());
            newModel->setCoordinates(model->coordinates());
            newModel->setGeometry_type(model->geometry_type());
            newModel->setIconPath(model->getIconPath());
            newModel->setSchemePath(model->getSchemePath());
            newModel->setShortName(model->getShortName());
            newModel->setGroup(model->getGroup());

            newModel->setProperties(model->properties());

            newItem->setData(0,Qt::UserRole,QVariant::fromValue(const_cast<TacticalSignModel*>(newModel)));
            jo = newModel->toJson();
        }
            break;
        case teINTERACTION:
        {
            InteractionModelTemplate *model = newItem->data(0, Qt::UserRole).value<InteractionModelTemplate *>();
            InteractionModelTemplate *newModel = new InteractionModelTemplate();

            newModel->setTitle(text);
            newModel->setId(id_text);

//            newModel->setDescription(model->getDescription());
//            newModel->setInteractionType(model->getInteractionType());
//            newModel->setCountObject(model->getCountObject());
//            newModel->setSourceObjectId(model->getSourceObjectId());
//            newModel->setResponseAction(model->getResponseAction());
//            newModel->setResponseSetValues(model->getResponseSetValues());
//            newModel->setTriggerCondition(model->getTriggerCondition());

            newItem->setData(0,Qt::UserRole,QVariant::fromValue(const_cast<InteractionModelTemplate*>(newModel)));
            jo = newModel->toJson();
        }
            break;
        default:
            return;
        }

        parent->addChild(newItem);

        //            treeWidget->setCurrentItem(newItem);
        m_itemClicked(newItem,parent->data(0,Qt::UserRole).toInt());

        DataStorageServiceFactory::getInstance()->addElement(te, jo);

    }

    currentId = id_text;
    currentTab = ui->toolBox->currentIndex();
    currentTitle = text;
    updateSelection();
    addModelObjDialog= nullptr;
}


void ModelObjectsForm::onlyObjects() {
    ui->toolBox->removeItem(1);
    treeAlgorithms->setVisible(false);
    ui->toolBox->removeItem(1);
    treeLibFiles->setVisible(false);
    ui->toolBox->removeItem(1);
    treeTacticalSign->setVisible(false);
}

const QList<DataStorageItem *> &ModelObjectsForm::getListTacticalSign() const
{
    return listTacticalSign;
}

void ModelObjectsForm::setListTacticalSign(const QList<DataStorageItem *> &newListTacticalSign)
{
    listTacticalSign = newListTacticalSign;
}

const QList<DataStorageItem *> &ModelObjectsForm::getTemplateListTacticalSign() const
{
    return templateListTacticalSign;
}

void ModelObjectsForm::setTemplateListTacticalSign(const QList<DataStorageItem *> &newTemplateListTacticalSign)
{
    templateListTacticalSign = newTemplateListTacticalSign;
}

const QList<DataStorageItem *> &ModelObjectsForm::getListIterations() const
{
    return listIterations;
}

void ModelObjectsForm::setListIterations(const QList<DataStorageItem *> &newListIterations)
{
    listIterations = newListIterations;
}

void ModelObjectsForm::updateSelection()
{
    bool first = currentId != "";
    bool second = currentTab == ui->toolBox->currentIndex();
    bool third = currentTitle!="";

    QTreeWidget *tree;
    switch (ui->toolBox->currentIndex()) {
    case 0:
        tree = treeObjects;
        break;
    case 1:
        tree = treeAlgorithms;
        break;
    case 2:
        tree = treeLibFiles;
        break;
    case 3:
        tree = treeTacticalSign;
        break;
    default:
        break;
    }
    if (first && second && third){

        QList<QTreeWidgetItem *> listItems = tree->findItems(currentId,Qt::MatchWrap|Qt::MatchWildcard|Qt::MatchRecursive,1);
        if (listItems.size()==0)
            listItems.append(tree->findItems(currentTitle,Qt::MatchWrap|Qt::MatchWildcard|Qt::MatchRecursive,0));
        if (listItems.size()>0){
            tree->setCurrentItem(listItems.at(0));
            m_itemClicked(listItems.at(0),ui->toolBox->currentIndex());
        }    else{
            QList<QTreeWidgetItem *> listItems = tree->findItems(" ",Qt::MatchContains | Qt::MatchRecursive,0);;
            if (listItems.size()>0){
                tree->setCurrentItem(listItems.at(0));
                m_itemClicked(listItems.at(0),ui->toolBox->currentIndex());
            }
        }
    }
    else{
        QList<QTreeWidgetItem *> listItems = tree->findItems(" ",Qt::MatchContains | Qt::MatchRecursive,0);;
        if (listItems.size()>0){
            tree->setCurrentItem(listItems.at(0));
            m_itemClicked(listItems.at(0),ui->toolBox->currentIndex());
        }
    }
}
