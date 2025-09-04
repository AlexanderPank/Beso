
#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "core/EnvConfig.h"

#include <QFileDialog>
#include <QJsonArray>
#include <QMap>
#include <QIcon>
#include <QSettings>

#include "diagramscene/DiagramSceneDlg.h"

#include "db_service/modules/dscenariodistrict.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/icons/images/appicon.png")); // Для ресурсов

    auto config = EnvConfig::getInstance();
    auto apiEndpoint = config->get("API_ENDPOINT", "");
    auto wsEndpoint = config->get("WS_ENDPOINT", "");

    this->setWindowTitle("Система разработки сценариев и моделирования взаимодействия объектов");

    engineService = std::make_shared<EngineService>(apiEndpoint, this);
    websocketService = std::make_shared<WebSocketService>(wsEndpoint, this);


    state = ModelState::Closed;
    firstStart=true;
    speedValue=1;

    ui->verticalLayout->setSpacing(0);
    ui->verticalLayout->setContentsMargins(0, 0, 0, 0);

    ui->frame->setContentsMargins(0, 0, 0, 0);

    ui->cbSpeed->addItems({"x1", "x2", "x5", "x10", "x20", "x30", "x40", "x50", "x99"});

    QObject::connect(ui->pbOpen, &QPushButton::clicked, [=](){openScenario();} );
    QObject::connect(ui->pbPlay, &QPushButton::clicked, this, &MainWindow::startSimulation);
    QObject::connect(ui->pbPause, &QPushButton::clicked, this, &MainWindow::pauseSimulation);
    QObject::connect(ui->pbStop, &QPushButton::clicked, this, &MainWindow::stopSimulation);
    QObject::connect(ui->pbResume, &QPushButton::clicked, this, &MainWindow::resumeSimulation);
    QObject::connect(ui->pbReload, &QPushButton::clicked, this, &MainWindow::reloadScenario);
    QObject::connect(ui->pb3D, &QPushButton::clicked, this, &MainWindow::set3DMode);
    QObject::connect(ui->cbSpeed, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {
        // Handle the change
        int value =  ui->cbSpeed->itemText(index).replace("x","").toInt();
        changeSpeed(value);
    });

    connect(websocketService.get(), &WebSocketService::simulationStateReceived, this, &MainWindow::handleSimulationStateUpdate);

    // mapWidget = new MapWidget(this, "maps/RU4MDLM0/RU4MDLM0.sit", "maps/violit");
    // mapWidget = new MapWidget(this, "maps/RU5MELN0/RU5MELN0.sit", "maps/violit");
    // mapWidget = new MapWidget(this, "maps/novoross-100-9/L-37-138.map", "maps/violit");
    // mapWidget = new MapWidget(this, "maps/RU2NKK59/RU2NKK59.sit", "maps/violit");
