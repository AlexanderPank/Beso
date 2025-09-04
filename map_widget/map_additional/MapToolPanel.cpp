#include <QGraphicsDropShadowEffect>
#include "MapToolPanel.h"
#include "ui_map_tool_panel.h"
#include "../../core/QLogStream.h"

MapToolPanel::MapToolPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MapToolPanel),
    _parent(parent)
{
    ui->setupUi(this);

    connect(ui->deleteButton,&QPushButton::released, this, [=](){
        emit removeSignClicked();
        updateButtonView(ButtonState::NONE);
    });

    connect(ui->changeButton,&QPushButton::released, this, [=](){
        emit changeSignClicked();
        updateButtonView(_state == ButtonState::CHANGE_SIGN ? ButtonState::NONE : ButtonState::CHANGE_SIGN);
    });

    connect(ui->addPointButton,&QPushButton::released, this, [=](){
        emit addPointClicked();
        updateButtonView(_state == ButtonState::ADD_POINT ? ButtonState::NONE : ButtonState::ADD_POINT);
    });

    connect(ui->removePointButton,&QPushButton::released, this, [=](){
        emit removePointClicked();
        updateButtonView(_state == ButtonState::REMOVE_POINT ? ButtonState::NONE : ButtonState::REMOVE_POINT);
    });

    connect(ui->propetiesButton,&QPushButton::released, this, [=](){
        emit propertySignClicked();
        updateButtonView(_state == ButtonState::PROPERTY? ButtonState::NONE : ButtonState::PROPERTY);
    });

    connect(ui->rulerButton,&QPushButton::released, this, [=](){
        emit rouletteClicked();
        updateButtonView(_state == ButtonState::ROULETTE ? ButtonState::NONE : ButtonState::ROULETTE);
    });

    connect(ui->rotateButton,&QPushButton::released, this, [=](){
        emit rotateSignClicked();
        updateButtonView(_state == ButtonState::ROTATE_SIGN ? ButtonState::NONE : ButtonState::ROTATE_SIGN);
    });

    connect(ui->multySelectButton,&QPushButton::released, this, [=](){
        emit multySelectClicked();
        updateButtonView(_state == ButtonState::MULTI_SELECT ? ButtonState::NONE : ButtonState::MULTI_SELECT);
    });

    connect(ui->signLabelsButton, &QPushButton::toggled, this, &MapToolPanel::signLabelsToggled);
    connect(ui->featureLabelsButton, &QPushButton::toggled, this, &MapToolPanel::featureLabelsToggled);
    connect(ui->layerListButton, &QPushButton::toggled, this, [this](bool checked){
        emit showLayerList(checked);
    });

    this->setMaximumWidth(40);
    this->setFixedWidth(40);

    // Установка геометрии окна
    updatePosition();

    // Установка стиля
    this->setStyleSheet(R"(
        background-color: white; /* Белый фон */
        border-radius: 4px; /* Скругленные углы */
        border: none; /* Без границы */
    )");

    // Создание эффекта тени (дополнительно)
    auto shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(15);
    shadow->setColor("#333333");
    shadow->setOffset(3, 3); // Смещение тени
    this->setGraphicsEffect(shadow);

    setSignSelected(false);
}



MapToolPanel::~MapToolPanel()
{
    delete ui;
}

void MapToolPanel::updateButtonView(ButtonState state) {

    if (_state == ButtonState::CHANGE_SIGN && state == ButtonState::NONE) ui->changeButton->setIcon( QIcon(":/icons/images/sign_change.jpg"));
    if (_state == ButtonState::ROTATE_SIGN && state == ButtonState::NONE) ui->rotateButton->setIcon( QIcon(":/icons/images/sign_rotate.jpg"));
    if (_state == ButtonState::ROULETTE && state == ButtonState::NONE) ui->rulerButton->setIcon( QIcon(":/icons/images/sign_rotate.jpg"));
    if (state == ButtonState::ROULETTE) ui->rulerButton->setIcon( QIcon(":/icons/images/roulette_active.jpg"));
    else ui->rulerButton->setIcon( QIcon(":/icons/images/roulette.jpg"));

    _state = state;
    ui->rulerButton->setEnabled(_state == ButtonState::ROULETTE || _state == ButtonState::NONE);
    ui->deleteButton->setEnabled(_state == ButtonState::REMOVE_SIGN);
    ui->removePointButton->setEnabled(_state == ButtonState::REMOVE_POINT);
    ui->addPointButton->setEnabled(_state == ButtonState::ADD_POINT);
    ui->multySelectButton->setEnabled( _state == ButtonState::MULTI_SELECT || _state == ButtonState::NONE);
    ui->changeButton->setEnabled( _state == ButtonState::CHANGE_SIGN);
    ui->rotateButton->setEnabled( _state == ButtonState::ROTATE_SIGN);
    ui->propetiesButton->setEnabled( _state == ButtonState::ROTATE_SIGN);
    if (_state == ButtonState::CHANGE_SIGN) ui->changeButton->setIcon( QIcon(":/icons/images/sign_change_active.jpg"));
    if (_state == ButtonState::ROTATE_SIGN) ui->rotateButton->setIcon( QIcon(":/icons/images/sign_rotate_active.jpg"));
}

void MapToolPanel::setSignSelected(bool signSelected){
    //Если был выделен знак



    if (_state != ButtonState::NONE) return;

    if (!signSelected) {
        updateButtonView(ButtonState::NONE);
        return;
    }

    ui->rulerButton->setEnabled(!signSelected);
    ui->deleteButton->setEnabled(signSelected);
    ui->removePointButton->setEnabled(signSelected);
    ui->addPointButton->setEnabled(signSelected);
    ui->changeButton->setEnabled(signSelected);
    ui->rotateButton->setEnabled(signSelected);
    ui->multySelectButton->setEnabled(!signSelected);
    ui->propetiesButton->setEnabled(signSelected);

}

void MapToolPanel::updatePosition() {
    if (!_parent) return;

    int parentHeight = _parent->height();
    int parentWidth = _parent->width();
    int panelHeight = this->height();
    int panelWidth = this->width();
    int posY = (parentHeight - panelHeight) / 2 - 20;
    int posX =  parentWidth - 60 ; // Центрируем по высоте
    qLog() << posX << " " << posY << " " << panelWidth << " " << parentWidth;

    this->setGeometry(posX, posY, this->width(), this->height());
}

void MapToolPanel::clearState(){
    updateButtonView(ButtonState::NONE);
}
bool MapToolPanel::isRouletteOn(){
    return _state == ButtonState::ROULETTE;
}

bool MapToolPanel::isSignLabelsVisible() const {
    return ui->signLabelsButton->isChecked();
}

bool MapToolPanel::isFeatureLabelsVisible() const {
    return ui->featureLabelsButton->isChecked();
}
