#include "ObjectSelectDialog.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QStyle>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>

#include "../db_service/services/FileDataStorageService.h"

ObjectSelectDialog::ObjectSelectDialog(QWidget *parent)
    : QDialog(parent), m_tree(new QTreeWidget(this))
{
    setWindowTitle(tr("Выбор объекта"));
    resize(400, 300);

    m_tree->setHeaderHidden(true);
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(m_tree);

    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Выбрать"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Отменить"));
    layout->addWidget(buttonBox);

    loadObjects();

    connect(m_tree, &QTreeWidget::itemDoubleClicked,
            this, &ObjectSelectDialog::onItemDoubleClicked);
    connect(buttonBox, &QDialogButtonBox::accepted, [this]() {
        QTreeWidgetItem *item = m_tree->currentItem();
        if (item && item->childCount() == 0) {
            m_selectedId = item->data(0, Qt::UserRole).toString();
            m_selectedTitle = item->text(0);
            accept();
        }
    });
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

QString ObjectSelectDialog::selectedId() const
{
    return m_selectedId;
}

QString ObjectSelectDialog::selectedTitle() const
{
    return m_selectedTitle;
}

void ObjectSelectDialog::loadObjects()
{
    QDir dir(QString(MAIN_DIR_DEFAULT) + SUB_DIR_OBJECTS);
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

        QString id = obj["id"].toString();
        QString title = obj["title"].toString(info.completeBaseName());

        QString type;
        QString subType;
        QJsonArray props = obj["properties"].toArray();
        for (const QJsonValue &val : props) {
            QJsonObject prop = val.toObject();
            if (prop["name"].toString() == "type")
                type = prop["value"].toString();
            else if (prop["name"].toString() == "subType")
                subType = prop["value"].toString();
        }

        QTreeWidgetItem *typeItem = nullptr;
        if (!typeItems.contains(type)) {
            typeItem = new QTreeWidgetItem(m_tree);
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

        QTreeWidgetItem *objItem = new QTreeWidgetItem(subTypeItem);
        objItem->setText(0, title);
        objItem->setIcon(0, fileIcon);
        objItem->setData(0, Qt::UserRole, id);
    }

    m_tree->expandAll();
}

void ObjectSelectDialog::onItemDoubleClicked(QTreeWidgetItem *item, int)
{
    if (item && item->childCount() == 0) {
        m_selectedId = item->data(0, Qt::UserRole).toString();
        m_selectedTitle = item->text(0);
        accept();
    }
}
