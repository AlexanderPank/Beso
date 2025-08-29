//
// Created by qq on 30.03.25.
//

#ifndef SCENARIOCLIENT_PROPERTYITEMDELEGATE_H
#define SCENARIOCLIENT_PROPERTYITEMDELEGATE_H


#include <QStyledItemDelegate>

class PropertyItemDelegate : public  QStyledItemDelegate {

public:
    PropertyItemDelegate(QObject *parent);
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};


#endif //SCENARIOCLIENT_PROPERTYITEMDELEGATE_H
