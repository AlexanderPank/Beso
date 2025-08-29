#include "texteditdialog.h"
#include "ui_texteditdialog.h"

TextEditDialog::TextEditDialog(QString title, QString text, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TextEditDialog)
{
    ui->setupUi(this);
    setWindowTitle("Редактирование параметра \""+title+"\"");
    ui->textEdit->setText(text);

    connect(ui->buttonBox,&QDialogButtonBox::accepted,this,[=](){
        QString m_text = ui->textEdit->toPlainText().replace("\n"," ");
        emit textChanged(m_text);
        close();
    });
}

TextEditDialog::~TextEditDialog()
{
    delete ui;
}
