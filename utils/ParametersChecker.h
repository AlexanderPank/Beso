#ifndef SCENARIOCLIENT_PARAMETERSCHECKER_H
#define SCENARIOCLIENT_PARAMETERSCHECKER_H

#include <QString>
#include <QMap>

class ParametersChecker {

public:
    static void getParametersFromString(QString idObject, const QString &text, QMap<QString, QString> &parameters);
    static void parametersToStringList(QMap<QString, QString> parameters, QStringList &errorParameters);
    static void showMessage(QString msg);

};


#endif //SCENARIOCLIENT_PARAMETERSCHECKER_H
