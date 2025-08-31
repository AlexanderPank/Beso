#ifndef ITEMTOOLBOX_H
#define ITEMTOOLBOX_H

#include <QToolBox>
#include <QTreeWidget>

namespace Ui {
class ItemToolBox;
}

class ItemToolBox : public QToolBox
{
    Q_OBJECT

public:
    explicit ItemToolBox(QWidget *parent = nullptr);
    ~ItemToolBox();

private:
    void loadAlgorithms();

    Ui::ItemToolBox *ui;
    QTreeWidget *eventTree = nullptr;
    QTreeWidget *conditionTree = nullptr;
    QTreeWidget *algoritmTree = nullptr;
    QTreeWidget *paramTree = nullptr;
};

#endif // ITEMTOOLBOX_H
