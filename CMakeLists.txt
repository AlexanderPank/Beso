cmake_minimum_required(VERSION 3.16)
project(ScenarioClient)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3")

find_package(Qt5 COMPONENTS
    Core
    Gui
    Widgets
    Network
    WebSockets
    REQUIRED
    )

# Правильный способ добавления ресурсов
qt5_add_resources(RESOURCE_FILES resources/resource.qrc)

add_executable(ScenarioClient
    ${RESOURCE_FILES}
    main.cpp
    EngineService.cpp
    MainWindow.cpp
    map_widget/map_additional/MapCoordinates.cpp
    map_widget/map_additional/MapScale.cpp
    map_widget/map_additional/MapToolPanel.cpp
    map_widget/map_tools/CoordCtx.cpp
    map_widget/MapWidget.cpp
    signs/GeoUtil.cpp
    signs/SignBase.cpp
    signs/SignController.cpp
    signs/SignAnimator.cpp
    signs/SignShip.cpp
    WebsocketService.cpp
    db_service/modelobjectsform.h
    db_service/modelobjectsform.cpp
    db_service/modelobjectsform.ui

    ui/DObjectProperties.cpp
    ui/DObjectProperties.h
    ui/DObjectProperties.ui

    EngineService.h
    MainWindow.h
    map_widget/map_additional/MapCoordinates.h
    map_widget/map_additional/MapScale.h
    map_widget/map_additional/MapToolPanel.h
    map_widget/map_enums.h
    map_widget/map_tools/CoordCtx.h
    map_widget/MapWidget.h
    signs/GeoUtil.h
    signs/SignBase.h
    signs/SignController.h
    signs/SignAnimator.h
    signs/sign_enums.h
    signs/SignShip.h
    WebsocketService.h

    mainwindow.ui
    map_widget/map_additional/map_coordinates.ui
    map_widget/map_additional/map_scale.ui
    map_widget/map_additional/map_tool_panel.ui
    map_widget/mapwidget.ui
    core/QLogStream.cpp
    core/QLogStream.h
    map_widget/map_tools/SignEditor.cpp
    map_widget/map_tools/SignEditor.h
    map_widget/map_tools/SignDrawer.cpp
    map_widget/map_tools/SignDrawer.h
    db_service/ForcesEditor.cpp
    db_service/ForcesEditor.h
    db_service/ScenarioEditor.cpp
    db_service/ScenarioEditor.h
    db_service/ScenarioEditor.ui
    signs/SignFactory.cpp
    signs/SignFactory.h
    models/AlgorithmModel.cpp
    models/AlgorithmModel.h
    models/PropertyModel.cpp
    models/PropertyModel.h
    delegates/PropertyItemDelegate.cpp
    delegates/PropertyItemDelegate.h
    models/FileModel.cpp
    models/FileModel.h
    models/ActionModel.cpp
    models/ActionModel.h
    models/ObjectScenarioModel.cpp
    models/ObjectScenarioModel.h


    models/Feature.cpp
    models/Feature.h
    models/AlgorithParamModel.cpp
    models/AlgorithParamModel.h
    models/GeoPoint.cpp
    models/GeoPoint.h
    models/InteractionModel.cpp
    models/InteractionModel.h
    models/SimulationParametersModel.cpp
    models/SimulationParametersModel.h
    db_service/ScenarioParser.cpp
    db_service/ScenarioParser.h
    db_service/ScenarioParser.cpp
    db_service/ScenarioParser.h
    db_service/modules/daddscenarioobject.cpp
    db_service/modules/daddscenarioobject.h
    db_service/modules/daddscenarioobject.ui
    db_service/modules/daddscenariointeraction.cpp
    db_service/modules/daddscenariointeraction.h
    db_service/modules/daddscenariointeraction.ui
    core/EnvConfig.h
    map_widget/map_additional/MapInformer.cpp
    map_widget/map_additional/MapInformer.h
    models/Feature.cpp
    models/Feature.h
    models/GeoPoint.cpp
    models/GeoPoint.h
    db_service/services/models/DataStorageItem.cpp
    db_service/services/models/DataStorageItem.h
    db_service/services/DataStorageService.cpp
    db_service/services/DataStorageService.h
    db_service/services/FileDataStorageService.cpp
    db_service/services/FileDataStorageService.h
    db_service/services/DataStorageServiceFactory.h
    db_service/services/DataStorageServiceFactory.cpp
    core/EnvConfig.cpp
    db_service/services/DBDataStorageService.cpp
    db_service/services/DBDataStorageService.h
    db_service/ScenarioTacticalObjects.cpp
    db_service/ScenarioTacticalObjects.h
    db_service/ScenarioTacticalObjects.ui
    models/TacticalSignModel.cpp
    models/TacticalSignModel.h
    models/FeatureModel.cpp
    models/FeatureModel.h
    models/InteractionModelTemplate.cpp
    models/InteractionModelTemplate.h
    models/InteractionTemplateItem.cpp
    models/InteractionTemplateItem.h
    db_service/modules/components/ListWidgetInteractionTemplateItem.cpp
    db_service/modules/components/ListWidgetInteractionTemplateItem.h
    db_service/modules/components/ListWidgetInteractionTemplateItem.ui
    db_service/components/addmodelobjectsdialog.h
    db_service/components/addmodelobjectsdialog.cpp
    db_service/components/addmodelobjectsdialog.ui
    utils/ParametersChecker.cpp
    utils/ParametersChecker.h
    db_service/components/texteditdialog.h
    db_service/components/texteditdialog.cpp
    db_service/components/texteditdialog.ui
        map_widget/SimulationEventWidgetInMap.cpp
        map_widget/SimulationEventWidgetInMap.h
        db_service/modules/dscenariodistrict.cpp
        db_service/modules/dscenariodistrict.h
        db_service/modules/dscenariodistrict.ui

        diagramscene/DiagramSceneDlg.h
        diagramscene/algorithmitem.h
        diagramscene/diagramitem.h
        diagramscene/diagramscene.h
        diagramscene/arrow.h
        diagramscene/diagramtextitem.h
        diagramscene/itemtoolbox.h
        diagramscene/ObjectSelectDialog.h
        diagramscene/AlgorithmPropertiesDialog.h
        diagramscene/PropertiesDialog.h

        diagramscene/DiagramSceneDlg.cpp
        diagramscene/algorithmitem.cpp
        diagramscene/diagramitem.cpp
        diagramscene/itemtoolbox.cpp
        diagramscene/arrow.cpp
        diagramscene/diagramtextitem.cpp
        diagramscene/diagramscene.cpp
        diagramscene/ObjectSelectDialog.cpp
        diagramscene/AlgorithmPropertiesDialog.cpp
        diagramscene/PropertiesDialog.cpp
        diagramscene/itemtoolbox.ui
)




include_directories("/usr/lib/x86_64-linux-gnu/Qt5")
target_include_directories(ScenarioClient PRIVATE /usr/include/gisdesigner/)

target_link_libraries(ScenarioClient
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


target_link_libraries(ScenarioClient
    -Wl,-rpath,'$ORIGIN'
    )
