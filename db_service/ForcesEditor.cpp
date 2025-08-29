// forceseditor.cpp
#include "ForcesEditor.h"
#include "modelobjectsform.h"
#include "../models/ObjectScenarioModel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QJsonArray>
#include <QJsonObject>
#include <QDirIterator>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QScreen>
#include <QLineEdit>
#include <QInputDialog>
#include <QTableWidget>
#include <QHeaderView>
#include <QTimer>
#include <QDesktopWidget>

ForcesEditor::ForcesEditor(QWidget *parent) : QWidget(parent)
{
    setupUi();
    //    loadData();
    //    listObjects = newTreeWidget->getListObjects();
    //    QList<DataStorageItem *> _listAlgorithms= newTreeWidget->getListAlgorithms();
    //    QStringList listik={};
    //    for (auto var : _listAlgorithms) {
    //        listik.append(var->getId());
    //    }
    //    listik.sort();

    //    for (auto var : listik) {
    //        for (auto var2 : _listAlgorithms) {
    //            if (var == var2->getId()){
    //                listAlgorithms.append(var2);
    //            }
    //        }
    //    }

    //    listLibFiles = newTreeWidget->getListLibFiles();
    //    listTacticalSign = newTreeWidget->getListTacticalSign();
    //    listIterations = newTreeWidget->getListIterations();
    updateData();
    resize(1440,840);
    QDesktopWidget desktop;
    QRect rect = desktop.availableGeometry(desktop.primaryScreen()); // прямоугольник с размерами экрана
    QPoint center = rect.center(); // координаты центра экрана
    int x = center.x() - (this->width()/2); // учитываем половину ширины окна
    int y = center.y() - (this->height()/2); // аналогично для высоты
    center.setX(x);
    center.setY(y);
    this->move(center);
}

void ForcesEditor::setupUi()
{
    QHBoxLayout *formLayout = new QHBoxLayout(this);
    QVBoxLayout *mainLayout = new QVBoxLayout();

    // Создаем кнопки управления
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    addButton = new QPushButton("Добавить", this);
    removeButton = new QPushButton("Удалить", this);
    //    editButton = new QPushButton("Изменить", this);
    copyButton = new QPushButton("Дублировать", this);


    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(removeButton);
    //    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(copyButton);
    buttonLayout->setContentsMargins(10,0,10,0);
    //Подменяем дерево
    newTreeWidget = new ModelObjectsForm(this);
    newTreeWidget->loadData();
    mainLayout->addWidget(newTreeWidget);
    mainLayout->addLayout(buttonLayout);

    //coздаем таблички для редактирования
    QVBoxLayout *tablesLayout = new QVBoxLayout();
    QHBoxLayout *titleLayout = new QHBoxLayout();
    name = new QLabel();
    //    QSpacerItem *si = new QSpacerItem();
    QLabel *id_title = new QLabel();
    id_title->setText("Идентификатор");

    id = new QLineEdit();
    id->setText("...");
    titleLayout->addWidget(name);
    titleLayout->addSpacerItem(new QSpacerItem(10,10,QSizePolicy::Expanding,QSizePolicy::Minimum));
    titleLayout->addWidget(id_title);
    titleLayout->addWidget(id);

    tableParameters = new QTableWidget();
    tableParameters->setColumnCount(4);
    tableParameters->setSelectionMode(QAbstractItemView::SingleSelection);
    tableParameters->setHorizontalHeaderLabels({"Наименование","Свойство","Значение","Тип"});
    tableParameters->setColumnWidth(0,190);
    tableParameters->setColumnWidth(1,150);
    tableParameters->setColumnWidth(2,150);
    tableParameters->setColumnWidth(3,75);
    tableParameters->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);

    tableAlgoritms = new QTableWidget();
    tableAlgoritms->setColumnCount(2);
    tableAlgoritms->setSelectionMode(QAbstractItemView::SingleSelection);
    tableAlgoritms->setHorizontalHeaderLabels({"Наименование (Идентификатор)","Тип"});
    tableAlgoritms->setColumnWidth(0,415);
    tableAlgoritms->setColumnWidth(1,150);
    tableAlgoritms->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);

    tableOutPutMapping = new QTableWidget();
    tableOutPutMapping->setColumnCount(3);
    tableOutPutMapping->setSelectionMode(QAbstractItemView::SingleSelection);
    tableOutPutMapping->setHorizontalHeaderLabels({"Параметр","Значение","Тип"});
    tableOutPutMapping->setColumnWidth(0,200);
    tableOutPutMapping->setColumnWidth(1,200);
    tableOutPutMapping->setColumnWidth(2,200);
    tableOutPutMapping->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);



    tableMainAlg = new QTableWidget();
    tableMainAlg->setColumnCount(2);
    tableMainAlg->setSelectionMode(QAbstractItemView::SingleSelection);
    tableMainAlg->setHorizontalHeaderLabels({"Наименование","Значение"});
    tableMainAlg->setColumnWidth(0,150);
    tableMainAlg->setColumnWidth(1,415);
    tableMainAlg->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);

    tableInPut = new QTableWidget();
    tableInPut->setColumnCount(3);
    tableInPut->setSelectionMode(QAbstractItemView::SingleSelection);
    tableInPut->setHorizontalHeaderLabels({"Имя","Описание","Тип"});
    tableInPut->setColumnWidth(0,150);
    tableInPut->setColumnWidth(1,340);
    tableInPut->setColumnWidth(2,75);
    tableInPut->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);

    tableOutPut = new QTableWidget();
    tableOutPut->setColumnCount(3);
    tableOutPut->setSelectionMode(QAbstractItemView::SingleSelection);
    tableOutPut->setHorizontalHeaderLabels({"Имя","Описание","Тип"});
    tableOutPut->setColumnWidth(0,150);
    tableOutPut->setColumnWidth(1,340);
    tableOutPut->setColumnWidth(2,75);
    tableOutPut->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);

    tableLibFiles = new QTableWidget();
    tableLibFiles->setColumnCount(2);
    tableLibFiles->setSelectionMode(QAbstractItemView::SingleSelection);
    tableLibFiles->setHorizontalHeaderLabels({"Наименование","Значение"});
    for (int var = 0; var < tableLibFiles->columnCount(); ++var) {
        tableLibFiles->setColumnWidth(var,285);
    }
    tableLibFiles->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);

    tableTacticalSigns = new QTableWidget();
    tableTacticalSigns->setColumnCount(2);
    tableTacticalSigns->setSelectionMode(QAbstractItemView::SingleSelection);
    tableTacticalSigns->setHorizontalHeaderLabels({"Наименование","Значение"});
    for (int var = 0; var < tableTacticalSigns->columnCount(); ++var) {
        tableTacticalSigns->setColumnWidth(var,285);
    }
    tableTacticalSigns->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);

    tableTacticalSigns_prop = new QTableWidget();
    tableTacticalSigns_prop->setColumnCount(4);
    tableTacticalSigns_prop->setSelectionMode(QAbstractItemView::SingleSelection);
    tableTacticalSigns_prop->setHorizontalHeaderLabels({"Наименование","Название","Значение","Тип"});
    tableTacticalSigns_prop->setColumnWidth(0,150);
    tableTacticalSigns_prop->setColumnWidth(1,150);
    tableTacticalSigns_prop->setColumnWidth(2,190);
    tableTacticalSigns_prop->setColumnWidth(3,75);
    tableTacticalSigns_prop->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);

    tableInteractionProp = new QTableWidget();
    tableInteractionProp->setColumnCount(2);
    tableInteractionProp->setSelectionMode(QAbstractItemView::SingleSelection);
    tableInteractionProp->setHorizontalHeaderLabels({"Наименование","Значение"});
    for (int var = 0; var < tableInteractionProp->columnCount(); ++var) {
        tableInteractionProp->setColumnWidth(var,285);
    }
    tableInteractionProp->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);

    tableInteractionItems = new QTableWidget();
    tableInteractionItems->setColumnCount(2);
    tableInteractionItems->setSelectionMode(QAbstractItemView::SingleSelection);
    tableInteractionItems->setHorizontalHeaderLabels({"Идентификатор","Описание"});
    tableInteractionItems->setColumnWidth(0,185);
    tableInteractionItems->setColumnWidth(1,385);
    tableInteractionItems->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);

    connect(tableInteractionProp, &QTableWidget::cellDoubleClicked, this, [=](int row, int col){
        if (row>2 && col>0){
            QString title = tableInteractionProp->item(row,0)->text().trimmed();
            QString text = tableInteractionProp->item(row,1)->text().trimmed();
            TextEditDialog* ted = new TextEditDialog(title, text, this);
            connect(ted,&TextEditDialog::textChanged,this,[=](QString val){
                tableInteractionProp->item(row,1)->setText(val);
            });
            ted->exec();
        }
    });

    //    tablesLayout->addLayout(titleLayout);
    w_prop = new QWidget();
    QVBoxLayout *L1 = new QVBoxLayout(w_prop);
    L1->addWidget(tableParameters);
    w_prop->setLayout(L1);

    w_mapping = new QWidget();
    _cb_outputmapping = new QComboBox(w_mapping);
    QVBoxLayout *L1_1 = new QVBoxLayout(w_mapping);
    L1_1->addWidget(_cb_outputmapping);
    L1_1->addWidget(tableOutPutMapping);
    w_mapping->setLayout(L1_1);
    connect(_cb_outputmapping,&QComboBox::currentTextChanged,this,&ForcesEditor::comboboxActivated);

    w_alg = new QWidget();
    QVBoxLayout *L2 = new QVBoxLayout(w_alg);
    L2->addWidget(tableAlgoritms);
    w_alg->setLayout(L2);
    // Табы для алгоритмов
    w_alg_main = new QWidget();
    QVBoxLayout *L3 = new QVBoxLayout(w_alg_main);
    L3->addWidget(tableMainAlg);
    w_alg_main->setLayout(L3);

    w_alg_input = new QWidget();
    QVBoxLayout *L4 = new QVBoxLayout(w_alg_input);
    L4->addWidget(tableInPut);
    w_alg_input->setLayout(L4);

    w_alg_output = new QWidget();
    QVBoxLayout *L5 = new QVBoxLayout(w_alg_output);
    L5->addWidget(tableOutPut);
    w_alg_output->setLayout(L5);
    //Таба для файла
    w_lib_file = new QWidget();
    QVBoxLayout *L6 = new QVBoxLayout(w_lib_file);
    L6->addWidget(tableLibFiles);
    w_lib_file->setLayout(L6);
    //Таба для знаков
    w_tactical_sign = new QWidget();
    QVBoxLayout *L7 = new QVBoxLayout(w_tactical_sign);
    L7->addWidget(tableTacticalSigns);
    w_tactical_sign->setLayout(L7);
    w_tactical_sign_prop = new QWidget();
    QVBoxLayout *L7_1 = new QVBoxLayout(w_tactical_sign_prop);
    L7_1->addWidget(tableTacticalSigns_prop);
    w_tactical_sign_prop->setLayout(L7_1);

    //Таба для итераций
    w_interaction_prop = new QWidget();
    QVBoxLayout *L8 = new QVBoxLayout(w_interaction_prop);
    L8->addWidget(tableInteractionProp);
    w_interaction_prop->setLayout(L8);

    w_interaction_items = new QWidget();
    QVBoxLayout *L9 = new QVBoxLayout(w_interaction_items);
    L9->addWidget(tableInteractionItems);
    w_interaction_items->setLayout(L9);



    btn_remove = new QPushButton("Удалить", this);
    btn_remove->setMinimumWidth(150);
    btn_add = new QPushButton("Добавить", this);
    btn_add->setMinimumWidth(150);
    btn_save = new QPushButton("Сохранить", this);
    btn_save->setMinimumWidth(150);

    tabWidget = new QTabWidget();
    tabWidget->addTab(w_prop,"Параметры");
    tabWidget->addTab(w_alg,"Алгоритмы");

    QHBoxLayout *tablesButtonLayout = new QHBoxLayout();


    tablesButtonLayout->addSpacerItem(new QSpacerItem(10,10,QSizePolicy::Expanding,QSizePolicy::Minimum));
    tablesButtonLayout->addWidget(btn_add);
    tablesButtonLayout->addWidget(btn_remove);
    tablesButtonLayout->addWidget(btn_save);

    connect(btn_add, &QPushButton::clicked, this, &ForcesEditor::onAddDataToTable);
    connect(btn_remove, &QPushButton::clicked, this, &ForcesEditor::onDelDataToTable);
    connect(btn_save, &QPushButton::clicked, this, &ForcesEditor::onSaveDataTable);
    //
    tablesLayout->addWidget(tabWidget);
    tablesLayout->addLayout(tablesButtonLayout);

    formLayout->addLayout(mainLayout);
    formLayout->addLayout(tablesLayout);
    formLayout->setStretch(0,2);
    formLayout->setStretch(1,3);
    // Подключаем сигналы
    connect(addButton, &QPushButton::clicked, this, &ForcesEditor::onAddClicked);
    connect(removeButton, &QPushButton::clicked, this, &ForcesEditor::onRemoveClicked);
    connect(copyButton, &QPushButton::clicked, this, &ForcesEditor::onCopyClicked);

    // Устанавливаем минимальный размер окна
    this->setMinimumSize(1080, 700);
    this->setWindowTitle("Редактор Сил и Средств");

    // Выровняем окно по центру экрана
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - this->width()) / 2;
    int y = (screenGeometry.height() - this->height()) / 2;
    this->move(x,y);
    this->resize(1440,840);

    // Добавляем сигнал слот на отображение данных в таблице
    //    connect(treeWidget,SIGNAL(itemClicked(QTreeWidgetItem *, int)),this,SLOT(showDataInTables(QTreeWidgetItem *, int)));
    connect(newTreeWidget,SIGNAL(itemClicked(QTreeWidgetItem *, int)),this,SLOT(showDataInTables(QTreeWidgetItem *, int)));

    //Добавляем сигнал слот по скрытию кнопок при выборе разных табов
    connect(tabWidget, SIGNAL(currentChanged(int)),this, SLOT(currentTabChanged(int)));
}

