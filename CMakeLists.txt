cmake_minimum_required(VERSION 3.16)

# Установите имя проекта
project(Memory VERSION 0.1 LANGUAGES CXX)

# Включаем автоматическую обработку UI, MOC и ресурсов (RCC)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Настройка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск пакетов Qt
# ИСПРАВЛЕНИЕ: Используем 'Multimedia' как компонент. В Qt 6 это может включать
# подмодули. Если используется Qt 5, это верный путь.
# В зависимости от точной версии Qt 6, может потребоваться 'MultimediaWidgets',
# но начнем с 'Multimedia'.
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia)

# --- Список ВСЕХ Исходных Файлов (для корректной работы MOC) ---
set(PROJECT_SOURCES
    main.cpp
    memorygamewindow.cpp
    memorygamewindow.h
    memorygamewindow.ui
    memory_game.qrc

    mainmenu.cpp
    mainmenu.h
    styleswindow.cpp
    styleswindow.h

    settingswindow.cpp
    settingswindow.h
)
# -------------------------------------------------------------


# --- Создание Исполняемого Файла ---
if(QT_VERSION_MAJOR EQUAL 6)
    qt_add_executable(Memory
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        settingswindow.h settingswindow.cpp
    )
    # ИСПРАВЛЕНИЕ ДЛЯ QT 6: Для звуковых эффектов (QSoundEffect) и плеера
    # обычно достаточно Qt6::Multimedia, но если не сработает, нужно будет
    # добавить Qt6::Qml/Qt6::Quick, но это более сложный случай.
    # В большинстве случаев Qt6::Multimedia достаточно для базового аудио.
else()
    # Конфигурация для Qt 5
    if(ANDROID)
        # Если сборка идет под Android
        add_library(Memory SHARED
            ${PROJECT_SOURCES}
        )
    else()
        # Если сборка идет под Desktop
        add_executable(Memory WIN32
                    ${PROJECT_SOURCES}
        )
    endif()
endif()
set_property(TARGET Memory PROPERTY WIN32_EXECUTABLE ON)
# --- Настройка Свойств Цели ---

# Линковка с библиотеками Qt Widgets и Multimedia
# ИСПРАВЛЕНИЕ: Добавляем Qt::Multimedia
target_link_libraries(Memory PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Multimedia)

# Настройка пакетов (Mac/Windows)
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.memory)
else()
  set(BUNDLE_ID_OPTION)
endif()

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Memory)
endif()
