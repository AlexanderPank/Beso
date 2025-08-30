#ifndef DOBJECTPROPERTIES_H
#define DOBJECTPROPERTIES_H

#include <QDialog>
#include <QMap>

class PropertyModel;

namespace Ui {
class DObjectProperties;
}

class DObjectProperties : public QDialog
{
    Q_OBJECT
public:
    explicit DObjectProperties(const QString &title,
                               const QMap<QString, PropertyModel*> &properties,
                               QWidget *parent = nullptr);
    ~DObjectProperties();

    void accept() override;

private slots:
    void on_btnToggleParam_clicked();

private:
    Ui::DObjectProperties *ui;
    QMap<QString, PropertyModel*> m_properties;
    QMap<PropertyModel*, QWidget*> m_editors;

    void buildTable();
};

#endif // DOBJECTPROPERTIES_H
