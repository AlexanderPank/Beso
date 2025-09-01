#include "SimulationEventWidgetInMap.h"
#include <QJsonObject>
#include <QVBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QSettings>

SimulationEventWidgetInMap::SimulationEventWidgetInMap(QWidget *parent)
        : QWidget(parent)
{
    // Убираем стандартные рамки окна
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);

    // Начальный размер
    this->setMinimumSize(200, 200);
    this->resize(400, 300);
    // таймер для автоматического обновления времени с последнего скроллинга
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, [this]() {
        m_wasScroll -= m_wasScroll > 0 ? 1 : 0;
    });
    updateTimer->start(1000); // Обновление каждые 500 мс

    // Настройка текстового поля
    m_textEdit = new QTextEdit(this);
    m_textEdit->setReadOnly(true);
    m_textEdit->setFrameShape(QFrame::NoFrame);
    m_textEdit->viewport()->setCursor(Qt::ArrowCursor); // Курсор ввода в текстовом поле
//    m_textEdit->setAttribute(Qt::WA_TransparentForMouseEvents);
    // Устанавливаем фильтр событий для QTextEdit
    //m_textEdit->installEventFilter(this);
    m_textEdit->viewport()->installEventFilter(this);

    m_textEditFilter = new QTextEdit(this);
    m_textEditFilter->setFrameShape(QFrame::NoFrame);
    m_textEditFilter->setMaximumHeight(30);
    connect(m_textEditFilter, &QTextEdit::textChanged, this, &SimulationEventWidgetInMap::filterLog);
    m_textEditFilter->setTextColor(QColor(0, 0, 0, 200));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_textEdit);
    layout->addWidget(m_textEditFilter);
    layout->setContentsMargins(5, 5, 5, 5);
    this->setLayout(layout);

//    QSettings settings;
//    QSize savedSize = settings.value("EventLog/Size", QSize(200, 300)).toSize();
//    QPoint savedPos = settings.value("EventLog/Pos", QPoint(0, 0)).toPoint();
//    resize(savedSize);
//    move(savedPos);

}
SimulationEventWidgetInMap::~SimulationEventWidgetInMap(){
//    QSettings settings;
//    settings.setValue("EventLog/Size", size());
//    settings.setValue("EventLog/Pos", pos());
}

void SimulationEventWidgetInMap::filterLog() {
    m_filter_text = m_textEditFilter->toPlainText();
    if (m_filter_text.isEmpty())
        this->m_textEdit->setText(m_log_text);
    else {
        QString filtered_text_list = "";
        for (QString text : m_log_text_list)
            if (text.contains(m_filter_text)) filtered_text_list+=text + "\n";
        this->m_textEdit->setText(filtered_text_list);
    }
}
bool SimulationEventWidgetInMap::eventFilter(QObject *obj, QEvent *event)
{
//    if (obj == m_textEdit->viewport()) {
//        if (event->type() == QEvent::MouseButtonPress) {
//            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
//
//            // Если нажатие не на скроллбаре и не на текстовых ссылках
//            if (!m_textEdit->verticalScrollBar()->geometry().contains(mouseEvent->pos()) &&
//                m_textEdit->anchorAt(mouseEvent->pos()).isEmpty()) {
//                // Инициируем перетаскивание окна
//                m_isDragging = true;
//                m_dragStartPosition = mouseEvent->globalPos();
//                return true; // Перехватываем событие
//            }
//        }
//        else if (event->type() == QEvent::MouseMove && m_isDragging) {
//            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
//            QPoint delta = mouseEvent->globalPos() - m_dragStartPosition;
//            move(pos() + delta);
//            m_dragStartPosition = mouseEvent->globalPos();
//            return true;
//        }
//        else if (event->type() == QEvent::MouseButtonRelease) {
//            m_isDragging = false;
//        }else if (event->type() == QEvent::Wheel) {
//            // Получаем событие колесика мыши
//            QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
//
//            // Прокручиваем QTextEdit
//            QScrollBar *vScroll = m_textEdit->verticalScrollBar();
//            vScroll->setValue(vScroll->value() - wheelEvent->angleDelta().y());
//            m_wasScroll = 10;
//            // Блокируем передачу события дальше
//            return false;
//        }
//    }
    return QWidget::eventFilter(obj, event);
}

