#ifndef SCENARIOCLIENT_DSCENARIODISTRICT_H
#define SCENARIOCLIENT_DSCENARIODISTRICT_H

#include <QDialog>


QT_BEGIN_NAMESPACE
namespace Ui { class DScenarioDistrict; }
QT_END_NAMESPACE

class DScenarioDistrict : public QDialog {
Q_OBJECT

public:
    explicit DScenarioDistrict(QWidget *parent = nullptr);

    QJsonObject getCurrentDistrict();

    ~DScenarioDistrict() override;

private slots:
    void createScenario();

private:
    Ui::DScenarioDistrict *ui;

    void loadUI();
};


#endif //SCENARIOCLIENT_DSCENARIODISTRICT_H
