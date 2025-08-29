#ifndef SCENARIOCLIENT_ALGORITHMMODEL_H
#define SCENARIOCLIENT_ALGORITHMMODEL_H


#include <QObject>
#include <QString>
#include <QMap>
#include <QJsonObject>
#include <QJsonDocument>
#include "PropertyModel.h"
#include "AlgorithParamModel.h"

class AlgorithmModel : public QObject
{
Q_OBJECT
    Q_PROPERTY(QString id READ getId WRITE setId)
    Q_PROPERTY(QString title READ getTitle WRITE setTitle)
    Q_PROPERTY(QString description READ getDescription WRITE setDescription)
    Q_PROPERTY(QString type READ getType WRITE setType)
    Q_PROPERTY(QString function READ getFunction WRITE setFunction)
    Q_PROPERTY(QString outputSplitter READ getOutputSplitter WRITE setOutputSplitter)
    Q_PROPERTY(QString subType READ getMSubType WRITE setMSubType)

public:
    explicit AlgorithmModel(QObject *parent = nullptr);
    ~AlgorithmModel();

    // Getters
    QString getId() const;
    QString getTitle() const;
    QString getDescription() const;
    QString getType() const;
    QString getFunction() const;
    QList<AlgorithParamModel*> getInputParameters() const;
    QList<AlgorithParamModel*> getOutputParameters() const;
    QString getOutputSplitter() const;

    // Setters
    void setId(const QString &id);
    void setTitle(const QString &title);
    void setDescription(const QString &description);
    void setType(const QString &type);
    void setFunction(const QString &function);

    void addInputParameter(const QString &name, const QString &value, const QString &title);
    void addOutputParameter(const QString &name, const QString &value, const QString &title);
    void setInputParameters(const QList<AlgorithParamModel*> &params);
    void setOutputParameters(const QList<AlgorithParamModel*> &params);
    void setOutputSplitter(const QString &outputSplitter);

    // JSON methods
    QJsonObject toJson() const;
    static AlgorithmModel* fromJson(const QJsonObject &json, QObject* parent);

    // Tree methods
    QTreeWidgetItem* getTreeWidgetItem(QTreeWidgetItem* parent, bool addChild = true);

private:
    QString m_id;
    QString m_title;
    QString m_description;
    QString m_type;
    QString m_function;
    QList<AlgorithParamModel*> m_inputParameters;
    QList<AlgorithParamModel*> m_outputParameters;
    QString m_outputSplitter = " ";
    QTreeWidgetItem *treeWidgetItem = nullptr;
    QString m_subType;
public:
    const QString &getMSubType() const;

    void setMSubType(const QString &mSubType);
};

#endif //SCENARIOCLIENT_ALGORITHMMODEL_H
