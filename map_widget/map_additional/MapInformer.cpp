#include "MapInformer.h"

MapInformer::MapInformer(QWidget *parent):QWidget(parent),m_parent(parent) {
    this->setGeometry(0,0, 160, 50);
    this->setContentsMargins(0,0,0,0);
    this->setWindowTitle("Информер для карты");
    layout = new QGridLayout(this);
    layout->setContentsMargins(0,0,0,0);
    lineEdit = new QLineEdit();
    lineEdit->setReadOnly(true);
    lineEdit->setEnabled(true);
    lineEdit->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);
    layout->addWidget(lineEdit, 0, 0, 1, 1);

    updatePosition();
}


void MapInformer::setText(const QString& txt){
    lineEdit->setText(txt);
}


void MapInformer::updatePosition(){
    // Установка геометрии окна
    if (!m_parent) return;

    int parentWidth = m_parent->width();
    int panelWidth = this->width();
    this->setGeometry(m_parent->width()/2 -  this->width()/2,
                      m_parent->height() - 1.5 * this->height(),
                      this->width(),
                      this->height()
    );
}

void MapInformer::hideLabel(){
    hide();
    lineEdit->setText("");
}