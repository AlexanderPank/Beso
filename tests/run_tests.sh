#!/bin/bash

# Переходим в директорию тестов
cd "$(dirname "$0")" || exit

# Очищаем предыдущую сборку
rm -rf cmake-build-debug
mkdir -p cmake-build-debug
cd cmake-build-debug || exit

# Конфигурация и сборка
cmake .. && make

# Запуск тестов с подробным выводом
if [ -f "./ScenarioClientTests" ]; then
    ./ScenarioClientTests -v2
else
    echo "ERROR: Executable ScenarioClientTests not found!"
    exit 1
fi