void ForcesEditor::showDataInTables(QTreeWidgetItem *item, int typeElement)
{
    disconnect(tabWidget, SIGNAL(currentChanged(int)),this, SLOT(currentTabChanged(int)));
    tableParameters->clearContents();
    tableParameters->setRowCount(0);
    tableOutPutMapping->clearContents();
    tableOutPutMapping->setRowCount(0);
    tableAlgoritms->clearContents();
    tableAlgoritms->setRowCount(0);
    tableInPut->clearContents();
    tableInPut->setRowCount(0);
    tableOutPut->clearContents();
    tableOutPut->setRowCount(0);
    tableMainAlg->clearContents();
    tableMainAlg->setRowCount(0);
    tableLibFiles->clearContents();
    tableLibFiles->setRowCount(0);
    tableTacticalSigns->clearContents();
    tableTacticalSigns->setRowCount(0);
    tableTacticalSigns_prop->clearContents();
    tableTacticalSigns_prop->setRowCount(0);
    tableInteractionItems->clearContents();
    tableInteractionItems->setRowCount(0);
    tableInteractionProp->clearContents();
    tableInteractionProp->setRowCount(0);

    tabWidget->clear();

    if (item==nullptr)
        return;
    QTreeWidgetItem * parent = item->parent();
    if (parent==nullptr)
        parent = item;
    //    TypeElement te = (TypeElement)parent->data(0,Qt::UserRole).toInt();
    switch (typeElement) {
    case 0:{


        ObjectScenarioModel *model = item->data(0, Qt::UserRole).value<ObjectScenarioModel *>();
        if (model==nullptr)
            return;
        tabWidget->clear();
        tabWidget->addTab(w_prop,"Параметры");
        tabWidget->addTab(w_alg,"Алгоритмы");
        tabWidget->addTab(w_mapping,"Сопоставление параметров");
        btn_add->setVisible(true);
        btn_remove->setVisible(true);
        btn_save->setVisible(true);
//        QMap<QString, PropertyModel*> m_properties = {};//model->properties();
        QList<ActionModel*> m_actions = model->actions();

        //сортируем по названию
        QList<PropertyModel*> m_properties = model->properties().values();
        qSort(m_properties.begin(),m_properties.end(),[](PropertyModel* a,PropertyModel* b){return a->title()<b->title();});

        tableParameters->setRowCount(/*m_properties.keys()*/m_properties.size());
        int i = 0;
        for (auto var : m_properties/*.keys()*/) {
            QTableWidgetItem *item_1 = new QTableWidgetItem (var->name());
            tableParameters->setItem(i,1,item_1);

            QTableWidgetItem *item_3 = new QTableWidgetItem (var->title());
            tableParameters->setItem(i,0,item_3);

            QComboBox *cb_choose_type = new QComboBox();
            cb_choose_type->addItems(param_types);
            cb_choose_type->setCurrentText(var->type());
            tableParameters->setCellWidget(i,3,cb_choose_type);

            QTableWidgetItem *item_2 = new QTableWidgetItem (var->stringValue());
            tableParameters->setItem(i,2,item_2);
            i++;
        }
        tableAlgoritms->setRowCount(m_actions.size());
        i = 0;

        mapping = new QMap<QString,QMap<QString,QString>*>();
        _cb_outputmapping->clear();
        for (ActionModel *var : m_actions) {
            QString name_id = var->algorithmId() + " - "+var->name();

            _cb_outputmapping->addItem(name_id,var->algorithmId());
            QComboBox *cb_choose_name_alg = new QComboBox();
            for (auto var : listAlgorithms) {
                //                cb_choose_name_alg->addItem(var->getTitle() + " ("+var->getId()+")");
                cb_choose_name_alg->addItem(var->getId() + " - "+var->getTitle(),var->getId());
            }
            cb_choose_name_alg->setCurrentText(name_id);
            tableAlgoritms->setCellWidget(i,0,cb_choose_name_alg);
            QTableWidgetItem *item_2 = new QTableWidgetItem (var->type());
            tableAlgoritms->setItem(i,1,item_2);
            i++;
            QMap<QString,QString> *_mapping = new QMap<QString,QString>();
            for (auto _var : var->outputMapping()) {
                _mapping->insert(_var->name(),_var->value_string());
            }
            mapping->insert(var->algorithmId(),_mapping);
        }
        comboboxActivated("");
        //        _cb_outputmapping->addItems(algoritms_list);
        //тут еще добавляем структурки данных для корректного отображения в следующей вкладке всяких комбобоксов
        //Сделать и проверить TODO
    }
        break;
    case 1:{


        AlgorithmModel *model = item->data(0, Qt::UserRole).value<AlgorithmModel *>();
        if (model==nullptr)
            return;
        tabWidget->clear();
        tabWidget->addTab(w_alg_main,"Основные характеристики");
        tabWidget->addTab(w_alg_input,"Входящие параметры");
        tabWidget->addTab(w_alg_output,"Исходящие параметры");
        btn_add->setVisible(false);
        btn_remove->setVisible(false);
        btn_save->setVisible(true);
        tableMainAlg->clearContents();
        tableMainAlg->setRowCount(5);

        QTableWidgetItem *item_12 = new QTableWidgetItem ("Описание");
        tableMainAlg->setItem(0,0,item_12);
        item_12->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        QTableWidgetItem *item_22 = new QTableWidgetItem (model->getDescription());
        tableMainAlg->setItem(0,1,item_22);

        QTableWidgetItem *item_13 = new QTableWidgetItem ("Тип");
        tableMainAlg->setItem(1,0,item_13);
        item_13->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        //        QTableWidgetItem *item_23 = new QTableWidgetItem (model->getType());
        //        tableMainAlg->setItem(1,1,item_23);
        QComboBox *alg_type = new QComboBox();
        for (auto var : algoritm_type) {
            alg_type->addItem(var,QVariant::fromValue(const_cast<AlgorithmModel*>(model)));
        }
        tableMainAlg->setCellWidget(1,1,alg_type);
        connect(alg_type,static_cast<void (QComboBox::*)(const int)>(&QComboBox::currentIndexChanged),[alg_type,this](int index){
            AlgorithmModel *model = alg_type->currentData(Qt::UserRole).value<AlgorithmModel *>();
            int i = alg_type->currentIndex();
            if (i==1){
                QTableWidgetItem *item_14 = new QTableWidgetItem ("id библиотеки");
                tableMainAlg->setItem(2,0,item_14);
                item_14->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                QComboBox *lib_id = new QComboBox();
                for (auto var : listLibFiles) {
                    lib_id->addItem(var->getId());
                }
                tableMainAlg->setCellWidget(2,1,lib_id);
                lib_id->setCurrentText(model->getFunction().split(".")[0]);

                QTableWidgetItem *item_16 = new QTableWidgetItem ("Функция");
                tableMainAlg->setItem(3,0,item_16);
                item_14->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                QString txt = "";
                if (model->getFunction().split(".").size()>1)
                    txt = model->getFunction().split(".")[1];
                QTableWidgetItem *item_26 = new QTableWidgetItem (txt);
                tableMainAlg->setItem(3,1,item_26);
            }
            if (i==0){
                QTableWidgetItem *item_14 = new QTableWidgetItem ("id библиотеки");
                tableMainAlg->setItem(2,0,item_14);
                item_14->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                tableMainAlg->removeCellWidget(2,1);
                QString mainFunct = "ModelTasks";
                //                if (model->getFunction().split(".")[0].contains("function"))
                //                    mainFunct = model->getFunction().split(".")[0];
                QTableWidgetItem *item_24 = new QTableWidgetItem (mainFunct);
                tableMainAlg->setItem(2,1,item_24);
                QTableWidgetItem *item_16 = new QTableWidgetItem ("Функция");
                tableMainAlg->setItem(3,0,item_16);
                item_14->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                QString funct = "";
                if (model->getFunction().split(".").size()>1)
                    funct = model->getFunction().split(".")[1];
                QTableWidgetItem *item_26 = new QTableWidgetItem (funct);
                tableMainAlg->setItem(3,1,item_26);
            }
        });
        alg_type->setCurrentIndex(1);
        alg_type->setCurrentText(model->getType());


        QTableWidgetItem *item_15 = new QTableWidgetItem ("Характер");
        tableMainAlg->setItem(4,0,item_15);
        item_15->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        QTableWidgetItem *item_25 = new QTableWidgetItem (model->getMSubType());
        tableMainAlg->setItem(4,1,item_25);

        auto m_input = model->getInputParameters();

        tableInPut->setRowCount(m_input.size());
        int i = 0;
        for (AlgorithParamModel *var : m_input) {
            QTableWidgetItem *item_1 = new QTableWidgetItem (var->name());
            tableInPut->setItem(i,0,item_1);
            QTableWidgetItem *item_2 = new QTableWidgetItem (var->title().toString());
            tableInPut->setItem(i,1,item_2);
            //            QTableWidgetItem *item_3 = new QTableWidgetItem (var->value().toString());
            //            tableInPut->setItem(i,2,item_3);
            QComboBox *cb_choose_type = new QComboBox();
            cb_choose_type->addItems(param_types);
            cb_choose_type->setCurrentText(var->value().toString());
            tableInPut->setCellWidget(i,2,cb_choose_type);
            i++;
        }

        auto m_output = model->getOutputParameters();

        tableOutPut->setRowCount(m_output.size());
        i = 0;
        for (AlgorithParamModel *var : m_output) {
            QTableWidgetItem *item_1 = new QTableWidgetItem (var->name());
            tableOutPut->setItem(i,0,item_1);
            QTableWidgetItem *item_2 = new QTableWidgetItem (var->title().toString());
            tableOutPut->setItem(i,1,item_2);
            //            QTableWidgetItem *item_3 = new QTableWidgetItem (var->value().toString());
            //            tableOutPut->setItem(i,2,item_3);
            QComboBox *cb_choose_type = new QComboBox();
            cb_choose_type->addItems(param_types);
            cb_choose_type->setCurrentText(var->value().toString());
            tableOutPut->setCellWidget(i,2,cb_choose_type);
            i++;
        }
    }
        break;
    case 2:{

        FileModel *model = item->data(0, Qt::UserRole).value<FileModel *>();
        if (model==nullptr)
            return;
        tabWidget->clear();
        tabWidget->addTab(w_lib_file,"Файл библиотеки");
        tableLibFiles->clearContents();
        tableLibFiles->setRowCount(2);
        //        QTableWidgetItem *item_1 = new QTableWidgetItem ("Идентификатор");
        //        tableLibFiles->setItem(0,0,item_1);
        //        QTableWidgetItem *item_2 = new QTableWidgetItem (model->getId());
        //        tableLibFiles->setItem(0,1,item_2);
        //
        //        QTableWidgetItem *item_11 = new QTableWidgetItem ("Наименование файла");
        //        tableLibFiles->setItem(1,0,item_11);
        //        QTableWidgetItem *item_21 = new QTableWidgetItem (model->getFileName());
        //        tableLibFiles->setItem(1,1,item_21);

        QTableWidgetItem *item_12 = new QTableWidgetItem ("Тип");
        tableLibFiles->setItem(0,0,item_12);
        item_12->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        QTableWidgetItem *item_22 = new QTableWidgetItem (model->getType());
        tableLibFiles->setItem(0,1,item_22);

        QTableWidgetItem *item_13 = new QTableWidgetItem ("md5_hash");
        tableLibFiles->setItem(1,0,item_13);
        item_13->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        QTableWidgetItem *item_23 = new QTableWidgetItem (model->getMd5Hash());
        tableLibFiles->setItem(1,1,item_23);
    }
        break;
    case 3:{

        TacticalSignModel *model = item->data(0, Qt::UserRole).value<TacticalSignModel *>();
        if (model==nullptr)
            return;
        tabWidget->clear();
        tabWidget->addTab(w_tactical_sign,"Тактические знаки");
        tabWidget->addTab(w_tactical_sign_prop,"Параметры");
        btn_add->setVisible(false);
        btn_remove->setVisible(false);

        tableTacticalSigns->clearContents();
        tableTacticalSigns->setRowCount(15);

        setItemToTable("Описание",0,0,tableTacticalSigns,true);
        setItemToTable(model->getDescription(),0,1,tableTacticalSigns);

        setItemToTable("Тип",1,0,tableTacticalSigns,true);
        setItemToTable(model->type(),1,1,tableTacticalSigns);

        setItemToTable("Дочерний знак",2,0,tableTacticalSigns,true);
//        setItemToTable(model->getParentObjectId(),2,1,tableTacticalSigns);
        QStringList no_yes = {"нет","да"};
        QComboBox *_cb_1 = new QComboBox();
        _cb_1->addItems(no_yes);
        _cb_1->setCurrentIndex(model->getParentObjectId()=="no"?0:1);
        tableTacticalSigns->setCellWidget(2,1,_cb_1);

        setItemToTable("Принадлежность",3,0,tableTacticalSigns,true);
        setItemToTable(QString::number(model->is_own()),3,1,tableTacticalSigns);

        setItemToTable("Класс код",4,0,tableTacticalSigns,true);
        setItemToTable(model->getClassCod(),4,1,tableTacticalSigns);

        setItemToTable("Ширина линии",5,0,tableTacticalSigns,true);
        setItemToTable(QString::number(model->line_width()),5,1,tableTacticalSigns);

        setItemToTable("Масштаб",6,0,tableTacticalSigns,true);
        setItemToTable(QString::number(model->scale()),6,1,tableTacticalSigns);

        setItemToTable("Сплайн",7,0,tableTacticalSigns,true);
//        setItemToTable(QVariant(model->spline()).toString(),7,1,tableTacticalSigns);

        QComboBox *_cb_0 = new QComboBox();
        _cb_0->addItems(no_yes);
        _cb_0->setCurrentIndex(model->spline()?1:0);
        tableTacticalSigns->setCellWidget(7,1,_cb_0);

        setItemToTable("Цвет",8,0,tableTacticalSigns,true);
        setItemToTable(model->color(),8,1,tableTacticalSigns);

        QJsonDocument doc;
        doc.setArray(model->coordinates());
        QString dataToString(doc.toJson());
        setItemToTable("Координаты",9,0,tableTacticalSigns,true);
        setItemToTable(dataToString,9,1,tableTacticalSigns);

        setItemToTable("Тип геометрии",10,0,tableTacticalSigns,true);
//        setItemToTable(QString::number(model->geometry_type()),10,1,tableTacticalSigns);
        QStringList geomTypes = {"точечный","линейный","площадной","окружность","прямоугольный","текстовый","квадратный"};
        QComboBox *_cb = new QComboBox();
        _cb->addItems(geomTypes);
        _cb->setCurrentIndex(model->geometry_type());
        tableTacticalSigns->setCellWidget(10,1,_cb);

        setItemToTable("Краткое наименование",11,0,tableTacticalSigns,true);
        setItemToTable(model->getShortName(),11,1,tableTacticalSigns);

        setItemToTable("Иконка",12,0,tableTacticalSigns,true);
        setItemToTable(model->getIconPath(),12,1,tableTacticalSigns);
        setItemToTable("Схема",13,0,tableTacticalSigns,true);
        setItemToTable(model->getSchemePath(),13,1,tableTacticalSigns);
        setItemToTable("Группа знаков",14,0,tableTacticalSigns,true);
        setItemToTable(model->getGroup(),14,1,tableTacticalSigns);

        QMap<QString, PropertyModel*> m_properties = model->properties();
        tableTacticalSigns_prop->setRowCount(m_properties.keys().size());
        int i = 0;
        for (QString var : m_properties.keys()) {
            QTableWidgetItem *item_1 = new QTableWidgetItem (m_properties.value(var)->name());
            tableTacticalSigns_prop->setItem(i,0,item_1);

            QTableWidgetItem *item_3 = new QTableWidgetItem (m_properties.value(var)->title());
            tableTacticalSigns_prop->setItem(i,1,item_3);

            QComboBox *cb_choose_type = new QComboBox();
            cb_choose_type->addItems(param_types);
            cb_choose_type->setCurrentText(m_properties.value(var)->type());
            tableTacticalSigns_prop->setCellWidget(i,3,cb_choose_type);

            QTableWidgetItem *item_2 = new QTableWidgetItem (m_properties.value(var)->stringValue());
            tableTacticalSigns_prop->setItem(i,2,item_2);
            i++;
        }

    }
        break;
    case 4:{
        tabWidget->clear();
        tabWidget->addTab(w_interaction_prop,"Параметры");
        tabWidget->addTab(w_interaction_items,"Список объектов");
        InteractionModelTemplate *model = item->data(0, Qt::UserRole).value<InteractionModelTemplate *>();
        if (model==nullptr)
            return;
        btn_add->setVisible(false);
        btn_remove->setVisible(false);
        btn_save->setVisible(true);
        tableInteractionProp->clearContents();
        tableInteractionProp->setRowCount(6);

        //        setItemToTable("Описание",0,0,tableInteractionProp,true);
        //        setItemToTable(model->getDescription(),0,1,tableInteractionProp);
        //
        //        setItemToTable("Объект",1,0,tableInteractionProp,true);
        //        setItemToTable(model->getSourceObjectId(),1,1,tableInteractionProp);
        //
        //        setItemToTable("Тип взаимодействия",2,0,tableInteractionProp,true);
        //        setItemToTable(model->getInteractionType(),2,1,tableInteractionProp);
        //
        //        setItemToTable("Триггер взаимодействия",3,0,tableInteractionProp,true);
        //        setItemToTable(model->getTriggerCondition(),3,1,tableInteractionProp,true);
        //
        //        setItemToTable("Ответные действия",4,0,tableInteractionProp,true);
        //        setItemToTable(model->getResponseAction(),4,1,tableInteractionProp,true);
        //        setItemToTable("Ответные значения",5,0,tableInteractionProp,true);
        //        setItemToTable(model->getResponseSetValues(),5,1,tableInteractionProp,true);
        int i =0;
        tableInteractionItems->clearContents();
        tableInteractionItems->setRowCount(model->getItems().count());
        for (auto var : model->getItems()) {
            setItemToTable(var->getName(),i,0,tableInteractionItems);
            setItemToTable(var->getDescription(),i,1,tableInteractionItems);
            i++;
        }
    }
    default:
        break;
    }


//    QTimer::singleShot(50,this,[=](){
//        if (tabWidget->count()>curTab){
//            tabWidget->setCurrentIndex(curTab);

//        }
//    });
    connect(tabWidget, SIGNAL(currentChanged(int)),this, SLOT(currentTabChanged(int)));
        tabWidget->setCurrentIndex(currentTabInWidget);

}

