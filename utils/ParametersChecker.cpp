#include "ParametersChecker.h"

#include <QMessageBox>
#include <QSpacerItem>
#include <QGridLayout>

void ParametersChecker::getParametersFromString(QString idObject, const QString &text, QMap<QString, QString> &parameters) {
    QString textFind = QString("%1.properties").arg(idObject);
    QStringList splitList = text.split(textFind);
    if(splitList.size() > 0)
        splitList.removeFirst();
    for(auto propString: splitList) {
        QStringList splitProp = propString.split("'");
        QString key = splitProp.size()>=2?splitProp[1]:"";
        if(key.isEmpty() || key == "geo_path" || key == "geo_area")
            continue;

        parameters.insert(key, QString("%1['%2']").arg(textFind, key));
    }
}

void ParametersChecker::showMessage(QString msg) {
    QMessageBox msgBox;
    msgBox.setWindowTitle("Ошибка заполнения");
    msgBox.setIcon(QMessageBox::Information);
    QSpacerItem* horizontalSpacer = new QSpacerItem(800, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    msgBox.setText(msg);
    QGridLayout* layout = (QGridLayout*)msgBox.layout();
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());
    msgBox.exec();
}
