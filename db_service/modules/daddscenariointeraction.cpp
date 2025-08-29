// You may need to build the project (run Qt uic code generator) to get "ui_DAddScenarioInteraction.h" resolved

#include "daddscenariointeraction.h"

#include <QMessageBox>
#include <QTextEdit>

#include "../services/DataStorageServiceFactory.h"
#include "../../utils/ParametersChecker.h"

DAddScenarioInteraction::DAddScenarioInteraction(bool isEdit, QWidget *parent) :
        QDialog(parent), ui(new Ui::DAddScenarioInteraction), m_isEdit(isEdit) {
    ui->setupUi(this);

    ui->btnNext->setVisible(!m_isEdit);
    ui->btnPrior->setVisible(false);
    ui->btnAdd->setVisible(false);
    ui->btnUpdate->setVisible(m_isEdit);
    ui->lwObjects->setEnabled(!m_isEdit);
    ui->lwObjects->setEnabled(!m_isEdit);

    m_currentObject = nullptr;

    ui->btnShowInteractionScript->setVisible(!m_isEdit);
    ui->gbParameters->setVisible(m_isEdit);

    if(!m_isEdit) {
        this->setWindowTitle("Добавление шаблона взаимодействия");
        ui->swInteraction->setCurrentIndex(0);
        connect(ui->btnNext, &QPushButton::clicked, [this]() {
            ui->swInteraction->setCurrentIndex(1);
            ui->btnNext->setVisible(false);
            ui->btnPrior->setVisible(true);
            ui->btnAdd->setVisible(true);
            updateCurrentTemplate();
        });

        connect(ui->btnPrior, &QPushButton::clicked, [this]() {
            ui->swInteraction->setCurrentIndex(0);
            ui->btnPrior->setVisible(false);
            ui->btnNext->setVisible(true);
            ui->btnAdd->setVisible(false);
        });

        connect(ui->btnAdd, &QPushButton::clicked, this, &DAddScenarioInteraction::bntAdd_Clicked);

        connect(ui->btnShowInteractionScript, &QPushButton::clicked, [this]{
           ui->gbParameters->setVisible(ui->btnShowInteractionScript->isChecked());
           if(ui->btnShowInteractionScript->isChecked())
               ui->btnShowInteractionScript->setText("Скрыть параметры");
           else
               ui->btnShowInteractionScript->setText("Показать параметры");
        });

    } else {
        this->setWindowTitle("Редактирование взаимодействия");
        connect(ui->btnUpdate, &QPushButton::clicked, this, &DAddScenarioInteraction::btnUpdate_Clicked);
        ui->swInteraction->setCurrentIndex(1);
    }
    connect(ui->btnCancel, &QPushButton::clicked, this, &DAddScenarioInteraction::reject);

    ui->lwObjects->setSpacing(3);

    loadUi();
}

DAddScenarioInteraction::~DAddScenarioInteraction() {
    delete ui;
}

void DAddScenarioInteraction::loadUi() {
    ui->lwInteractions->setSpacing(0);

    DataStorageServiceFactory::getInstance()->loadData();
    ui->lwInteractions->clear();
    for(DataStorageItem* item: DataStorageServiceFactory::getInstance()->getElements(TypeElement::teINTERACTION)) {
        InteractionModelTemplate* itemTemplate = InteractionModelTemplate::fromJson(item->getData(), this);
        bool isAdd = true;
        if(m_currentObject) {
            isAdd = false;
            QString iType = "*";
            QString iSubType = "*";
            if(itemTemplate->getTypeObject() == "*") {
                isAdd = true;
            } else {
                QStringList itemTypeSplit = itemTemplate->getTypeObject().split(":");
                iType = itemTypeSplit.at(0);
                if (itemTypeSplit.size() > 1)
                    iSubType = itemTypeSplit.at(1);

                QString typeObject = m_currentObject->getPropertyString("type");
                QString subTypeObject = m_currentObject->getPropertyString("subType");

                if (iType == typeObject && (iSubType == "*" || iSubType == subTypeObject)) {
                    isAdd = true;
                }
            }
        }
        if(isAdd) {
            ui->lwInteractions->addItem(itemTemplate->getListWidgetItem(ui->lwInteractions));
            m_templates.append(itemTemplate);
        }
    }
}

void DAddScenarioInteraction::updateCurrentTemplate() {
    if(currentInteractionModelTemplate) {
        disconnect(currentInteractionModelTemplate);
        disconnect(ui->cbInteractions, SIGNAL(currentIndexChanged(int)), this, SLOT(updateCurrentInteractionModel(int)));
    }
    ui->lwObjects->clear();
    ui->cbInteractions->clear();
    currentInteractionModelTemplate = ui->lwInteractions->currentItem()->data(Qt::UserRole).value<InteractionModelTemplate*>();
    connect(currentInteractionModelTemplate, &InteractionModelTemplate::updateTemplateData, this, &DAddScenarioInteraction::updateParameter);
    ui->gbFillingInteraction->setTitle(currentInteractionModelTemplate->getTitle());
    currentInteractionModelTemplate->clear();
    for(InteractionTemplateItem* templateItem: currentInteractionModelTemplate->getItems()) {
        templateItem->getListWidgetItem(m_objects, m_features, ui->lwObjects);
    }

    if(currentInteractionModelTemplate->getCurrentInteractionModels().size() > 0) {
        m_editInteractionModel = currentInteractionModelTemplate->getCurrentInteractionModels()[0];

        for (InteractionModel *interactionModel: currentInteractionModelTemplate->getCurrentInteractionModels()) {
            ui->cbInteractions->addItem(interactionModel->getTitle(),
                                        QVariant::fromValue(const_cast<InteractionModel *>(interactionModel)));
        }

        if (ui->cbInteractions->count() > 0) {
            ui->cbInteractions->setCurrentIndex(0);
        }

        updateParameter();

        connect(ui->cbInteractions, SIGNAL(currentIndexChanged(int)), this, SLOT(updateCurrentInteractionModel(int)));
    } else {
        m_editInteractionModel = new InteractionModel(this);
    }

    if(m_currentObject && currentInteractionModelTemplate->getItems().size() > 0) {
        currentInteractionModelTemplate->getItems().at(0)->setCurrentObject(m_currentObject);
    }
}