void ForcesEditor::setItemToTable(QString title, int row, int col, QTableWidget *table, bool editable){
    QTableWidgetItem *item = new QTableWidgetItem (title);
    table->setItem(row,col,item);
    if (editable)
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
}

void ForcesEditor::currentTabChanged(int curTab){

    this->currentTabInWidget = curTab;
    if (newTreeWidget->currentItem() == nullptr)
        return;
    QTreeWidgetItem * parent = newTreeWidget->currentItem()->parent();
    if (parent==nullptr)
        parent = newTreeWidget->currentItem();
    TypeElement te = (TypeElement)parent->data(0,Qt::UserRole).toInt();
    switch (te) {
    case teOBJECT: {
        switch (curTab) {
        case 0:
            btn_add->setVisible(true);
            btn_remove->setVisible(true);
            break;
        case 1:
            btn_add->setVisible(true);
            btn_remove->setVisible(true);
            break;
        default:
            break;
        }
    }
        break;
    case teALGORITHM: {
        switch (curTab) {
        case 0:
            btn_add->setVisible(false);
            btn_remove->setVisible(false);
            break;
        case 1:
            btn_add->setVisible(true);
            btn_remove->setVisible(true);
            break;
        case 2:
            btn_add->setVisible(true);
            btn_remove->setVisible(true);
            break;
        default:
            break;
        }
    }
        break;
    case teLIBFILE: {
        btn_add->setVisible(false);
        btn_remove->setVisible(false);
    }
        break;
    case teTACTICAL_SIGN: {
        switch (curTab) {
        case 0:
            btn_add->setVisible(false);
            btn_remove->setVisible(false);
            break;
        case 1:
            btn_add->setVisible(true);
            btn_remove->setVisible(true);
            break;
        default:
            break;
        }
    }
        break;
    case teINTERACTION: {
        switch (curTab) {
        case 0:
            btn_add->setVisible(false);
            btn_remove->setVisible(false);
            break;
        case 1:
            btn_add->setVisible(true);
            btn_remove->setVisible(true);
            break;
        default:
            break;
        }
    }
        break;
    }

}

