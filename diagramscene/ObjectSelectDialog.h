#ifndef OBJECTSELECTDIALOG_H
#define OBJECTSELECTDIALOG_H

#include <QDialog>

class QTreeWidget;
class QTreeWidgetItem;

class ObjectSelectDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ObjectSelectDialog(QWidget *parent = nullptr);
    QString selectedId() const;
    QString selectedTitle() const;

private slots:
    void onItemDoubleClicked(QTreeWidgetItem *item, int column);

private:
    void loadObjects();
    QTreeWidget *m_tree;
    QString m_selectedId;
    QString m_selectedTitle;
};

#endif // OBJECTSELECTDIALOG_H
