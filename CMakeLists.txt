cmake_minimum_required(VERSION 3.16)

project(EnglishLearningApp VERSION 0.1 LANGUAGES CXX)



set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(MSVC)
    add_compile_options("/utf-8")
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia TextToSpeech)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia TextToSpeech)
set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        resources.qrc
        difficultywindow.cpp
        difficultywindow.h
        PracticeWindow.h
        PracticeWindow.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(EnglishLearningApp
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        gamewindow.h
        gamewindow.cpp
    )
else()
    if(ANDROID)
        add_library(EnglishLearningApp SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(EnglishLearningApp
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(EnglishLearningApp PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Multimedia Qt${QT_VERSION_MAJOR}::TextToSpeech)


if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.EnglishLearningApp)
endif()

set_target_properties(EnglishLearningApp PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS EnglishLearningApp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

configure_file(${CMAKE_SOURCE_DIR}/dictionary.json
              ${CMAKE_BINARY_DIR}/dictionary.json COPYONLY)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(EnglishLearningApp)
endif()