void DAddScenarioInteraction::updateTemplateParameter() {
    if (!m_editInteractionModel) return;
    m_editInteractionModel->setResponseAction(ui->tEdtResponseAction->toPlainText());
    m_editInteractionModel->setResponseSetValues(ui->tEdtResponseSetValues->toPlainText());
    m_editInteractionModel->setTriggerCondition(ui->tEdtTriggerCondition->toPlainText());
    m_editInteractionModel->setInteractionType(ui->edtInteractionType->text());
    m_editInteractionModel->setSourceObjectId(ui->edtSourceObjectId->text());
}

void DAddScenarioInteraction::updateParameter() {
    if (!m_editInteractionModel) return;

    ui->tEdtResponseAction->setText(m_editInteractionModel->getResponseAction());
    ui->tEdtResponseSetValues->setText(m_editInteractionModel->getResponseSetValues());
    ui->tEdtTriggerCondition->setText(m_editInteractionModel->getTriggerCondition());
    ui->edtSourceObjectId->setText(m_editInteractionModel->getSourceObjectId());
    ui->edtInteractionType->setText(m_editInteractionModel->getInteractionType());
}

void DAddScenarioInteraction::changeTemplate() {

}

void DAddScenarioInteraction::bntAdd_Clicked() {
    updateTemplateParameter();
    QStringList listError;
    QStringList listErrorParameters;
    ObjectScenarioModel *selectObject = nullptr;
    for(InteractionTemplateItem* item: currentInteractionModelTemplate->getItems()) {
        if(item->getValue().count()==0) {
            listError.append("Не выбран объект: " + item->getName());
        } else {
            if(item->getValue().contains("properties")) {
                selectObject = ObjectScenarioModel::fromJson(item->getValue(), this);
            } else {
                selectObject = nullptr;
            }

            QMap<QString, QString> parameters;
            for(InteractionModel* interactionModel: currentInteractionModelTemplate->getCurrentInteractionModels()) {
                ParametersChecker::getParametersFromString(item->getValue().value("id").toString(),
                                                           interactionModel->getResponseAction().replace("\n", ""),
                                                           parameters);
                ParametersChecker::getParametersFromString(item->getValue().value("id").toString(),
                                                           interactionModel->getResponseSetValues().replace("\n", ""),
                                                           parameters);
                ParametersChecker::getParametersFromString(item->getValue().value("id").toString(),
                                                           interactionModel->getTriggerCondition().replace("\n", ""),
                                                           parameters);
            }
            for(auto key: parameters.keys()) {
                if(selectObject) {
                    if (!selectObject->containsProperty(key)) {
                        listErrorParameters.append(QString("Свойства объекта не существует: %1").arg(parameters[key]));
                    }
                } else {
                    if(!item->getValue().contains(key)){
                        listErrorParameters.append(QString("Свойства знака не существует: %1").arg(parameters[key]));
                    }
                }
            }
        }
    }

    if(listError.count() > 0 || listErrorParameters.count() > 0) {
        QString msg = listError.join("\n");
        if(!msg.isEmpty())
            msg += "\n";
        msg += listErrorParameters.join("\n");

        ParametersChecker::showMessage(msg);
        return;
    }

    currentInteractionModelTemplate->currentToLocal();

    this->accept();
}

void DAddScenarioInteraction::btnUpdate_Clicked() {
    m_editInteractionModel->setInteractionType(ui->edtInteractionType->text());
    m_editInteractionModel->setSourceObjectId(ui->edtSourceObjectId->text());
    m_editInteractionModel->setResponseAction(ui->tEdtResponseAction->toPlainText().replace("\n",""));
    m_editInteractionModel->setResponseSetValues(ui->tEdtResponseSetValues->toPlainText().replace("\n",""));
    m_editInteractionModel->setTriggerCondition(ui->tEdtTriggerCondition->toPlainText().replace("\n",""));
    this->accept();
}

void DAddScenarioInteraction::setEditInteracionModel(InteractionModel &interactionModel) {
    m_editInteractionModel = &interactionModel;
    ui->gbFillingInteraction->setTitle(m_editInteractionModel->getSourceObjectId() + " - " + m_editInteractionModel->getInteractionType());
    ui->edtSourceObjectId->setText(m_editInteractionModel->getSourceObjectId());
    ui->edtInteractionType->setText(m_editInteractionModel->getInteractionType());
    ui->tEdtResponseAction->setText(m_editInteractionModel->getResponseAction());
    ui->tEdtResponseSetValues->setText(m_editInteractionModel->getResponseSetValues());
    ui->tEdtTriggerCondition->setText(m_editInteractionModel->getTriggerCondition());

    ui->cbInteractions->addItem(m_editInteractionModel->getTitle());
    ui->cbInteractions->setCurrentIndex(0);
}

void DAddScenarioInteraction::updateCurrentInteractionModel(int index) {
    if(index == -1)
        return;

    updateTemplateParameter();
    m_editInteractionModel = currentInteractionModelTemplate->getCurrentInteractionModels()[index];
    updateParameter();
}

void DAddScenarioInteraction::setCurrentObject(ObjectScenarioModel *currentObject) {
    m_currentObject = currentObject;
    loadUi();
}
