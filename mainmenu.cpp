#include "mainmenu.h"
#include "memorygamewindow.h" // Подключаем заголовок вашей игры
#include "styleswindow.h"     // Подключаем окно стилей
#include "settingswindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>      // Для сообщений
#include <QApplication>     // Для qApp и выхода
#include <QSettings>        // Для сохранения/загрузки монет
#include <QCloseEvent>      // Для closeEvent
#include <QUrl>             // НОВОЕ: Для QUrl в QSoundEffect и QMediaPlayer

// --- Вспомогательные методы для работы с монетами ---

/**
 * @brief Загружает количество монет из QSettings.
 */
void MainMenu::loadCoins()
{
    QSettings settings("AmNyamm", "MemoryGame");
    // Устанавливаем значение по умолчанию 1000, если нет сохраненного значения
    coins = settings.value("coins", 1000).toInt();
}

/**
 * @brief Сохраняет количество монет в QSettings.
 */
void MainMenu::saveCoins()
{
    QSettings settings("AmNyamm", "MemoryGame");
    settings.setValue("coins", coins);
}

/**
 * @brief Обновляет метку с количеством монет на UI.
 */
void MainMenu::updateCoinLabel()
{
    if (coinLabel) {
        coinLabel->setText(QString("Монеты: %1 💰").arg(coins));
    }
}

// НОВОЕ: Метод для применения настроек аудио
void MainMenu::applyAudioSettings()
{
    QSettings settings("AmNyamm", "MemoryGame");

    // 1. Настройки Музыки (Menu BGM)
    bool musicEnabled = settings.value("audio/music_enabled", true).toBool();
    float musicVolume = musicEnabled ? 0.1f : 0.0f; // Используем исходную громкость 0.1f

    // ИСПРАВЛЕНИЕ ОШИБКИ: Используем menuAudioOutput, как объявлено в mainmenu.h
    if (menuAudioOutput) {
        menuAudioOutput->setVolume(musicVolume);
    }

    // 2. Настройки Звуковых Эффектов (Button Click)
    bool soundEnabled = settings.value("audio/sound_enabled", true).toBool();
    float soundVolume = soundEnabled ? 0.8f : 0.0f; // Используем исходную громкость 0.8f

    if (clickAudioOutput) {
        clickAudioOutput->setVolume(soundVolume);
    }
}
// -------------------------------------------------------------

/**
 * @brief Конструктор MainMenu.
 */
MainMenu::MainMenu(QWidget *parent)
    : QWidget(parent)
    , coins(0) // Инициализируем монеты перед загрузкой
    , menuBGMPlayer(new QMediaPlayer(this)) // НОВОЕ: Инициализация BGM плеера
    , menuAudioOutput(new QAudioOutput(this))
    , clickSound(new QMediaPlayer(this))    // НОВОЕ: Инициализация звука нажатия
    , clickAudioOutput(new QAudioOutput(this))
{
    loadCoins();    // Загружаем монеты при запуске
    setupUI();      // Настраиваем интерфейс
    applyStyles();  // Применяем стили

    // Настройка фоновой музыки меню
    menuBGMPlayer->setAudioOutput(menuAudioOutput);
    // Громкость будет установлена в applyAudioSettings
    menuBGMPlayer->setSource(QUrl("qrc:/audios/menu_bgm.mp3"));
    menuBGMPlayer->setLoops(QMediaPlayer::Infinite);
    // Настройка звука нажатия
    clickSound->setAudioOutput(clickAudioOutput);
    // Громкость будет установлена в applyAudioSettings
    clickSound->setSource(QUrl("qrc:/audios/button_click.mp3"));
    // НОВОЕ: Применяем настройки звука сразу после инициализации плееров
    applyAudioSettings();
    menuBGMPlayer->play();
}

/**
 * @brief Деструктор MainMenu.
 */
MainMenu::~MainMenu()
{
}

/**
 * @brief Обработчик события закрытия окна.
 * @param event Объект события закрытия.
 *
 * ИСПРАВЛЕНИЕ 1: Реализация отсутствующего closeEvent(QCloseEvent*).
 */
