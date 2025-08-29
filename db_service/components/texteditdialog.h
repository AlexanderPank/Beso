#ifndef TEXTEDITDIALOG_H
#define TEXTEDITDIALOG_H

#include <QDialog>

namespace Ui {
class TextEditDialog;
}

class TextEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TextEditDialog(QString title = "", QString text = "", QWidget *parent = nullptr);
    ~TextEditDialog();

private:
    Ui::TextEditDialog *ui;
signals:
    void textChanged(QString txt);
};

#endif // TEXTEDITDIALOG_H
