cmake_minimum_required(VERSION 3.16)
project(ScenarioClientTests)

# Настройка выходной директории
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
# Автоматическая генерация MOC
set(CMAKE_AUTOMOC ON)

# Настройки Qt
find_package(Qt5 REQUIRED COMPONENTS
        Gui
        Core
        Test
        Gui
        Widgets
        Network
        WebSockets
) # Добавьте нужные модули

# Автоматическое обнаружение тестов
file(GLOB TEST_SOURCES "tst_*.cpp")

# Добавление исходников из основной директории проекта
set(PROJECT_SOURCES
        ../models/PropertyModel.cpp
        ../models/PropertyModel.h
        ../models/ObjectScenarioModel.cpp
        ../models/ObjectScenarioModel.h
        ../models/AlgorithmModel.cpp
        ../models/AlgorithmModel.h
        ../models/ActionModel.cpp
        ../models/ActionModel.h
        ../models/FileModel.cpp
        ../models/FileModel.h
        ../models/Feature.cpp
        ../models/Feature.h
        ../models/GeoPoint.cpp
        ../models/GeoPoint.h
        ../db_service/ScenarioParser.cpp
        ../db_service/ScenarioParser.h
        test_main.cpp
        test_propertymodeltest.cpp
        test_propertymodeltest.h
        test_scenarioparser.cpp
        test_scenarioparser.h
        ../models/AlgorithParamModel.cpp
        ../models/AlgorithParamModel.h
        ../models/InteractionModel.cpp
        ../models/InteractionModel.h
        ../models/SimulationParametersModel.cpp
        ../models/SimulationParametersModel.h
)

# Основной исполняемый файл тестов
add_executable(ScenarioClientTests
        ${TEST_SOURCES}
        ${PROJECT_SOURCES}
)

# Подключение зависимостей
target_link_libraries(ScenarioClientTests
    Qt5::Core
    Qt5::Test
    Qt5::WebSockets
    Qt5::Network
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    qdmapacces
    qdmapmtrex
    mapcomponents
    dl
)

# Добавление в CTest
enable_testing()
add_test(NAME AllTests COMMAND ScenarioClientTests)