void SimulationEventWidgetInMap::mousePressEvent(QMouseEvent *event)
{
//    // Область для изменения размера (правый нижний угол)
//    QRect resizeHandleRect = QRect(
//            width() - RESIZE_HANDLE_SIZE,
//            height() - RESIZE_HANDLE_SIZE,
//            RESIZE_HANDLE_SIZE,
//            RESIZE_HANDLE_SIZE
//    );
//
//    if (resizeHandleRect.contains(event->pos())) {
//        m_isResizing = true;
//    }
}

void SimulationEventWidgetInMap::mouseMoveEvent(QMouseEvent *event)
{
//    // Изменение курсора при наведении на область изменения размера
//    QRect resizeHandleRect = QRect(
//            width() - RESIZE_HANDLE_SIZE,
//            height() - RESIZE_HANDLE_SIZE,
//            RESIZE_HANDLE_SIZE,
//            RESIZE_HANDLE_SIZE
//    );
//
//    if (resizeHandleRect.contains(event->pos())) {
//        setCursor(Qt::SizeFDiagCursor);
//    } else {
//        setCursor(Qt::ArrowCursor);
//    }
//
//    if (m_isDragging) {
//        QPoint delta = event->globalPos() - m_dragStartPosition;
//        move(pos() + delta);
//        m_dragStartPosition = event->globalPos();
//    }
//    if (m_isResizing) {
//        QSize newSize(width(), height());
//
//        // Вычисляем новые размеры на основе движения мыши
//        QPoint delta = event->pos() - QPoint(width(), height());
//        newSize += QSize(delta.x(), delta.y());
//
//        // Ограничиваем минимальный размер
//        newSize = newSize.expandedTo(minimumSize());
//        resize(newSize);
//    }

}

void SimulationEventWidgetInMap::mouseReleaseEvent(QMouseEvent *event)
{
//    m_isDragging = false;
//    m_isResizing = false;
//    setCursor(Qt::ArrowCursor);
//    QWidget::mouseReleaseEvent(event);
}

void SimulationEventWidgetInMap::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    // Рисуем индикатор изменения размера
    QPainter painter(this);
    painter.setPen(Qt::gray);
    painter.drawLine(
            width() - RESIZE_HANDLE_SIZE,
            height(),
            width(),
            height() - RESIZE_HANDLE_SIZE
    );
}

void SimulationEventWidgetInMap::updateEventList(const QJsonArray &events)
{

    for (const QJsonValue &value : events) {
        QJsonObject event = value.toObject();
        QString type = event["event_type"].toString();
        QString text = event["event_text"].toString();
        QString objectId = event["object_id"].toString();
        if (text.trimmed().isEmpty()) continue; // Пропускаем пустые строки и строки с пробелами

        QString formattedText;
        if (!objectId.isEmpty()) {
            formattedText = QString("[%1]: %2").arg(objectId).arg(text);
        } else {
            formattedText = QString("%1: %2").arg(type).arg(text);
        }

        QColor color = getColorForType(type);
        m_textEdit->setTextColor(color);
        m_log_text += formattedText;
        m_log_text_list.append(formattedText);
        if (m_filter_text.isEmpty() || formattedText.contains(m_filter_text))
            m_textEdit->append(formattedText);

    }

    // Автоскролл в конец
    if (m_wasScroll <= 0)
        m_textEdit->verticalScrollBar()->setValue(m_textEdit->verticalScrollBar()->maximum());
}

QColor SimulationEventWidgetInMap::getColorForType(const QString &type) const
{
    if (type == "Ошибка") return Qt::red;
    if (type == "Событие") return Qt::blue;
    if (type == "Действие") return Qt::darkGreen;
    if (type == "Информация") return Qt::black;
    return Qt::gray;
}