//    mapWidget = new MapWidget(this, "maps/RU4MDLM0/RU4MDLM0.sit", "maps/RU4MDLM0/");
    mapWidget = new MapWidget(this, "maps/Sochi2/K3704.map", "maps/Sochi2/");
 //    mapWidget = new MapWidget(this, "maps/RU4MDLM0-map/RU4MDLM0-s57.mpt", "maps/violit");
    this->ui->mapLayout->addWidget(mapWidget);
    eventLog = new SimulationEventWidgetInMap(mapWidget);

    forceEditor = new ForcesEditor();

    connect(ui->actionShowForceEdit, &QAction::triggered, [=](){
        forceEditor->show();
    });

    scenarioEditor = new ScenarioEditor(this);
    ui->verticalLayout->addWidget(scenarioEditor);
    scenarioEditor->setEventLog(eventLog);

    connect(scenarioEditor, &ScenarioEditor::setMapCenter, mapWidget, &MapWidget::onSetMapCenter);
    connect(scenarioEditor, &ScenarioEditor::selectSignOnMap, mapWidget, &MapWidget::onSelectSignOnMap);
    connect(scenarioEditor, &ScenarioEditor::clearMap, mapWidget, &MapWidget::onClearMap);
    connect(scenarioEditor, &ScenarioEditor::needMapRepaint, mapWidget, &MapWidget::onRepaint);
    connect(ui->wTacticalSigns, &ScenarioTacticalObjects::addTacticalSign, scenarioEditor, &ScenarioEditor::addTacticalSign);
    connect(scenarioEditor, &ScenarioEditor::filterTacticalSign, ui->wTacticalSigns, &ScenarioTacticalObjects::selectTacticalSign);

    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::createScenario);
    connect(ui->actionSaveScenario, &QAction::triggered, [=](){ scenarioEditor->saveScenario(false);});
    connect(ui->actionSaveAsScenario, &QAction::triggered, [=](){ scenarioEditor->saveScenario(true);});
    connect(ui->actionOpen, &QAction::triggered, [=](){
        QString filePath = QFileDialog::getOpenFileName(this, "Open Scenario", "", "JSON Files (*.json)");
        if (filePath.isEmpty()) return;
        this->openScenario(filePath);
    });

    connect(ui->action_diagram, &QAction::triggered, [=](){
        DiagramSceneDlg *dsd = new DiagramSceneDlg();

        dsd->setGeometry(100, 100, 1080, 700);
        dsd->showNormal();
    });

    connect(ui->actionUpdateObjectsFromBD, &QAction::triggered, scenarioEditor, &ScenarioEditor::updateObjectsFromBD);
    connect(ui->actionSetMapCenter, &QAction::triggered, [=](){
        auto center = mapWidget->getMapCenter();
        scenarioEditor->moveAllElements(center);
        qDebug() << "Center: " << center.x() << ", " << center.y();
    });
    this->showMaximized();
    //openScenario("scenario/fox_rabbit_cpp_v4.json");
    //openScenario("../scenario/bpla.json");
    //openScenario("../scenario/fox_rabbit_cpp_v5.json");
    //openScenario("../scenario/bpla_real.json");
    //openScenario("../scenario/target_distribution.json");
    //openScenario("../scenario/модель видимости в инфракрасном диапазоне.json");
    //openScenario("../scenario/радиосвязь между двумя объектами.json");
    //openScenario("../scenario/модель видимости по дальности.json");
    //openScenario("../scenario/Расчет видимости 4.2 и 13.3.json");
    //openScenario("../scenario/маневр уклонения.json");
    //openScenario("../scenario/target_distribution.json");
    //openScenario("../scenario/path_graph.json");
    //openScenario("../scenario/scenario_1.json");
    //openScenario("../scenario/отладка бэк поведенческое ядро.json");
    //openScenario("../scenario/отладка бпла поведенческое ядро.json");
    //openScenario("../scenario/scenario_1_1.json");
    //openScenario("../scenario/Поражение БЭК с помощью Артиллерийского Комплекса.json");
    // openScenario("../scenario/scenario_1_1_ak.json");
    // openScenario("../scenario/Маневр уклонения Корабля от бэк.json");
     // openScenario("../scenario/path_graph_empty.json");
    //  openScenario("../scenario/calcsea/GetVisibilityRadioObj1Obj2 - радио видимость.json");
   //  openScenario("../scenario/calcsea/GetVisibilityObj1Obj2 - видимость одного объекта другим.json");
    // openScenario("../scenario/calcsea/GetVisibilityObj1Obj2Ver2 - видимость одного объекта другим.json");
//this->setWindowTitle("../scenario/scen281024new.json");
    QSettings settings;
    QString lastScenario = settings.value("last_scenario", "").toString();
    if (!lastScenario.isEmpty()) openScenario(lastScenario);

}

MainWindow::~MainWindow(){
    delete ui;
}


void MainWindow::showInfoDialog(const QString &status, const QString &text) {
    QMessageBox msgBox;
    if (status == STATUS_SUCCESS) {
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(" Success");
    } else if (status == STATUS_ERROR) {
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(" Error");
    }
    msgBox.setText(text);
    msgBox.exec();
}

