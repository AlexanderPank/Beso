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
    // Создает тулбокс с деревьями алгоритмов
    explicit ItemToolBox(QWidget *parent = nullptr);
    // Освобождает ресурсы
    ~ItemToolBox();

private:
    // Загружает описания алгоритмов из файлов
    void loadAlgorithms();

    Ui::ItemToolBox *ui;
    QTreeWidget *eventTree = nullptr;
    QTreeWidget *conditionTree = nullptr;
    QTreeWidget *algorithmTree = nullptr;
    QTreeWidget *paramTree = nullptr;
};

#endif // ITEMTOOLBOX_H