void ForcesEditor::onAddDataToTable()
{
    int curTab = tabWidget->currentIndex();
    QTreeWidgetItem * parent = newTreeWidget->currentItem()->parent();
    if (parent==nullptr)
        parent = newTreeWidget->currentItem();
    TypeElement te = (TypeElement)parent->data(0,Qt::UserRole).toInt();
    switch (te) {
    case teOBJECT: {
        switch (curTab) {
        case 0:{
            tableParameters->setRowCount(tableParameters->rowCount() + 1);
            QComboBox *cb_choose_type = new QComboBox();
            cb_choose_type->addItems(param_types);
            tableParameters->setCellWidget(tableParameters->rowCount() - 1, 3, cb_choose_type);
        }break;
        case 1:{
            tableAlgoritms->setRowCount(tableAlgoritms->rowCount() + 1);
            QComboBox *cb_choose_name_alg = new QComboBox();
            for (auto var : listAlgorithms) {
                cb_choose_name_alg->addItem(var->getId() + " - "+var->getTitle());
            }
            tableAlgoritms->setCellWidget(tableAlgoritms->rowCount() -1,0,cb_choose_name_alg);
        }break;
        case 2:{
            int row = tableOutPutMapping->rowCount();
            tableOutPutMapping->setRowCount(row + 1);
            //            QComboBox *_cb_1 = new QComboBox();
            //            QStringList _params,_types;
            //            for (int row = 0; row < tableParameters->rowCount(); ++row) {
            //                _params.append(tableParameters->item(row,0)->text().trimmed());
            //                 QComboBox *cb = qobject_cast<QComboBox*>(tableParameters->cellWidget(row,3));
            //                 _types.append(cb->currentText());
            //            }
            //            _cb_1->addItems(_params);
            //            connect(_cb_1,&QComboBox::textActivated,this,[=](QString txt){
            //                QComboBox* _cb_2 = new QComboBox();
            //                QStringList _params_2,_types_2;
            //                for (auto var_3 : listAlgorithms) {
            //                    QString v3 = var_3->getId();
            //                    if (v3 == _cb_outputmapping->currentData(Qt::UserRole).toString()){
            //                        AlgorithmModel *algModel = AlgorithmModel::fromJson(var_3->getData(), nullptr);
            //                        for (auto _apm : algModel->getOutputParameters()) {
            //                            if ( _apm->value_string() == _types.at(_params.indexOf(txt)))
            //                            _params_2.append(_apm->name());
            //                            _types_2.append(_apm->value_string());
            //                        }
            //                    }
            //                }
            //                _cb_2->addItems(_params_2);
            //                tableOutPutMapping->setCellWidget(row,1,_cb_2);
            //            });

            //            tableOutPutMapping->setCellWidget(row,0,_cb_1);


            QComboBox * cb_2 = new QComboBox();
            QStringList params_2,types_2;
            for (auto var_3 : listAlgorithms) {
                QString v3 = var_3->getId();
                if (v3 == _cb_outputmapping->currentData(Qt::UserRole).toString()){
                    AlgorithmModel *algModel = AlgorithmModel::fromJson(var_3->getData(), nullptr);
                    for (auto _apm : algModel->getOutputParameters()) {
                        params_2.append(_apm->name());
                        types_2.append(_apm->value_string());
                    }
                }
            }
            cb_2->addItems(params_2);
            connect(cb_2,&QComboBox::textActivated,this,[=](QString txt){
                QComboBox *_cb_1 = new QComboBox();
                QStringList _params,_types;
                for (int row = 0; row < tableParameters->rowCount(); ++row) {
                    QComboBox *cb = qobject_cast<QComboBox*>(tableParameters->cellWidget(row,3));
                    QString _type = (cb->currentText());
                    if (types_2.at(params_2.indexOf(txt))== _type)
                        _params.append(tableParameters->item(row,1)->text().trimmed());
                }
                setItemToTable(types_2.at(params_2.indexOf(txt)),row,2,tableOutPutMapping,true);
                _cb_1->addItems(_params);
                tableOutPutMapping->setCellWidget(row,0,_cb_1);
            });
            QComboBox *_cb_1 = new QComboBox();
            QStringList _params,_types;
            for (int row = 0; row < tableParameters->rowCount(); ++row) {
                QComboBox *cb = qobject_cast<QComboBox*>(tableParameters->cellWidget(row,3));
                QString _type = (cb->currentText());
                if (types_2.size()>0 && types_2.at(0)== _type)
                    _params.append(tableParameters->item(row,1)->text().trimmed());
            }
            _cb_1->addItems(_params);
            tableOutPutMapping->setCellWidget(row,0,_cb_1);
            tableOutPutMapping->setCellWidget(row,1,cb_2);
        }break;
        default:
            break;
        }
    }
        break;
    case teALGORITHM: {
        switch (curTab) {
        case 0:
            tableMainAlg->setRowCount(tableMainAlg->rowCount() + 1);
            break;
        case 1:{
            tableInPut->setRowCount(tableInPut->rowCount() + 1);
            QComboBox *cb_choose_type = new QComboBox();
            cb_choose_type->addItems(param_types);
            tableInPut->setCellWidget(tableInPut->rowCount() - 1, 2, cb_choose_type);
        }break;
        case 2: {
            tableOutPut->setRowCount(tableOutPut->rowCount() + 1);
            QComboBox *cb_choose_type = new QComboBox();
            cb_choose_type->addItems(param_types);
            tableOutPut->setCellWidget(tableOutPut->rowCount() - 1, 2, cb_choose_type);

        }break;
        default:
            break;
        }
    }
        break;
    case teLIBFILE: {
        tableLibFiles->setRowCount(tableLibFiles->rowCount() + 1);
    }
        break;
    case teTACTICAL_SIGN:{
        tableTacticalSigns_prop->setRowCount(tableTacticalSigns_prop->rowCount()+1);
        QComboBox *cb_choose_type = new QComboBox();
        cb_choose_type->addItems(param_types);
        tableTacticalSigns_prop->setCellWidget(tableTacticalSigns_prop->rowCount() - 1, 3, cb_choose_type);

    }
        break;
    case teINTERACTION: {
        tableInteractionItems->setRowCount(tableInteractionItems->rowCount() + 1);
    }
        break;
    }
}