void MainWindow::openScenario(QString fileName) {
    if (state == ModelState::Playing || state == ModelState::Paused) return;
    eventLog->clearEvents();

    if (fileName.isEmpty()) {
        fileName = QFileDialog::getOpenFileName(nullptr, "Выберите сценарий", "", "All Files (*.json)");
        if (fileName.isEmpty()) return;
    }
    firstStart = true;
    currentScenarioFileName = fileName;

    QString map_path = scenarioEditor->getMapPathFromScenarioFile(fileName);


    auto  result = scenarioEditor->loadScenario(fileName, map_path, mapWidget);
    if (result==-1) {showInfoDialog(STATUS_ERROR, "Не удалось загрузить карту"); return;}
    scenarioEditor->show();
    this->setWindowTitle(fileName);
    setModelingProcessState(ModelState::Opened);
}

void MainWindow::setModelingProcessState(ModelState newState) {
    state = newState;
    switch (state) {
        case ModelState::Opened :
            ui->pbOpen->setEnabled(true);
            ui->pbPlay->setEnabled(true);
            ui->pbStop->setEnabled(false);
            ui->pbPause->setEnabled(false);
            ui->pbResume->setEnabled(false);
            ui->pbReload->setEnabled(true);
            ui->pb3D->setEnabled(false);
            break;
         case ModelState::Closed:
            ui->pbOpen->setEnabled(true);
            ui->pbPlay->setEnabled(true);
            ui->pbStop->setEnabled(false);
            ui->pbPause->setEnabled(false);
            ui->pbResume->setEnabled(false);
            ui->pb3D->setEnabled(false);
            break;
        case ModelState::Paused:
           ui->pbOpen->setEnabled(false);
           ui->pbPlay->setEnabled(false);
           ui->pbStop->setEnabled(true);
           ui->pbPause->setEnabled(false);
           ui->pbResume->setEnabled(true);
           break;
        case ModelState::Playing:
           ui->pbOpen->setEnabled(false);
           ui->pbPlay->setEnabled(false);
           ui->pbStop->setEnabled(true);
           ui->pbPause->setEnabled(true);
           ui->pbResume->setEnabled(false);
           ui->pb3D->setEnabled(true);

           break;
        case ModelState::Stopped:
           ui->pbOpen->setEnabled(true);
           ui->pbPlay->setEnabled(true);
           ui->pbStop->setEnabled(false);
           ui->pbPause->setEnabled(false);
           ui->pbResume->setEnabled(false);
            ui->pb3D->setEnabled(false);
           break;
    }

    update3DButtonView();
}

void MainWindow::update3DButtonView() {
    if (!ui->pb3D->isEnabled()) return;
    if (is3DMode)
        ui->pb3D->setStyleSheet("QPushButton#pb3D:enabled{ background:#fdff7f; border:1px solid #eee;}");
    else
        ui->pb3D->setStyleSheet("QPushButton#pb3D:enabled{ background:#eee; border:1px solid #eee;}");
}

