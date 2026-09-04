cmake_minimum_required(VERSION 3.16)

project(CentreDeFormation VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Activation de la compilation automatique des fichiers Qt (.ui, moc, etc.)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Chargement des modules Qt6 indispensables (Inclusion de Network pour QR Code & Mail)
find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Charts PrintSupport Network)

# Liste de tous vos fichiers C++ et de l'interface graphique .ui
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    connection.cpp
    connection.h
    formateur.cpp
    formateur.h
    cours.cpp
    cours.h
)

# Création de l'exécutable
qt_add_executable(appCentreDeFormation ${PROJECT_SOURCES})

# Liaison des bibliothèques Qt nécessaires à votre application
target_link_libraries(appCentreDeFormation PRIVATE
    Qt6::Widgets
    Qt6::Sql
    Qt6::Charts
    Qt6::PrintSupport
    Qt6::Network
)

set_target_properties(appCentreDeFormation PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)