void ForcesEditor::onDelDataToTable()
{
    int curTab = tabWidget->currentIndex();
    QTreeWidgetItem * parent = newTreeWidget->currentItem()->parent();
    if (parent==nullptr)
        parent = newTreeWidget->currentItem();
    TypeElement te = (TypeElement)parent->data(0,Qt::UserRole).toInt();
    switch (te) {
    case teOBJECT: {
        switch (curTab) {
        case 0:
            tableParameters->removeRow(tableParameters->currentRow());
            break;
        case 1:
            tableAlgoritms->removeRow(tableAlgoritms->currentRow());
            break;
        case 2:
            tableOutPutMapping->removeRow(tableOutPutMapping->currentRow());
            break;
        default:
            break;
        }
    }
        break;
    case teALGORITHM: {
        switch (curTab) {
        case 0:
            tableMainAlg->removeRow(tableMainAlg->currentRow());
            break;
        case 1:
            tableInPut->removeRow(tableInPut->currentRow());
            break;
        case 2:
            tableOutPut->removeRow(tableOutPut->currentRow());
            break;
        default:
            break;
        }
    }
        break;
    case teLIBFILE: {
        tableLibFiles->removeRow(tableLibFiles->currentRow());
    } break;
    case teTACTICAL_SIGN: {
        tableTacticalSigns_prop->removeRow(tableTacticalSigns_prop->currentRow());
    }
        break;
    case teINTERACTION: {
        tableInteractionItems->removeRow(tableInteractionItems->currentRow());
    }
        break;
    }
}

