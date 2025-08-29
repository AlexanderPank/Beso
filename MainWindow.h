#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "EngineService.h"
#include "WebsocketService.h"
#include "map_widget/MapWidget.h"
#include "db_service/ForcesEditor.h"
#include "db_service/ScenarioEditor.h"
#include "map_widget/SimulationEventWidgetInMap.h"

/**
 * @enum ModelState
 * @brief Перечисление состояний модели симуляции.
 */
enum ModelState {
    Closed,   /**< Симуляция закрыта. */
    Opened,   /**< Симуляция открыта. */
    Playing,  /**< Симуляция выполняется. */
    Paused,   /**< Симуляция приостановлена. */
    Stopped   /**< Симуляция остановлена. */
};

const QString STATE_FINISHED = "ended";    /**< Конечное состояние симуляции. */
const QString STATE_PLAYING = "running";   /**< Состояние выполнения симуляции. */
const QString STATE_PAUSED = "paused";     /**< Состояние паузы симуляции. */
const QString STATE_OPENED = "not_started";/**< Состояние когда сценарий загружен но еще не проигрывался. */
const QString STATE_STOPPED = "stopped";/**< Состояние когда сценарий загружен но еще не проигрывался. */

const QString STATUS_SUCCESS = "success";  /**< Успешный статус. */
const QString STATUS_ERROR = "error";      /**< Статус ошибки. */

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @class MainWindow
 * @brief Основное окно приложения для управления симуляцией.
 *
 * Класс предоставляет интерфейс для управления симуляцией, включая функции запуска,
 * паузы, остановки и изменения скорости симуляции.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    /**
     * @brief Конструктор MainWindow.
     * @param parent Родительский объект.
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Деструктор MainWindow.
     */
    ~MainWindow();

    /**
     * @brief Запускает симуляцию.
     */
    void startSimulation();

    /**
     * @brief Приостанавливает симуляцию.
     */
    void pauseSimulation();

    /**
     * @brief Возобновляет симуляцию.
     */
    void resumeSimulation();
    /**
     * @brief перезагружает сценарий
     */
    void reloadScenario();

    /**
     * @brief Останавливает симуляцию.
     */
    void stopSimulation();

    /**
     * @brief Изменяет скорость симуляции.
     * @param speed Новое значение скорости.
     */
    void changeSpeed(int speed);

    /**
     * @brief Открывает сценарий симуляции.
     */
    void openScenario(QString fileName = "");

    /**
     * @brief Запускает симуляцию в 3D режиме.
     */
    void set3DMode();

private slots:
    void createScenario();

private:
    Ui::MainWindow *ui;  /**< Указатель на интерфейс пользователя. */
    std::shared_ptr<EngineService> engineService;  /**< Указатель на сервис движка симуляции. */
    std::shared_ptr<WebSocketService> websocketService;  /**< Указатель на WebSocket сервис. */

    /**
     * @brief Отображает информационное диалоговое окно.
     * @param status Статус сообщения.
     * @param text Текст сообщения.
     */
    void showInfoDialog(const QString &status, const QString &text);

    /**
     * @brief Обрабатывает обновление состояния симуляции.
     * @param data Данные состояния симуляции.
     */
    void handleSimulationStateUpdate(QJsonDocument data);

    /**
     * @brief Загружает симуляцию из файла.
     * @param filePath Путь к файлу сценария.
     * @return true, если загрузка прошла успешно; false в противном случае.
     */
    bool loadEngineByFile(const QString &filePath);


    /**
     * @brief Загружает все необходимые файлы для симуляции.
     * @param filePath Путь к сценарию.
     * @param fileList Список требуемых файлов.
     * @return true, если загрузка прошла успешно; false в противном случае.
     */
    bool loadAllNeedFiles(const QString &filePath, QList<QString> fileList);

    /**
     * @brief Загружает выбранные файлы для симуляции.
     * @param selectedFiles Список файлов для загрузки.
     * @param scenarioFilePath Путь к файлу сценария.
     * @return true, если загрузка прошла успешно; false в противном случае.
     */
    bool onLoadFiles(const QList<UploadFile> &selectedFiles, QString scenarioFilePath);

    /**
     * @brief Устанавливает текущее состояние процесса моделирования.
     * @param newState Новое состояние моделирования.
     */
    void setModelingProcessState(ModelState newState);
    /**
    * @brief Устанавливает Вид кнопки 3D Включенная/выключенная.
    */
    void update3DButtonView();

    QString currentScenarioFileName = ""; /**< Имя загружаемого файла сценария. */
    ModelState state = ModelState::Stopped; /**< Текущее состояние симуляции. */
    bool firstStart; /**< Флаг первого запуска симуляции. */
    int speedValue; /**< Текущая скорость симуляции. */

    std::unique_ptr<QFile> fileScenario; /**< Указатель на файл сценария симуляции. */

    MapWidget *mapWidget;
    ForcesEditor *forceEditor;
    ScenarioEditor *scenarioEditor;
    SimulationEventWidgetInMap *eventLog;
    bool is3DMode = false;
    int m_time_elapsed = 0;
};

#endif // MAINWINDOW_H
