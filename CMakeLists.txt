# CMake things ------------------------------------------------------------
cmake_minimum_required(VERSION 3.16)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include(FeatureSummary)


# Project things ----------------------------------------------------------
project(cuteviewer VERSION "0.0.0" LANGUAGES CXX)

configure_file(config.h.in config.h)
include_directories(${CMAKE_CURRENT_BINARY_BIN})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_CAST_FROM_ASCII
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_NO_CAST_FROM_BYTEARRAY
    -DQT_NO_SIGNALS_SLOTS_KEYWORDS
    -DQT_USE_QSTRINGBUILDER
    -DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT
    -DQT_STRICT_ITERATORS
    -DQT_NO_KEYWORDS
    -DQT_DISABLE_DEPRECATED_BEFORE=0x051500
    -DQT_NO_FOREACH
)


# Qt things ---------------------------------------------------------------
set(QT_MINIMUM_VERSION "5.15.0")

find_package(Qt5 "${QT_MINIMUM_VERSION}" COMPONENTS REQUIRED
    Core
    Gui
    Widgets
    PrintSupport
    PdfWidgets
)


# Compile && Link ---------------------------------------------------------
add_executable(cuteviewer
    src/main.cpp
    src/application.cpp
    src/mainwindow.cpp
    src/searchbar.cpp
    src/statusbar.cpp
    src/settingsdialog.cpp
    resources.qrc
)

target_link_libraries(cuteviewer PRIVATE 
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::PrintSupport
    Qt5::PdfWidgets
)


# INSTALL ----------------------------------------------------------------
install(TARGETS cuteviewer RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")

install(FILES data/cuteviewer.desktop     DESTINATION "${CMAKE_INSTALL_PREFIX}/share/applications")
install(FILES data/cuteviewer.appdata.xml DESTINATION "${CMAKE_INSTALL_PREFIX}/share/metainfo")
install(FILES LICENSE                     DESTINATION "${CMAKE_INSTALL_PREFIX}/share/adjam/cuteviewer")

install(DIRECTORY icons                DESTINATION "${CMAKE_INSTALL_PREFIX}/share/adjam/cuteviewer")


# --------------------------------------------------------------------------
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
