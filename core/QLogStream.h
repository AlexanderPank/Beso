#ifndef SCENARIOCLIENT_QLOGSTREAM_H
#define SCENARIOCLIENT_QLOGSTREAM_H

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMutex>


// Глобальные настройки логгера
namespace Logger {
    enum OutputType {
        Console,
        File,
        Both
    };

    static OutputType outputType = Console;
    static QMutex logMutex;

    void setOutputType(OutputType type) ;
}



// Класс логгера
class QLogStream {
public:
    QLogStream(const char* file, const char* function)
            : m_file(file), m_function(function) {
        // Получаем только имя файла (без пути)
        QString fullPath(file);
        m_fileName = fullPath.mid(fullPath.lastIndexOf('/') + 1);
    }

    ~QLogStream() {
        Logger::logMutex.lock();

        QString message = m_buffer;
        QString logEntry = QString("[%1] %2 (%3): %4")
                .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"))
                .arg(m_fileName)
                .arg(m_function)
                .arg(message);

        if (Logger::outputType == Logger::Console || Logger::outputType == Logger::Both) {
            qDebug().noquote() << logEntry;
        }

        if (Logger::outputType == Logger::File || Logger::outputType == Logger::Both) {
            QFile file("/tmp/scenario.log");
            if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
                QTextStream stream(&file);
                stream << logEntry << "\n";
                file.close();
            }
        }

        Logger::logMutex.unlock();
    }

    QLogStream& operator<<(const QString& message) {
        m_buffer += message;
        return *this;
    }

    QLogStream& operator<<(const char* message) {
        m_buffer += QString::fromUtf8(message);
        return *this;
    }

    template<typename T>
    QLogStream& operator<<(const T& value) {
        m_buffer += QString::number(value);
        return *this;
    }

private:
    QString m_buffer;
    QString m_fileName;
    const char* m_file;
    const char* m_function;
};

// Макрос для удобного использования
#define qLog() QLogStream(__FILE__, Q_FUNC_INFO)


#endif //SCENARIOCLIENT_QLOGSTREAM_H
