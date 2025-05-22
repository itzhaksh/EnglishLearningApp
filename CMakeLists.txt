cmake_minimum_required(VERSION 3.27) # עדכון לגרסה חדשה יותר
project(EnglishLearningApp VERSION 0.1 LANGUAGES CXX)

# Set the Qt installation directory
set(CMAKE_PREFIX_PATH "C:/Qt/6.8.2/msvc2022_64")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia TextToSpeech)

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