void MainMenu::closeEvent(QCloseEvent *event)
{
    // Сохраняем текущее количество монет при закрытии главного окна.
    saveCoins();
    // Обязательно вызываем базовую реализацию
    QWidget::closeEvent(event);
}


/**
 * @brief Настройка UI.
 */
void MainMenu::setupUI()
{
    // ... (код настройки UI остался прежним, кроме coinLabel) ...
    setWindowTitle("Игра на Память - Главное Меню");
    setFixedSize(450, 600);

    // Главная вертикальная компоновка
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    // 1. Заголовок
    QLabel *titleLabel = new QLabel("Игра на Память");
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);

    // 2. Метка для монет
    coinLabel = new QLabel(this); // Инициализируем member
    coinLabel->setObjectName("coinLabel");
    coinLabel->setAlignment(Qt::AlignCenter);
    updateCoinLabel(); // Устанавливаем начальное значение

    // 3. Кнопки
    QPushButton *playButton = new QPushButton("Играть");
    playButton->setObjectName("menuButton");
    connect(playButton, &QPushButton::clicked, this, &MainMenu::onPlayClicked);

    QPushButton *stylesButton = new QPushButton("Стили");
    stylesButton->setObjectName("menuButton");
    connect(stylesButton, &QPushButton::clicked, this, &MainMenu::onStylesClicked);

    QPushButton *settingsButton = new QPushButton("Настройки");
    settingsButton->setObjectName("menuButton");
    connect(settingsButton, &QPushButton::clicked, this, &MainMenu::onSettingsClicked);

    QPushButton *exitButton = new QPushButton("Выход");
    exitButton->setObjectName("menuButton");
    connect(exitButton, &QPushButton::clicked, qApp, &QApplication::quit);


    // 4. Сборка компоновки
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(coinLabel);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(playButton);
    mainLayout->addWidget(stylesButton);
    mainLayout->addWidget(settingsButton);
    mainLayout->addStretch(1);
    mainLayout->addWidget(exitButton);
}

/**
 * @brief Применение стилей QSS.
 */
void MainMenu::applyStyles()
{
    // Стиль QSS для главного меню
    QString styleSheet = R"(
        QWidget {
            background-color: #9fbfa1;
            font-family: 'Segoe UI', 'Roboto', Arial, sans-serif;
            color: #1e2b38; /* Темный текст */
            font-size: 16px;
        }

        /* Заголовок */
        #titleLabel {
            font-size: 40px;
            font-weight: bold;
            color: #2c3e50;
            padding: 15px;
            letter-spacing: 1px;
        }

        /* Метка для монет */
        #coinLabel {
            font-size: 20px;
            color: #e67e22; /* Яркий оранжевый для акцента */
            font-weight: 700;
            padding: 5px;
            margin-bottom: 10px;
        }

        /* Стиль для всех кнопок в меню */
        #menuButton {
            background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                              stop:0 #3498db, stop:1 #2980b9); /* Голубой градиент */
            color: white;
            border: none;
            padding: 15px 30px;
            font-size: 19px;
            font-weight: 600;
            border-radius: 10px; /* Более скругленные углы */
            min-height: 45px;
        }

        /* Эффект при наведении */
        #menuButton:hover {
            background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                              stop:0 #2980b9, stop:1 #2471a3); /* Темнее градиент */
        }

        /* Эффект при нажатии */
        #menuButton:pressed {
            background-color: #1a5276; /* Еще темнее синий */
        }

        /* Улучшение QMessageBox - общие стили */
        QMessageBox {
            color: #34495e;
            font-size: 15px;
        }
        QMessageBox QPushButton {
            background-color: #3498db;
            color: #c2c2c2;
            border-radius: 5px;
            padding: 8px 20px;
            font-size: 14px;
        }
        QMessageBox QPushButton:hover {
            background-color: #2980b9;
        }
    )";

    // Применяем стили ко всему приложению
    qApp->setStyleSheet(styleSheet);
}


