#include "itemtoolbox.h"
#include "ui_itemtoolbox.h"
#include "../db_service/services/FileDataStorageService.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QStyle>


ItemToolBox::ItemToolBox(QWidget *parent) :
    QToolBox(parent),
    ui(new Ui::ItemToolBox),
    algoritmTree(new QTreeWidget(this))
{
    ui->setupUi(this);

    algoritmTree->setHeaderHidden(true);
    addItem(algoritmTree, tr("Алгоритмы"));
    loadAlgorithms();
}

ItemToolBox::~ItemToolBox()
{
    delete ui;
}

void ItemToolBox::loadAlgorithms()
{
    QDir dir(QString(MAIN_DIR_DEFAULT) + SUB_DIR_ALGORITHMS);
    dir.setNameFilters({"*.json"});
    QFileInfoList files = dir.entryInfoList();

    QMap<QString, QTreeWidgetItem*> typeItems;
    QMap<QString, QMap<QString, QTreeWidgetItem*>> subTypeItems;

    QIcon folderIcon = QApplication::style()->standardIcon(QStyle::SP_DirIcon);
    QIcon fileIcon = QApplication::style()->standardIcon(QStyle::SP_FileIcon);

    for (const QFileInfo &info : files) {
        QFile f(info.absoluteFilePath());
        if (!f.open(QIODevice::ReadOnly))
            continue;
        QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
        f.close();
        QJsonObject obj = doc.object();

        QString type = obj["type"].toString();
        QString subType = obj["subType"].toString();
        QString title = obj["title"].toString(info.completeBaseName());

        QTreeWidgetItem *typeItem = nullptr;
        if (!typeItems.contains(type)) {
            typeItem = new QTreeWidgetItem(algoritmTree);
            typeItem->setText(0, type);
            typeItem->setIcon(0, folderIcon);
            typeItems.insert(type, typeItem);
        } else {
            typeItem = typeItems.value(type);
        }

        QTreeWidgetItem *subTypeItem = nullptr;
        QMap<QString, QTreeWidgetItem*> &subMap = subTypeItems[type];
        if (!subMap.contains(subType)) {
            subTypeItem = new QTreeWidgetItem(typeItem);
            subTypeItem->setText(0, subType);
            subTypeItem->setIcon(0, folderIcon);
            subMap.insert(subType, subTypeItem);
        } else {
            subTypeItem = subMap.value(subType);
        }

        QTreeWidgetItem *algItem = new QTreeWidgetItem(subTypeItem);
        algItem->setText(0, title);
        algItem->setIcon(0, fileIcon);
        algItem->setData(0, Qt::UserRole, info.absoluteFilePath());
    }

    algoritmTree->expandAll();
}
