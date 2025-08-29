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
//{ ALGORITM, CONDITION, EVENT,PARAM };
private:
    Ui::ItemToolBox *ui;
    QTreeWidget *eventTree;
    QTreeWidget *conditionTree;
    QTreeWidget *algoritmTree;
    QTreeWidget *paramTree;


};

#endif // ITEMTOOLBOX_H