// Кнопка Играть
void MainMenu::onPlayClicked()
{
    // НОВОЕ: Проигрываем звук клика
    clickSound->setPosition(0);
    clickSound->play();

    // Останавливаем музыку меню перед запуском игры
    menuBGMPlayer->stop();

    // Скрываем главное меню
    this->hide();

    // Создаем окно игры
    MemoryGameWindow *gameWindow = new MemoryGameWindow();

    // Устанавливаем флаг, чтобы окно удалилось само при закрытии
    gameWindow->setAttribute(Qt::WA_DeleteOnClose);

    // ИСПРАВЛЕНИЕ 2: Подключаем сигналы победы/поражения/закрытия к новым слотам.
    connect(gameWindow, &MemoryGameWindow::gameWon, this, &MainMenu::onGameWon);
    connect(gameWindow, &MemoryGameWindow::gameLost, this, &MainMenu::onGameLost);

    // Подключаем сигнал destroyed для общего случая закрытия окна (например, по крестику)
    // gameWindow->close() в слотах победы/поражения вызывает этот destroyed сигнал
    connect(gameWindow, &QWidget::destroyed, this, &MainMenu::onGameWindowClosed);

    // Показываем игру
    gameWindow->show();
}

// Кнопка стили
void MainMenu::onStylesClicked()
{
    // НОВОЕ: Проигрываем звук клика
    clickSound->setPosition(0);
    clickSound->play();

    // Открываем окно стилей, передавая текущее количество монет.
    StylesWindow *stylesWindow = new StylesWindow(coins, this);
    stylesWindow->setAttribute(Qt::WA_DeleteOnClose); // Удаляем при закрытии
    stylesWindow->show();
}

// кнопка настройки
void MainMenu::onSettingsClicked()
{
    // НОВОЕ: Проигрываем звук клика
    clickSound->setPosition(0);
    clickSound->play();

    SettingsWindow settingsWindow(this);
    settingsWindow.exec(); // exec() для модального окна (блокирует выполнение)

    // После закрытия окна настроек, применяем новые настройки аудио
    applyAudioSettings();
}


// --- Реализация Слотов Игры ---

/**
 * @brief Слот, вызываемый, когда игра сообщает о победе.
 * @param moves Количество ходов, затраченных на победу.
 *
 * ИСПРАВЛЕНИЕ 3: Реализация отсутствующего onGameWon(int).
 */
void MainMenu::onGameWon(int moves)
{
    // 1. Рассчитываем награду
    // Пример: 1000 монет - (ходы * 10). Минимум 500.
    int reward = qMax(500, 1000 - moves * 10);

    // 2. Обновляем монеты
    coins += reward;
    saveCoins(); // Сохраняем изменения

    // 3. Выводим сообщение о победе
    QMessageBox::information(this,
                             "Победа!",
                             QString("Поздравляем! Вы нашли все пары за %1 ходов.\n"
                                     "Ваша награда: %2 💰").arg(moves).arg(reward));

    // Обновляем метку, чтобы пользователь увидел новое кол-во монет,
    // когда главное меню станет видно (после вызова onGameWindowClosed).
    updateCoinLabel();
}

/**
 * @brief Слот, вызываемый, когда игра сообщает о поражении.
 * @param pairsFound Количество найденных пар до поражения.
 *
 * ИСПРАВЛЕНИЕ 4: Реализация отсутствующего onGameLost(int).
 */
void MainMenu::onGameLost(int pairsFound)
{
    // Награда за найденные пары
    int reward = pairsFound * 50;
    coins += reward;
    saveCoins(); // Сохраняем изменения

    QMessageBox::information(this,
                             "Поражение",
                             QString("Время вышло или кончились попытки!\n"
                                     "Вы нашли %1 пар.\n"
                                     "Ваша утешительная награда: %2 💰").arg(pairsFound).arg(reward));

    updateCoinLabel();
}

/**
 * @brief Слот, вызываемый при закрытии окна игры.
 *
 * ИСПРАВЛЕНИЕ 5: Реализация отсутствующего onGameWindowClosed().
 */
void MainMenu::onGameWindowClosed()
{
    // Убедимся, что главное меню видно, когда игра закрывается (победа, поражение или крестик).
    this->show();

    // Обновляем метку монет на случай, если их изменили в окне "Стили"
    // или если игру закрыли, не завершив.
    loadCoins();
    updateCoinLabel();

    menuBGMPlayer->play();
}
