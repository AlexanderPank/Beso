#ifndef MAP_TOOL_PANEL_H
#define MAP_TOOL_PANEL_H

#include <QWidget>

namespace Ui { class MapToolPanel;}


class MapToolPanel : public QWidget
{
    Q_OBJECT

public:
    explicit MapToolPanel(QWidget *parent);
    ~MapToolPanel();

    void setSignSelected(bool);
    void updatePosition();
    bool isRouletteOn();
    void clearState();
    bool isSignLabelsVisible() const;
    bool isFeatureLabelsVisible() const;

signals:
    void rouletteClicked();
    void propertySignClicked();
    void addPointClicked();
    void removePointClicked();
    void changeSignClicked();
    void rotateSignClicked();
    void removeSignClicked();
    void multySelectClicked();
    void signLabelsToggled(bool);
    void featureLabelsToggled(bool);

private:
    Ui::MapToolPanel *ui;
    enum  ButtonState{
        NONE = 0,
        ROULETTE = 1,
        PROPERTY = 2,
        ADD_POINT=3,
        REMOVE_POINT = 4,
        CHANGE_SIGN = 5,
        ROTATE_SIGN = 6,
        REMOVE_SIGN = 7,
        MULTI_SELECT = 8
    };

    ButtonState _state = ButtonState::NONE;
    void updateButtonView(ButtonState state);
    QWidget *_parent;
};


#endif // MAP_TOOL_PANEL_H
