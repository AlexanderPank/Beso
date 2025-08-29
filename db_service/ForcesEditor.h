// forceseditor.h
#ifndef FORCESEDITOR_H
#define FORCESEDITOR_H

#include <QWidget>
#include <QTreeWidget>
#include <QPushButton>
#include "QComboBox"
#include <QJsonDocument>
#include <QTableWidget>
#include <QLabel>
#include "../models/ObjectScenarioModel.h"
#include "../models/AlgorithmModel.h"
#include "../models/FileModel.h"

#include "./services/DataStorageServiceFactory.h"

#include "./components/addmodelobjectsdialog.h"
#include "./components/texteditdialog.h"

#include "modelobjectsform.h"

class ForcesEditor : public QWidget
{
Q_OBJECT

public:
    explicit ForcesEditor(QWidget *parent = nullptr);

private slots:
    void onAddClicked();
    void onRemoveClicked();
    void onCopyClicked();

    void showDataInTables(QTreeWidgetItem *, int);
    void currentTabChanged(int);
    void setItemToTable(QString title, int row, int col, QTableWidget *table, bool editable = false);

    void onAddDataToTable();
    void onDelDataToTable();
    void onSaveDataTable();

    bool checkOutUnicId(QString id_text, QList<DataStorageItem *> list);

    void comboboxActivated(QString val);
    void updateData();
private:
    //типы переменных
    QStringList param_types = {"int","float","feature","str","bool","list","double","any","OUTPUT_STRUCT_POINTS"};
    QStringList algoritm_type = {"inner","libcpp"};
    QMap<QString,QMap<QString,QString>*> *mapping;

//    QTreeWidget *treeWidget;
    ModelObjectsForm *newTreeWidget;
    QLabel *name;
    QLineEdit *id;
    //таблицы для обьектов
    QWidget *w_prop;
    QTableWidget *tableParameters;
    QWidget *w_mapping;
    QComboBox *_cb_outputmapping;
    QTableWidget *tableOutPutMapping;
    QWidget *w_alg;
    QTableWidget *tableAlgoritms;
    //Таблицы для алгоритмов
    QWidget *w_alg_main;
    QTableWidget *tableMainAlg;
    QWidget *w_alg_input;
    QTableWidget *tableInPut;
    QWidget *w_alg_output;
    QTableWidget *tableOutPut;
    //Таблица для файлов
    QWidget *w_lib_file;
    QTableWidget *tableLibFiles;
    //Таблица для знаков
    QWidget *w_tactical_sign;
    QTableWidget *tableTacticalSigns;
    QWidget *w_tactical_sign_prop;
    QTableWidget *tableTacticalSigns_prop;
    //Таблицы для итераций
    QWidget *w_interaction_prop;
    QWidget *w_interaction_items;
    QTableWidget *tableInteractionProp;
    QTableWidget *tableInteractionItems;

    QPushButton *addButton;
    QPushButton *removeButton;
    QPushButton *copyButton;

    QTabWidget *tabWidget;
    QPushButton *btn_add;
    QPushButton *btn_remove;
    QPushButton *btn_save;

    void setupUi();

    QList<DataStorageItem *> listObjects;
    QList<DataStorageItem *> listAlgorithms;
    QList<DataStorageItem *> listLibFiles;
    QList<DataStorageItem *> listTacticalSign;
    QList<DataStorageItem *> listIterations;
//    //Шаблоны
//    QList<DataStorageItem *> templateListObjects;
//    QList<DataStorageItem *> templateListAlgorithms;
//    QList<DataStorageItem *> templateListLibFiles;
//    QList<DataStorageItem *> templatelistTacticalSign;

//    QMap<QString,QTreeWidgetItem*> subParents;

    //Окно по добавлению нового обьекта
    AddModelObjectsDialog *addDialog;

    //отслеживание текущей табы
    int currentTabInWidget = 0;

};

#endif // FORCESEDITOR_H
