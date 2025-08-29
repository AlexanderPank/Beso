#include "PropertyItemDelegate.h"
#include "../models/PropertyModel.h"
#include <QLineEdit>

PropertyItemDelegate::PropertyItemDelegate(QObject *parent)
        : QStyledItemDelegate(parent) {}

QWidget* PropertyItemDelegate::createEditor(QWidget *parent,
                                            const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const
{
    Q_UNUSED(option)
    if (index.column() == 1) { // Разрешаем редактирование только второго столбца
        auto *editor = new QLineEdit(parent);
        return editor;
    }
    return nullptr;
}

void PropertyItemDelegate::setModelData(QWidget *editor,
                                        QAbstractItemModel *model,
                                        const QModelIndex &index) const
{
    if (index.column() == 1) {
        auto *lineEdit = qobject_cast<QLineEdit*>(editor);
        if (lineEdit) {
            model->setData(index, lineEdit->text(), Qt::EditRole);

            // Получаем PropertyModel из UserRole
            QVariant propData = index.data(Qt::UserRole);
            if (propData.canConvert<PropertyModel*>()) {
                auto *prop = propData.value<PropertyModel*>();
                if (prop) {
                    prop->setValue(lineEdit->text());
                }
            }
        }
    } else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}
