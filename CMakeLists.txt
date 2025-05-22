cmake_minimum_required(VERSION 3.27)
project(EnglishLearningApp VERSION 0.1 LANGUAGES CXX)

# Allow CMake to find Qt from environment variable or default path
if(DEFINED ENV{QT_ROOT_DIR})
    set(CMAKE_PREFIX_PATH "$ENV{QT_ROOT_DIR}/lib/cmake/Qt6")
else()
    # Fallback for local development
    set(CMAKE_PREFIX_PATH "C:/Qt/6.8.2/msvc2022_64/lib/cmake/Qt6")
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia TextToSpeech)
if(NOT Qt6_FOUND)
    message(FATAL_ERROR "Qt6 not found. Please set QT_ROOT_DIR environment variable or update CMAKE_PREFIX_PATH.")
endif()

include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)

set(PROJECT_SOURCES
    main.cpp
    src/MainWindow.cpp
    src/DifficultyWindow.cpp
    src/PracticeWindow.cpp
    src/GameWindow.cpp
    include/MainWindow.h
    include/DifficultyWindow.h
    include/PracticeWindow.h
    include/GameWindow.h
    MainWindow.ui
    src/MemoryGame.cpp
    include/MemoryGame.h
    src/UserNameDialog.h
    src/UserNameDialog.cpp
    src/StatsTableWidget.cpp
    src/StatsTableWidget.h
    resources/resources.qrc
)

qt_add_executable(EnglishLearningApp
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)
qt_finalize_executable(EnglishLearningApp)

target_link_libraries(EnglishLearningApp PRIVATE
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::TextToSpeech
)

set_target_properties(EnglishLearningApp PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS EnglishLearningApp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

foreach(LEVEL RANGE 1 5)
    configure_file(${CMAKE_SOURCE_DIR}/resources/dictionary_level${LEVEL}.json
                  ${CMAKE_BINARY_DIR}/resources/dictionary_level${LEVEL}.json
                  COPYONLY)
endforeach()

file(COPY ${CMAKE_SOURCE_DIR}/resources/Learn-English.png
          ${CMAKE_SOURCE_DIR}/resources/Learn-English-Icon.png
    DESTINATION ${CMAKE_BINARY_DIR}/resources)