void ForcesEditor::onSaveDataTable()
{
    if (QMessageBox::question(this, "Подтверждение", "Сохранить выбранный элемент?") == QMessageBox::Yes) {
        QTreeWidgetItem *currItem = newTreeWidget->currentItem();
        if (currItem==nullptr)
            return;
        QTreeWidgetItem *parent = currItem->parent();
        TypeElement te;
        if (parent==nullptr)
            te = newTreeWidget->getcurrentTypeElement();
        else
        {
            qDebug()<<parent->text(0)<<parent->data(0,Qt::UserRole);
            try {
                te = (TypeElement)parent->data(0,Qt::UserRole).toInt();
            }  catch (...) {

            }

        }
        QJsonObject jo;

        switch (te) {
        case teOBJECT:{
            ObjectScenarioModel *newModel = currItem->data(0,Qt::UserRole).value<ObjectScenarioModel*>();
            for (QString key : newModel->properties().keys()) {
                newModel->removeProperty(key);
            }
            for (int var = 0; var < tableParameters->rowCount(); ++var) {
                if (tableParameters->item(var,0)!=nullptr){
                PropertyModel* pm = new PropertyModel () ;
                pm->setName(tableParameters->item(var,1)==nullptr?"":tableParameters->item(var,1)->text().trimmed());
                pm->setTitle(tableParameters->item(var,0)==nullptr?"":tableParameters->item(var,0)->text().trimmed());
                QComboBox *cb = qobject_cast<QComboBox*>(tableParameters->cellWidget(var,3));
                pm->setType(cb->currentText());
                bool ok = false;
                if (pm->type()!="feature"){
                    pm->setValue(tableParameters->item(var,2)==nullptr?"0":tableParameters->item(var,2)->text().trimmed().replace(",","."));
                    ok = true;
                }else{
                    QString str = tableParameters->item(var,2)->text().trimmed();
                    QJsonObject obj;
                    QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8());
                    if(!doc.isNull())
                    {
                        if(doc.isObject())
                        {
                            obj = doc.object();
                            ok =true;
                            try {
                                pm->setValue(obj);
                            } catch (...) {
                                ok = false;
                                qDebug() << "Ошибка преобразования значения geo_path обьекта в feature";
                            }
                        }
                        else
                        {
                            qDebug() << "Документ не является объектом";
                        }
                    }
                    else
                    {
                        qDebug() << "Недействительный JSON...\n" << str;
                    }
                }
                if (ok)
                    newModel->addProperty(pm->name(),pm);
                }
            }
            QList<ActionModel*> old_a_models;
            for (auto _act : newModel->actions()) {
                ActionModel *alg = new ActionModel();
                alg->setAlgorithmId(_act->algorithmId());
                alg->setName(_act->name());
                alg->setType(_act->type());
                QList<AlgorithParamModel * > m_outputmapping;
                for (auto  _opm : _act->outputMapping()) {
                    AlgorithParamModel *_apm = new AlgorithParamModel(_opm->name(),_opm->value_string(),_opm->title().toString(),nullptr);
                    m_outputmapping.append(_apm);
                }
                alg->setOutputMapping(m_outputmapping);
                old_a_models.append(alg);
            }
            newModel->clearActions();
            QList<ActionModel*> a_models= {};
            for (int var = 0; var < tableAlgoritms->rowCount(); ++var) {
                ActionModel *alg = new ActionModel();
                //сохраним данные из вкладки "Сопоставление параметров"

                alg->setType(tableAlgoritms->item(var,1)==nullptr?"":tableAlgoritms->item(var,1)->text().trimmed());
                QComboBox *cb = qobject_cast<QComboBox*>(tableAlgoritms->cellWidget(var,0));
                QString str = cb->currentText().split(" - ")[0];
                QString name = cb->currentText().replace(str+" - ","");
//                alg->setName(cb->currentText().split(" - ")[1]);
                alg->setName(name);

                //                str.resize(str.length()-1);
                alg->setAlgorithmId(str);
                QString str_2 = _cb_outputmapping->currentData(Qt::UserRole).toString();
                //                str_2.resize(str_2.length()-1);

                QList<AlgorithParamModel * > m_outputmapping;
                if (str==str_2){
                    for (int i = 0; i < tableOutPutMapping->rowCount(); ++i) {
                        //                        AlgorithParamModel * _apm = new AlgorithParamModel(tableOutPutMapping->item(i,0)->text().trimmed(),
                        //                                                                           tableOutPutMapping->item(i,1)->text().trimmed(),"",nullptr);
                        AlgorithParamModel * _apm = new AlgorithParamModel(
                                    qobject_cast<QComboBox*>(tableOutPutMapping->cellWidget(i,0))->currentText(),
                                    qobject_cast<QComboBox*>(tableOutPutMapping->cellWidget(i,1))->currentText(),
                                    "",nullptr);
                        m_outputmapping.append(_apm);
                    }
                    alg->setOutputMapping(m_outputmapping);
                }else
                    for (auto _var : old_a_models) {
                        if (_var!=nullptr && _var->algorithmId()!=nullptr && _var->algorithmId() == str)
                            alg->setOutputMapping(_var->outputMapping());
                    }

                a_models.append(alg);
            }
            newModel->setActions(a_models);
            jo = newModel->toJson();
        }break;

        case teALGORITHM:{
            AlgorithmModel *newModel = currItem->data(0,Qt::UserRole).value<AlgorithmModel*>();

            //                newModel->setId(tableMainAlg->item(0,1)->text().trimmed());
            //                newModel->setTitle(tableMainAlg->item(1,1)->text().trimmed());
            newModel->setDescription(tableMainAlg->item(0,1)==nullptr?"":tableMainAlg->item(0,1)->text().trimmed());
            //            newModel->setType(tableMainAlg->item(1,1)->text().trimmed()); ушли от ввода текста вручную на комбарь
            QComboBox *cb_type = qobject_cast<QComboBox*>(tableMainAlg->cellWidget(1,1));
            newModel->setType(cb_type->currentText());
            if (cb_type->currentIndex()==1){
                newModel->setFunction(qobject_cast<QComboBox*>(tableMainAlg->cellWidget(2,1))->currentText()+"."+tableMainAlg->item(3,1)->text().trimmed());
            }else{
                QString s1 = tableMainAlg->item(2,1)==nullptr?"":tableMainAlg->item(2,1)->text().trimmed();
                QString s2 = tableMainAlg->item(3,1)==nullptr?"":tableMainAlg->item(3,1)->text().trimmed();
                newModel->setFunction(s1+"."+s2);
            }
            newModel->setMSubType(tableMainAlg->item(4,1)==nullptr?"":tableMainAlg->item(4,1)->text().trimmed());
            const QList<AlgorithParamModel *> m_inputList;
            newModel->setInputParameters(m_inputList);
            const QList<AlgorithParamModel *> m_outputList;
            newModel->setOutputParameters(m_outputList);
            for (int i = 0; i < tableInPut->rowCount(); ++i) {
                QComboBox *cb = qobject_cast<QComboBox*>(tableInPut->cellWidget(i,2));
                QString s1 = tableInPut->item(i,0)==nullptr?"":tableInPut->item(i,0)->text().trimmed();
                QString s2 = cb->currentText();
                QString s3 = "";
                if (tableInPut->item(i,1)!=nullptr)
                    s3 = tableInPut->item(i,1)->text().trimmed();
                newModel->addInputParameter(s1,s2,s3);
            }
            for (int i = 0; i < tableOutPut->rowCount(); ++i) {
                QComboBox *cb = qobject_cast<QComboBox*>(tableOutPut->cellWidget(i,2));
                QString s1 = tableOutPut->item(i,0)==nullptr?"":tableOutPut->item(i,0)->text().trimmed();
                QString s2 = cb->currentText();
                QString s3 = "";
                if (tableOutPut->item(i,1)!=nullptr)
                    s3 = tableOutPut->item(i,1)->text().trimmed();
                newModel->addOutputParameter(s1,s2,s3);
            }

            jo = newModel->toJson();
        }break;
        case teLIBFILE:{
            FileModel *newModel = currItem->data(0,Qt::UserRole).value<FileModel*>();

            //                newModel->setId(tableLibFiles->item(0,1)->text().trimmed());
            //                newModel->setFileName(tableLibFiles->item(1,1)->text().trimmed());
            newModel->setType(tableLibFiles->item(0,1)==nullptr?"":tableLibFiles->item(0,1)->text().trimmed());
            newModel->setMd5Hash(tableLibFiles->item(1,1)==nullptr?"":tableLibFiles->item(1,1)->text().trimmed());

            jo = newModel->toJson();
        }break;
        case teTACTICAL_SIGN:{
            TacticalSignModel *newModel = currItem->data(0,Qt::UserRole).value<TacticalSignModel*>();

            newModel->setDescription(tableTacticalSigns->item(0,1)==nullptr?"":tableTacticalSigns->item(0,1)->text().trimmed());
            newModel->setType(tableTacticalSigns->item(1,1)==nullptr?"":tableTacticalSigns->item(1,1)->text().trimmed());
//            newModel->setParentObjectId(tableTacticalSigns->item(2,1)==nullptr?"":tableTacticalSigns->item(2,1)->text().trimmed());
            QComboBox *cb = qobject_cast<QComboBox*>(tableTacticalSigns->cellWidget(2,1));
            newModel->setParentObjectId(cb->currentIndex()==0?"no":"yes");
            newModel->setIs_own(tableTacticalSigns->item(3,1)==nullptr?0:tableTacticalSigns->item(3,1)->text().trimmed().toInt());
            newModel->setClassCod(tableTacticalSigns->item(4,1)==nullptr?"":tableTacticalSigns->item(4,1)->text().trimmed());
            newModel->setLine_width(tableTacticalSigns->item(5,1)==nullptr?0:tableTacticalSigns->item(5,1)->text().trimmed().replace(",",".").toDouble());
            newModel->setScale(tableTacticalSigns->item(6,1)==nullptr?0:tableTacticalSigns->item(6,1)->text().trimmed().toInt());
            newModel->setSpline(QVariant(tableTacticalSigns->item(7,1)==nullptr?"false":tableTacticalSigns->item(7,1)->text().trimmed()).toBool());
            newModel->setColor(tableTacticalSigns->item(8,1)==nullptr?"":tableTacticalSigns->item(8,1)->text().trimmed());
            QJsonArray obj;
            QJsonDocument doc = QJsonDocument::fromJson(tableTacticalSigns->item(9,1)==nullptr?"":tableTacticalSigns->item(9,1)->text().trimmed().toUtf8());
            if(!doc.isNull())
            {
                if(doc.isArray())
                {
                    obj = doc.array();
                }
                else
                {
                    QMessageBox::warning(this,"Ошибка!","Невозможно текст преобразовать в JSON документ");
                }
            }
            else
            {
                QMessageBox::warning(this,"Ошибка!","Невозможно JSON преобразовать в список координат");
            }
            newModel->setCoordinates(obj);
//            newModel->setGeometry_type(tableTacticalSigns->item(10,1)==nullptr?0:tableTacticalSigns->item(10,1)->text().trimmed().toInt());
            cb = qobject_cast<QComboBox*>(tableTacticalSigns->cellWidget(10,1));
            newModel->setGeometry_type(cb->currentIndex());
            newModel->setIconPath(tableTacticalSigns->item(12,1)==nullptr?"":tableTacticalSigns->item(12,1)->text().trimmed());
            newModel->setSchemePath(tableTacticalSigns->item(13,1)==nullptr?"":tableTacticalSigns->item(13,1)->text().trimmed());
            newModel->setShortName(tableTacticalSigns->item(11,1)==nullptr?"":tableTacticalSigns->item(11,1)->text().trimmed());
            newModel->setGroup(tableTacticalSigns->item(14,1)==nullptr?"":tableTacticalSigns->item(14,1)->text().trimmed());


            newModel->removePropertys();

            for (int var = 0; var < tableTacticalSigns_prop->rowCount(); ++var) {
                if (tableTacticalSigns_prop->item(var,0)!=nullptr){
                PropertyModel* pm = new PropertyModel () ;
                pm->setName(tableTacticalSigns_prop->item(var,0)==nullptr?"":tableTacticalSigns_prop->item(var,0)->text().trimmed());
                pm->setTitle(tableTacticalSigns_prop->item(var,1)==nullptr?"":tableTacticalSigns_prop->item(var,1)->text().trimmed());
                QComboBox *cb = qobject_cast<QComboBox*>(tableTacticalSigns_prop->cellWidget(var,3));
                pm->setType(cb->currentText());
                bool ok = false;
                if (pm->type()!="feature"){
                    pm->setValue(tableTacticalSigns_prop->item(var,2)==nullptr?0:tableTacticalSigns_prop->item(var,2)->text().trimmed().replace(",","."));
                    ok = true;
                }else{
                    QString str = tableTacticalSigns_prop->item(var,2)==nullptr?"":tableTacticalSigns_prop->item(var,2)->text().trimmed();
                    QJsonObject obj;
                    QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8());
                    if(!doc.isNull())
                    {
                        if(doc.isObject())
                        {
                            obj = doc.object();
                            ok =true;
                            try {
                                pm->setValue(obj);
                            } catch (...) {
                                ok = false;
                                qDebug() << "Ошибка преобразования значения geo_path обьекта в feature";
                            }
                        }
                        else
                        {
                            qDebug() << "Документ не является объектом";
                        }
                    }
                    else
                    {
                        qDebug() << "Недействительный JSON...\n" << str;
                    }
                }
                if (ok)
                    newModel->addProperty(pm->name(),pm);
            }
            }

            jo = newModel->toJson();
        }break;
        case teINTERACTION:{
            InteractionModelTemplate *newModel = currItem->data(0,Qt::UserRole).value<InteractionModelTemplate*>();

            //            newModel->setDescription(tableInteractionProp->item(0,1)->text().trimmed());
            //            newModel->setSourceObjectId(tableInteractionProp->item(1,1)->text().trimmed());
            //            newModel->setInteractionType(tableInteractionProp->item(2,1)->text().trimmed());
            //            newModel->setTriggerCondition(tableInteractionProp->item(3,1)->text().trimmed());
            //            newModel->setResponseAction(tableInteractionProp->item(4,1)->text().trimmed());
            //            newModel->setResponseSetValues(tableInteractionProp->item(5,1)->text().trimmed());
            QList<InteractionTemplateItem*> itms;
            for (int var = 0; var < tableInteractionItems->rowCount(); ++var) {
                InteractionTemplateItem* itm = new InteractionTemplateItem();
                itm->setName(tableInteractionItems->itemAt(var,0)==nullptr?"":tableInteractionItems->itemAt(var,0)->text().trimmed());
                itm->setDescription(tableInteractionItems->itemAt(var,1)==nullptr?"":tableInteractionItems->itemAt(var,1)->text().trimmed());
                itms.append(itm);
            }
            newModel->setItems(itms);

            jo = newModel->toJson();
        }break;
        default:
            return;
        }

        DataStorageServiceFactory::getInstance()->updateElement(te,jo);
    }

    updateData();
}

