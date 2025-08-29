#ifndef ADDMODELOBJECTSDIALOG_H
#define ADDMODELOBJECTSDIALOG_H

#include <QDialog>
#include "../services/models/DataStorageItem.h"
#include "../services/DataStorageServiceFactory.h"

namespace Ui {
class AddModelObjectsDialog;
}

class AddModelObjectsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddModelObjectsDialog(QString title, QString id, QStringList types, QStringList sub_types, QStringList allIds, bool is_file = false,
                                   bool isType = true, bool isSubType = true, QString subType = nullptr, bool is_function = false,
                                   QWidget *parent = nullptr);
    ~AddModelObjectsDialog();

private:
    Ui::AddModelObjectsDialog *ui;
signals:
    void addNewModelObject(QString title, QString id, QString type, QString sub_type, QString function);
};

#endif // ADDMODELOBJECTSDIALOG_H
