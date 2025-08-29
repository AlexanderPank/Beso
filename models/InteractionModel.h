#ifndef SCENARIOCLIENTTESTS_INTERACTIONMODEL_H
#define SCENARIOCLIENTTESTS_INTERACTIONMODEL_H


#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QTreeWidgetItem>
#include <QPushButton>

class InteractionModel : public QObject
{
Q_OBJECT

public:
    explicit InteractionModel(QObject *parent = nullptr);

    QString getTitle() const;
    QString getSourceObjectId() const;
    QString getInteractionType() const;
    QString getTriggerCondition() const;
    QString getResponseSetValues() const;
    QString getResponseAction() const;
    QString getId() const;

    void setTitle(const QString &);
    void setSourceObjectId(const QString &);
    void setInteractionType(const QString &);
    void setTriggerCondition(const QString &);
    void setResponseSetValues(const QString &);
    void setResponseAction(const QString &);
    void setId(const QString &);

    QJsonObject toJson() const;
    static InteractionModel* fromJson(const QJsonObject &json, QObject* parent);

    QTreeWidgetItem* getTreeWidgetItem(QTreeWidgetItem* parent = nullptr);

signals:
    void editInteractionModel(InteractionModel*);

private:
    void addEditButton();

    QString m_title;
    QString m_sourceObjectId;
    QString m_interactionType;
    QString m_triggerCondition;
    QString m_responseSetValues;
    QString m_responseAction;
    QString m_id;

    QTreeWidgetItem* treeWidgetItem = nullptr;

    QPushButton *btnEdit = nullptr;
};



#endif //SCENARIOCLIENTTESTS_INTERACTIONMODEL_H
