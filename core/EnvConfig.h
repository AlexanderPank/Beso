#ifndef SCENARIOCLIENT_ENVCONFIG_H
#define SCENARIOCLIENT_ENVCONFIG_H


#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMap>
#include <QDebug>
#include <QSharedPointer>
#include <QRegularExpression>

class EnvConfig {
private:
    QMap<QString, QString> configMap;
    QString envFilePath;
    static QSharedPointer<EnvConfig> instance;

    // Parse a line from .env file
    void parseLine(const QString& line);

    // Private constructor for singleton pattern
    EnvConfig(const QString& filePath = "../.env");

public:
    // Singleton getter
    static QSharedPointer<EnvConfig> getInstance(const QString& filePath = "../.env");

    // Load or reload the .env file
    bool loadEnvFile();

    // Get a configuration value by key
    QString get(const QString& key, const QString& defaultValue = QString()) const;

    // Check if a key exists
    bool has(const QString& key) const;

    // Get all configuration values
    const QMap<QString, QString>& getAll() const;

    // Set a configuration value (won't persist to file)
    void set(const QString& key, const QString& value);
};


#endif //SCENARIOCLIENT_ENVCONFIG_H