void MainWindow::handleSimulationStateUpdate(QJsonDocument data) {

    auto time_elapsed = data["time_elapsed"].toInt();
    if (m_time_elapsed == time_elapsed) return;
    m_time_elapsed = time_elapsed;
    if (m_waiting_scenario_reload) return;

    if (data["status"] == STATE_FINISHED && state != ModelState::Stopped) {
        setModelingProcessState(ModelState::Stopped);
        if (firstStart) return;
        scenarioEditor->updateObjectStateFromModel(data["objects"].toArray(), data["features"].toArray());
        eventLog->updateEventList(data["simulation_events"].toArray());
        showInfoDialog(STATUS_SUCCESS, QString("Simulation ended: %1").arg(data["message"].toString()));
    }

    if (firstStart) {
        // если первый запуск приложения то нужно понять в каком состоянии находится ядро моделирование
        firstStart = false;
        if (data["status"] == STATE_PLAYING) setModelingProcessState(ModelState::Playing);
        else if (data["status"] == STATE_PAUSED) setModelingProcessState(ModelState::Paused);
        //else if (data["status"] == STATE_OPENED) setModelingProcessState(ModelState::Opened);
        else if (data["status"] == STATE_FINISHED || data["status"] == STATE_STOPPED || data["status"] == STATE_OPENED) {
            setModelingProcessState(ModelState::Stopped);
            return;
        }
        scenarioEditor->updateObjectStateFromModel(data["objects"].toArray(), data["features"].toArray());
        eventLog->updateEventList(data["simulation_events"].toArray());
    }

    if (state == ModelState::Playing) {
        scenarioEditor->updateObjectStateFromModel(data["objects"].toArray(), data["features"].toArray());
        eventLog->updateEventList(data["simulation_events"].toArray());
    }
}

bool MainWindow::loadEngineByFile(const QString &filePath) {
    auto response = engineService->loadEngine(filePath);

    if (response["status"] == STATUS_SUCCESS) {
        // добавляем отображение на карту geo_json объекты  если они есть
        return true;
    } else if (response["status"] == STATUS_ERROR && response["code"] == 404) {
        auto arr = response["not_exists_files"].toArray();
        QList<QString> fileList;
        foreach (auto item, arr) fileList.append(item.toString());

        //TODO: тут должна быть вызвана форма загрузки доп файлов
        showInfoDialog(STATUS_ERROR, "Необходимо загрузить дополнительные файлы!");
        return loadAllNeedFiles(filePath, fileList);
    } else if (response["status"] == STATUS_ERROR) {
        showInfoDialog(STATUS_ERROR, response["detail"].toString());
    } else {
        showInfoDialog(STATUS_ERROR, "Ошибка загрузки сценария");
    }
    return false;
}

bool MainWindow::loadAllNeedFiles(const QString &, QList<QString> ) {
    // TODO здесь нужно вызвать форму с выбором и загрузкой фалов чтобы
    // передать в onLoadFiles
    return false;
}

bool MainWindow::onLoadFiles(const QList<UploadFile> &selectedFiles, QString scenarioFilePath) {
    if (selectedFiles.isEmpty()) {return false;}

    auto response = engineService->uploadFiles(selectedFiles);
    if (!response.isEmpty() && response.isArray()) {

        QStringList errorFiles;
        for (const auto &file : response.array()) {
            if (!file.isObject()) continue;
            QJsonObject jobj = file.toObject();
            if (jobj["code"].toInt() != 200) {
                errorFiles.append(QString("Файл %1 не загружен:%2").arg(jobj["file_name"].toString()).arg(jobj["message"].toString()));
            }
        }
        if (!errorFiles.isEmpty()) {
            showInfoDialog(STATUS_ERROR, errorFiles.join("\n"));
            return false;
        }

        if (scenarioFilePath != "") return loadEngineByFile(scenarioFilePath);
    } else if (response["status"] == STATUS_ERROR) {
        showInfoDialog(STATUS_ERROR, response["detail"].toString());
    }
    return false;
}

void MainWindow::startSimulation() {
    ui->cbSpeed->setCurrentIndex(0);
    m_time_elapsed = 0;

    auto tmpFile = scenarioEditor->getTempFileName();
    if(!scenarioEditor->saveScenario(tmpFile))
     {
        showInfoDialog(STATUS_SUCCESS, "Ошибка временный файл сценария не создан!");
        return;
    }
    if (loadEngineByFile(tmpFile)) {
        setModelingProcessState(ModelState::Opened);
    } else return;
    firstStart = false;

    auto response = engineService->startEngine();
    if (response["status"] == STATUS_SUCCESS) {
        showInfoDialog(STATUS_SUCCESS, "Процесс моделирования успешно запущен!");
        setModelingProcessState(ModelState::Playing);
    } else if (response["status"] == STATUS_ERROR) {
        showInfoDialog(STATUS_ERROR, response["detail"].toString());
    }
}

