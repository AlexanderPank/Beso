#include "addmodelobjectsdialog.h"
#include "ui_addmodelobjectsdialog.h"

#include <QDialogButtonBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QValidator>
#include <QRegExpValidator>

AddModelObjectsDialog::AddModelObjectsDialog(QString title, QString id, QStringList types, QStringList sub_types, QStringList allIds, bool is_file, bool isType, bool isSubType, QString subType, bool is_function, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddModelObjectsDialog)
{
    ui->setupUi(this);
    ui->title_lineEdit->setText(title);
    ui->id_lineEdit->setText(id);

    ui->id_lineEdit->setValidator(new QRegExpValidator(QRegExp("[^-][A-Za-z0-9_]*")));

    ui->label_5->setVisible(false);
    ui->lineEdit_function->setVisible(false);
    this->setWindowTitle("Добавление");
    ui->type_comboBox->insertItems(0,types);
    connect(ui->type_comboBox,&QComboBox::currentTextChanged,this,[=](QString text){
        if (text=="Cистемные алгоритмы" && is_function){
            ui->label_5->setVisible(true);
            ui->lineEdit_function->setVisible(true);
        }else{
            ui->label_5->setVisible(false);
            ui->lineEdit_function->setVisible(false);
        }
    });
    ui->subType_comboBox->insertItems(0,sub_types);
    if (subType!= nullptr)
        ui->subType_comboBox->setCurrentText(subType);
    ui->pushButton->setVisible(is_file);

    ui->label_3->setVisible(isType);
    ui->type_comboBox->setVisible(isType);

    ui->label_4->setVisible(isSubType);
    ui->subType_comboBox->setVisible(isSubType);



    connect(ui->pushButton_Cansel,&QPushButton::clicked,this,[=](){
        close();
    });

    connect(ui->pushButton_OK,&QPushButton::clicked,this,[=](){
        QString m_title = ui->title_lineEdit->text().trimmed();
        QString m_id = ui->id_lineEdit->text().trimmed();
        QString m_type = ui->type_comboBox->currentText();
        QString m_sub_type = ui->subType_comboBox->currentText();
        QString m_function = ui->lineEdit_function->text().trimmed();
        if (allIds.contains(m_id)){
            QMessageBox::warning(this,"Дублирование ID","Идентификатор должен быть уникальным");
            return ;
        }
        if (m_type.isEmpty() || m_id.isEmpty() || m_title.isEmpty()){
            QMessageBox::information(this,"Внимание!","Не заполнены обязательные поля");
            return ;
        }
        if (m_sub_type.isEmpty()){
            m_sub_type = nullptr;
        }

        emit addNewModelObject(m_title,m_id,m_type,m_sub_type,m_function);
        close();
    });
    connect(ui->pushButton,&QPushButton::clicked,this,[=](){
        QString text = QFileDialog::getOpenFileName(this, tr("Выбирите файл библиотеки"),
                                                            "/home/qq/",
                                                            tr("Lib Files (*.so*)"));
        if (!text.isEmpty())
            text = text.split("/").last();
        ui->title_lineEdit->setText(text);
    });
}

AddModelObjectsDialog::~AddModelObjectsDialog()
{
    delete ui;
}