bool ForcesEditor::checkOutUnicId(QString id_text, QList<DataStorageItem *> list)
{
    for (auto var : list) {
        if (var->getId()==id_text){
            QMessageBox::warning(this,"Дублирование ID","Идентификатор должен быть уникальным");
            onAddClicked();
            return true;
        }
    }
    return false;
}

void ForcesEditor::comboboxActivated(QString val){
    tableOutPutMapping->clearContents();
    tableOutPutMapping->setRowCount(0);
    QString val_id = _cb_outputmapping->currentData(Qt::UserRole).toString();
    if (mapping->contains(val_id)){
        tableOutPutMapping->setRowCount(mapping->value(val_id)->keys().size());
        int i = 0;
        for (QString var : mapping->value(val_id)->keys()) {
            QTableWidgetItem *item_1 = new QTableWidgetItem (var);
            tableOutPutMapping->setItem(i,0,item_1);
            QComboBox* _cb_2 = new QComboBox();
            QStringList _params_2,_types_2;
            for (auto var_3 : listAlgorithms) {
                QString v3 = var_3->getId()+" - "+var_3->getTitle();
                if (var_3->getId() == _cb_outputmapping->currentData(Qt::UserRole)){
                    AlgorithmModel *algModel = AlgorithmModel::fromJson(var_3->getData(), nullptr);
                    for (auto _apm : algModel->getOutputParameters()) {
                        _params_2.append(_apm->name());
                        _types_2.append(_apm->value_string());
                    }
                }
            }
            _cb_2->addItems(_params_2);

            connect(_cb_2,&QComboBox::textActivated,this,[=](QString txt){
                QComboBox *_cb_1 = new QComboBox();
                QStringList _params,_types;
                for (int row = 0; row < tableParameters->rowCount(); ++row) {
                    QComboBox *cb = qobject_cast<QComboBox*>(tableParameters->cellWidget(row,3));
                    QString _type = (cb->currentText());
                    if (_types_2.at(_params_2.indexOf(txt))== _type)
                        _params.append(tableParameters->item(row,1)->text().trimmed());
                }
                _cb_1->addItems(_params);
                setItemToTable(_types_2.at(_params_2.indexOf(txt)),i,2,tableOutPutMapping,true);
                tableOutPutMapping->setCellWidget(i,0,_cb_1);
            });
            _cb_2->setCurrentText(mapping->value(val_id)->value(var));
            QComboBox *_cb_1 = new QComboBox();
            QStringList _params,_types;
            for (int row = 0; row < tableParameters->rowCount(); ++row) {
                _params.append(tableParameters->item(row,1)->text().trimmed());
                QComboBox *cb = qobject_cast<QComboBox*>(tableParameters->cellWidget(row,3));
                _types.append(cb->currentText());
            }
            _cb_1->addItems(_params);
            _cb_1->setCurrentText(var);
            QString _wtf = mapping->value(val_id)->value(var);
            if (_params_2.contains(_wtf))
                setItemToTable(_types_2.at(_params_2.indexOf(_wtf)),i,2,tableOutPutMapping,true);
            tableOutPutMapping->setCellWidget(i,1,_cb_2);
            tableOutPutMapping->setCellWidget(i,0,_cb_1);

            i++;
        }
    }
}

