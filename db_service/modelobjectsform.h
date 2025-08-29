#ifndef MODELOBJECTSFORM_H
#define MODELOBJECTSFORM_H

#include <QWidget>
#include <QTreeWidget>
#include <QPushButton>
#include "QComboBox"
#include <QJsonDocument>
#include <QTableWidget>
#include <QLabel>
#include <QToolBox>
#include "../models/ObjectScenarioModel.h"
#include "../models/AlgorithmModel.h"
#include "../models/FileModel.h"
#include "../models/TacticalSignModel.h"
#include "../models/InteractionModelTemplate.h"
#include "./components/addmodelobjectsdialog.h"

#include "./services/DataStorageServiceFactory.h"

namespace Ui {
class ModelObjectsForm;
}

class ModelObjectsForm : public QWidget
{
    Q_OBJECT

public:
    explicit ModelObjectsForm(QWidget *parent = nullptr);
    ~ModelObjectsForm();
    void loadData();
    QTreeWidgetItem *currentItem();
    const QList<DataStorageItem *> &getListObjects() const;
    void setListObjects(const QList<DataStorageItem *> &newListObjects);

    const QList<DataStorageItem *> &getListAlgorithms() const;
    void setListAlgorithms(const QList<DataStorageItem *> &newListAlgorithms);

    const QList<DataStorageItem *> &getListLibFiles() const;
    void setListLibFiles(const QList<DataStorageItem *> &newListLibFiles);

    TypeElement getcurrentTypeElement();

    /**
     * Скрывает все табы кроме объектов
     */
    void onlyObjects();

    const QList<DataStorageItem *> &getListTacticalSign() const;
    void setListTacticalSign(const QList<DataStorageItem *> &newListTacticalSign);

    const QList<DataStorageItem *> &getTemplateListTacticalSign() const;
    void setTemplateListTacticalSign(const QList<DataStorageItem *> &newTemplateListTacticalSign);

    const QList<DataStorageItem *> &getListIterations() const;
    void setListIterations(const QList<DataStorageItem *> &newListIterations);

    void updateSelection();

signals:
    void itemClicked(QTreeWidgetItem *, int);



private:
    Ui::ModelObjectsForm *ui;

    QTreeWidget *treeObjects = new QTreeWidget();
    QTreeWidget *treeAlgorithms = new QTreeWidget();
    QTreeWidget *treeLibFiles = new QTreeWidget();
    QTreeWidget *treeTacticalSign = new QTreeWidget();
    QTreeWidget *treeIterations = new QTreeWidget();

    QList<DataStorageItem *> listObjects;
    QList<DataStorageItem *> listAlgorithms;
    QList<DataStorageItem *> listLibFiles;
    QList<DataStorageItem *> listTacticalSign;
    QList<DataStorageItem *> listIterations;
    //Шаблоны
    QList<DataStorageItem *> templateListObjects;
    QList<DataStorageItem *> templateListAlgorithms;
    QList<DataStorageItem *> templateListLibFiles;
    QList<DataStorageItem *> templateListTacticalSign;
    QList<DataStorageItem *> templateListIterations;

    QMap<QString,QTreeWidgetItem*> subParents;
    QMap<QString,QMap<QString,QTreeWidgetItem*> > fullTypeParents;

    void createTree(QTreeWidget *);

    void createTreeItem(QString subType, QString type, QTreeWidgetItem*item, TypeElement typeElement, QTreeWidgetItem *ItemObjects);

    //Еще списки для др окон
    // 1 - type
    // 2 - sub_type
    // 3 - ids

    QMap<TypeElement,QList<QPair<int,QStringList>>> mapOfTypesAndIds;

    AddModelObjectsDialog *addModelObjDialog;

    QString currentTitle ="";
    QString currentId="";
    int currentTab=0;

private slots:

    void m_itemClicked(QTreeWidgetItem *, int);
    void addNewObject(QString title, QString id, QString type, QString sub_type, QString function);
    void copyNewObject(QString title,QString id, QString type, QString sub_type);

public slots:

    void onAddClicked(QString subType);
    void onRemoveClicked();
    void onCopyClicked();
};

#endif // MODELOBJECTSFORM_H
