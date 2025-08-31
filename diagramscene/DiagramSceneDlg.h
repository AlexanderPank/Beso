#ifndef DIAGRAMSCENEDLG_H
#define DIAGRAMSCENEDLG_H

#include "diagramitem.h"
#include "algorithmitem.h"

#include <QMainWindow>

class DiagramScene;

QT_BEGIN_NAMESPACE
class QAction;
class QToolBox;
class QSpinBox;
class QComboBox;
class QFontComboBox;
class QButtonGroup;
class QLineEdit;
class QGraphicsTextItem;
class QFont;
class QToolButton;
class QAbstractButton;
class QGraphicsView;
QT_END_NAMESPACE

class DiagramSceneDlg : public QMainWindow
{
    Q_OBJECT

public:
   // Создает диалоговое окно редактирования диаграммы
   DiagramSceneDlg();

private slots:
   // Обрабатывает выбор фона сцены
   void backgroundButtonGroupClicked(QAbstractButton *button);
   // Обрабатывает выбор типа элемента
   void buttonGroupClicked(QAbstractButton *button);
   // Удаляет выделенный элемент
   void deleteItem();
   // Переключает режим указателя
   void pointerGroupClicked();
   // Поднимает элемент на передний план
   void bringToFront();
   // Отправляет элемент на задний план
   void sendToBack();
   // Обрабатывает вставку текстового элемента
   void textInserted(QGraphicsTextItem *item);
   // Изменяет текущий шрифт
   void currentFontChanged(const QFont &font);
   // Изменяет размер шрифта
   void fontSizeChanged(const QString &size);
   // Изменяет масштаб сцены
   void sceneScaleChanged(const QString &scale);
   // Меняет цвет текста
   void textColorChanged();
   // Меняет цвет элементов
   void itemColorChanged();
   // Меняет цвет линий
   void lineColorChanged();
   // Вставляет текстовый элемент
   void textButtonTriggered();
   // Заливка элемента
   void fillButtonTriggered();
   // Вставка линий
   void lineButtonTriggered();
   // Обработка смены шрифта
   void handleFontChange();
   // Обработка выбора элемента
   void itemSelected(QGraphicsItem *item);
   // Изменяет масштаб сцены
   void changeZoom(int);
   // Отображает информацию о программе
   void about();
   // Включает режим одиночной линии
   void singleLineButtonClicked();
   // Обработка вставки линии
   void handleLineInserted();

   // Сигнал о вставке нового алгоритмического элемента
   void itemInserted(AlgorithmItem *item);
   // Открывает диалог выбора объекта
   void openObjectSelectDialog();
   // Открывает настройки фона
   void openBackgroundSettings();
   // Открывает свойства элемента
   void openItemProperties();
   void saveToJson();
   void loadFromJson();
private:
    void createToolBox();
    void createActions();
    void createMenus();
    void createToolbars();
    QWidget *createBackgroundCellWidget(const QString &text,
                                        const QString &image);
    QWidget *createCellWidget(const QString &text,
                              DiagramItem::DiagramType type);
    QMenu *createColorMenu(const char *slot, QColor defaultColor);
    QIcon createColorToolButtonIcon(const QString &image, QColor color);
    QIcon createColorIcon(QColor color);

    DiagramScene *scene;
    QGraphicsView *view;

    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *exitAction;
    QAction *addAction;
    QAction *backgroundAction;
    QAction *deleteAction;
    QAction *propertiesAction;

    QAction *toFrontAction;
    QAction *sendBackAction;
    QAction *aboutAction;

    QMenu *fileMenu;
    QMenu *modelMenu;
    QMenu *itemMenu;
    QMenu *settingsMenu;

    QToolBar *textToolBar;
    QToolBar *editToolBar;
    QToolBar *colorToolBar;
    QToolBar *pointerToolbar;

    QComboBox *sceneScaleCombo;
    QComboBox *itemColorCombo;
    QComboBox *textColorCombo;
    QComboBox *fontSizeCombo;
    QFontComboBox *fontCombo;

    QToolBox *toolBox;
    QButtonGroup *buttonGroup;
    QButtonGroup *pointerTypeGroup;
    QButtonGroup *backgroundButtonGroup = nullptr;
    QToolButton *fontColorToolButton;
    QToolButton *fillColorToolButton;
    QToolButton *lineColorToolButton;
    QAction *boldAction;
    QAction *underlineAction;
    QAction *italicAction;
    QAction *textAction;
    QAction *fillAction;
    QAction *lineAction;
    bool m_singleLineMode = false;
};

#endif // DIAGRAMSCENEDLG_H