void ForcesEditor::updateData()
{
    newTreeWidget->loadData();

    listObjects = newTreeWidget->getListObjects();
    QList<DataStorageItem *> _listAlgorithms= newTreeWidget->getListAlgorithms();
    QStringList listik={};
    for (auto var : _listAlgorithms) {
        listik.append(var->getId());
    }
    listik.sort();
    listAlgorithms.clear();
    for (auto var : listik) {
        for (auto var2 : _listAlgorithms) {
            if (var == var2->getId()){
                listAlgorithms.append(var2);
            }
        }
    }

    listLibFiles = newTreeWidget->getListLibFiles();
    listTacticalSign = newTreeWidget->getListTacticalSign();

    listIterations = newTreeWidget->getListIterations();
    newTreeWidget->updateSelection();


}

void ForcesEditor::onAddClicked()
{
    QString subType = nullptr;
    if (newTreeWidget->currentItem()!=nullptr)
    if (newTreeWidget->currentItem()->parent()!=nullptr)
        if (newTreeWidget->currentItem()->parent()->parent()!=nullptr)
            if (newTreeWidget->currentItem()->parent()->text(0)!=nullptr)
                subType = newTreeWidget->currentItem()->parent()->text(0);
    newTreeWidget->onAddClicked(subType);
    newTreeWidget->updateSelection();
    updateData();
}

void ForcesEditor::onRemoveClicked()
{
    QTreeWidgetItem *currentItem = newTreeWidget->currentItem();
    if (!currentItem) return;
    QTreeWidgetItem *parent = currentItem->parent();
    if (!parent) return;

    if (QMessageBox::question(this, "Подтверждение", "Удалить выбранный элемент?") == QMessageBox::Yes) {

        TypeElement te = (TypeElement)parent->data(0,Qt::UserRole).toInt();
        QJsonObject jo;
        switch (te) {
        case teOBJECT:
            jo = currentItem->data(0,Qt::UserRole).value<ObjectScenarioModel *>()->toJson();
            break;
        case teALGORITHM:
            jo = currentItem->data(0,Qt::UserRole).value<AlgorithmModel *>()->toJson();
            break;
        case teLIBFILE:
            jo = currentItem->data(0,Qt::UserRole).value<FileModel *>()->toJson();
            break;
        case teTACTICAL_SIGN:
            jo = currentItem->data(0,Qt::UserRole).value<TacticalSignModel *>()->toJson();
            break;
        case teINTERACTION:
            jo = currentItem->data(0,Qt::UserRole).value<InteractionModelTemplate *>()->toJson();
            break;
        default:
            break;
        }

        DataStorageServiceFactory::getInstance()->removeElement(te,jo);
        if (currentItem->parent()!=nullptr){
            if (currentItem->parent()->childCount()==1)
                delete currentItem->parent();
            else
                delete currentItem;
        }else
            delete currentItem;
    }

//    newTreeWidget->updateSelection();
    updateData();
}

void ForcesEditor::onCopyClicked()
{
    newTreeWidget->onCopyClicked();
//    newTreeWidget->updateSelection();
    updateData();
}

