#include "EnvConfig.h"

// Initialize static member
QSharedPointer<EnvConfig> EnvConfig::instance = nullptr;

EnvConfig::EnvConfig(const QString &filePath): envFilePath(filePath) {
    loadEnvFile();
}

void EnvConfig::parseLine(const QString &line) {
    // Skip empty lines and comments
    if (line.trimmed().isEmpty() || line.trimmed().startsWith('#')) {
        return;
    }

    // Find the position of the equals sign
    int pos = line.indexOf('=');
    if (pos != -1) {
        QString key = line.left(pos).trimmed();
        QString value = line.mid(pos + 1).trimmed();

        // Remove quotes if present
        QRegularExpression quoteRegex("^[\"'](.+)[\"']$");
        auto match = quoteRegex.match(value);
        if (match.hasMatch()) {
            value = match.captured(1);
        }

        configMap[key] = value;
    }
}

QString EnvConfig::get(const QString &key, const QString &defaultValue) const {
    return configMap.value(key, defaultValue);
}

bool EnvConfig::has(const QString &key) const {
    return configMap.contains(key);
}

const QMap<QString, QString> &EnvConfig::getAll() const{
    return configMap;
}

void EnvConfig::set(const QString &key, const QString &value) {
    configMap[key] = value;
}

bool EnvConfig::loadEnvFile() {
    configMap.clear();
    QFile file(envFilePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open" << envFilePath;
        return false;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        parseLine(line);
    }

    file.close();
    return true;
}

QSharedPointer<EnvConfig> EnvConfig::getInstance(const QString &filePath) {
    if (!instance) {
        instance = QSharedPointer<EnvConfig>(new EnvConfig(filePath));
    }
    return instance;
}
