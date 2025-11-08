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
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Список всех исходных файлов
set(PROJECT_SOURCES
    main.cpp
    memorygamewindow.cpp
    memorygamewindow.h
    memorygamewindow.ui
)

# --- Создание Исполняемого Файла (Executable) ---

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    # Конфигурация для Qt 6
    qt_add_executable(Memory
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        memory_game.qrc
        # <-- Файл ресурсов добавлен для Qt 6
    )
    # Дополнительная настройка Android/пакетов здесь...
else()
    # Конфигурация для Qt 5
    if(ANDROID)
        # Если сборка идет под Android, создаем разделяемую библиотеку
        add_library(Memory SHARED
            ${PROJECT_SOURCES}
            memory_game.qrc  # <-- Файл ресурсов добавлен для Qt 5 / Android
        )
    else()
        # Если сборка идет под Desktop (Windows, Linux, macOS)
        add_executable(Memory
            ${PROJECT_SOURCES}
            memory_game.qrc  # <-- **ИСПРАВЛЕНО:** Файл ресурсов добавлен для Desktop Qt 5
        )
    endif()
endif()

# --- Настройка Свойств Цели ---

# Линковка с библиотеками Qt Widgets
target_link_libraries(Memory PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Настройка пакетов (Mac/Windows)
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Memory)
endif()
set_target_properties(Memory PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Инсталляция
include(GNUInstallDirs)
install(TARGETS Memory
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Финализация для Qt 6
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Memory)
endif()