void MainWindow::pauseSimulation() {
    auto response = engineService->pauseEngine();
    if (response["status"] == STATUS_SUCCESS) {
        showInfoDialog(STATUS_SUCCESS, "Процесс моделирования приостановлен!");
        setModelingProcessState(ModelState::Paused);
    } else if (response["status"] == STATUS_ERROR) {
        showInfoDialog(STATUS_ERROR, response["detail"].toString());
    }
}

void MainWindow::reloadScenario() {
    m_waiting_scenario_reload = true;
    if (state == ModelState::Playing || state == ModelState::Paused) {
        stopSimulation();
    }


    if (currentScenarioFileName != "")
        openScenario(currentScenarioFileName);
    m_waiting_scenario_reload = false;
}

void MainWindow::resumeSimulation() {
    auto response = engineService->resumeEngine();
    if (response["status"] == STATUS_SUCCESS) {
        showInfoDialog(STATUS_SUCCESS, "Процесс моделирования возобновлен!");
        setModelingProcessState(ModelState::Playing);
    } else if (response["status"] == STATUS_ERROR) {
        showInfoDialog(STATUS_ERROR, response["detail"].toString());
    }
}

void MainWindow::stopSimulation() {
    auto response = engineService->stopEngine();
    if (response["status"] == STATUS_SUCCESS) {
        showInfoDialog(STATUS_SUCCESS, "Процесс моделирования остановлен!");
        setModelingProcessState(ModelState::Stopped);
    } else if (response["status"] == STATUS_ERROR) {
        showInfoDialog(STATUS_ERROR, response["detail"].toString());
        if (response["detail"].toString().contains("not running")) {
            setModelingProcessState(ModelState::Closed);
        }
    }
}

void MainWindow::set3DMode() {
    auto response = engineService->start3Mode();
    if (response["status"] == STATUS_SUCCESS) {
        is3DMode = response["value"].toBool();
        showInfoDialog(STATUS_SUCCESS, is3DMode ? "Динамический режим 3D запущен!" : "Динамический режим 3D остановлен!");
        update3DButtonView();
    } else if (response["status"] == STATUS_ERROR) {
        showInfoDialog(STATUS_ERROR, response["detail"].toString());
    }
}

void MainWindow::changeSpeed(int speed) {
    auto response = engineService->setSpeed(speed);
    if (response["status"] == STATUS_SUCCESS) {
        speedValue = speed;
    } else if (response["status"] == STATUS_ERROR) {
        showInfoDialog(STATUS_ERROR, response["detail"].toString());
    }
}

void MainWindow::createScenario() {
    DScenarioDistrict dScenarioDistrict;
    if(dScenarioDistrict.exec() == QDialog::Accepted) {
        QJsonObject districtJson = dScenarioDistrict.getCurrentDistrict();
        QJsonArray arrayObjects;
        for(DataStorageItem* objectItem: DataStorageServiceFactory::getInstance()->getElements(TypeElement::teOBJECT)) {
            QString idObject = objectItem->getData().value("id").toString();
            if(idObject.startsWith("Model_00") || idObject.startsWith("WEATHER") || idObject.startsWith("CommunicationModel")) {
                arrayObjects.append(objectItem->getData());
            }
        }
        districtJson["objects"] = arrayObjects;
        auto config = EnvConfig::getInstance();
        auto pathTMP = config->get("PATH_TMP", "");
        QString pathNewScenario = QString("%1/%2.json").arg(pathTMP, districtJson["scenario_id"].toString());

        QFile file(pathNewScenario);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
            return;

        // Сохраняем в файл
        QJsonDocument doc(districtJson);
        file.write(doc.toJson());
        file.close();

        openScenario(pathNewScenario);
        scenarioEditor->updateObjectsFromBD();
        scenarioEditor->saveScenario(false);
    }
}




