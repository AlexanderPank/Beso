//
// Created by qq on 02.04.25.
//

#ifndef SCENARIOCLIENT_MAPINFORMER_H
#define SCENARIOCLIENT_MAPINFORMER_H
#include <QWidget>
#include <QLineEdit>
#include <QGridLayout>


class MapInformer: public QWidget
{
Q_OBJECT

public:
    explicit MapInformer(QWidget *parent);
    void setText(const QString& QString);
    void updatePosition();
    void hideLabel();

private:
    QGridLayout *layout;
    QLineEdit* lineEdit;
    QWidget* m_parent;
};


#endif //SCENARIOCLIENT_MAPINFORMER_H
