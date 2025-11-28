#include "mainmenu.h"
#include "memorygamewindow.h"
#include "styleswindow.h"
#include "settingswindow.h"
#include "difficultyselectionwindow.h"
#include "difficulties.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QApplication>
#include <QSettings>
#include <QCloseEvent>
#include <QUrl>

// --- Вспомогательные методы ---

void MainMenu::loadCoins()
{
    // QSettings позволяет сохранять настройки между запусками программы
    // "AmNyamm" - имя автора/компании, "MemoryGame" - название игры
    QSettings settings("AmNyamm", "MemoryGame");
    // Если настройки нет, вернет 1000 по умолчанию
    coins = settings.value("coins", 1000).toInt();
}

void MainMenu::saveCoins()
{
    QSettings settings("AmNyamm", "MemoryGame");
    settings.setValue("coins", coins); // Записываем значение на диск
}

void MainMenu::updateCoinLabel()
{
    if (coinLabel) {
        // %1 заменится на значение переменной coins
        coinLabel->setText(QString("Монеты: %1 💰").arg(coins));
    }
}

void MainMenu::applyAudioSettings()
{
    QSettings settings("AmNyamm", "MemoryGame");
    bool musicEnabled = settings.value("audio/music_enabled", true).toBool();

    // Если музыка включена громкость 0.1, иначе 0.0 (тишина)
    float musicVolume = musicEnabled ? 0.1f : 0.0f;
    if (menuAudioOutput) menuAudioOutput->setVolume(musicVolume);

    bool soundEnabled = settings.value("audio/sound_enabled", true).toBool();
    float soundVolume = soundEnabled ? 0.8f : 0.0f;
    if (clickAudioOutput) clickAudioOutput->setVolume(soundVolume);
}

MainMenu::MainMenu(QWidget *parent)
    : QWidget(parent)
    , coins(0)
    , menuBGMPlayer(new QMediaPlayer(this))
    , menuAudioOutput(new QAudioOutput(this))
    , clickSound(new QMediaPlayer(this))
    , clickAudioOutput(new QAudioOutput(this))
{
    loadCoins();
    setupUI();
    applyStyles();

    // Настраиваем плеер
    menuBGMPlayer->setAudioOutput(menuAudioOutput);
    // qrc:/ - это путь к ресурсам, встроенным внутрь exe-файла
    menuBGMPlayer->setSource(QUrl("qrc:/audios/menu_bgm.mp3"));
    menuBGMPlayer->setLoops(QMediaPlayer::Infinite); // Бесконечный повтор

    clickSound->setAudioOutput(clickAudioOutput);
    clickSound->setSource(QUrl("qrc:/audios/button_click.mp3"));

    applyAudioSettings();
    menuBGMPlayer->play();
}

MainMenu::~MainMenu()
{
}

// Вызывается автоматически при нажатии на крестик окна
void MainMenu::closeEvent(QCloseEvent *event)
{
    saveCoins(); // Сохраняем прогресс
    QWidget::closeEvent(event); // Разрешаем закрытие
}

void MainMenu::setupUI()
{
    setWindowTitle("Игра на Память - Главное Меню");
    setFixedSize(450, 600);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    QLabel *titleLabel = new QLabel("Игра на Память");
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);

    coinLabel = new QLabel(this);
    coinLabel->setObjectName("coinLabel");
    coinLabel->setAlignment(Qt::AlignCenter);
    updateCoinLabel();

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
    // qApp - глобальный указатель на приложение, quit - выход из программы
    connect(exitButton, &QPushButton::clicked, qApp, &QApplication::quit);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(coinLabel);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(playButton);
    mainLayout->addWidget(stylesButton);
    mainLayout->addWidget(settingsButton);
    mainLayout->addStretch(1);
    mainLayout->addWidget(exitButton);
}

void MainMenu::applyStyles()
{
    QString styleSheet = R"(
        QWidget {
            background-color: #5f9ea0;
            font-family: 'Segoe UI', 'Roboto', Arial, sans-serif;
            color: #480607;
            font-size: 16px;
        }
        #titleLabel {
            font-size: 40px;
            font-weight: bold;
            color: #480607;
            padding: 15px;
            letter-spacing: 1px;
        }
        #coinLabel {
            font-size: 20px;
            color: #800020;
            font-weight: 700;
            padding: 5px;
            margin-bottom: 10px;
        }
        #menuButton {
            background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #98f5ff, stop:1 #7ac5cd);
            color: #800020;
            border: none;
            padding: 15px 30px;
            font-size: 19px;
            font-weight: 600;
            border-radius: 10px;
            min-height: 45px;
        }
        #menuButton:hover {
            background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #7ac5cd, stop:1 #53868b);
        }
        #menuButton:pressed {
            background-color: #1a5276;
        }
        QMessageBox {
            color: #34495e;
            font-size: 15px;
        }
    )";
    qApp->setStyleSheet(styleSheet);
}

void MainMenu::onPlayClicked()
{
    clickSound->setPosition(0); // Перемотка звука в начало
    clickSound->play();

    // Создаем окно выбора сложности
    DifficultySelectionWindow *diffWindow = new DifficultySelectionWindow(this);
    // WA_DeleteOnClose означает, что память освободится сама при закрытии окна
    diffWindow->setAttribute(Qt::WA_DeleteOnClose);

    // Используем лямбда-функцию (анонимную функцию) для обработки результата
    connect(diffWindow, &DifficultySelectionWindow::difficultySelected, this, [this](GameDifficulty* difficulty){
        // Этот код выполнится, когда сложность будет выбрана
        menuBGMPlayer->stop();
        this->hide(); // Скрываем главное меню

        // Создаем окно игры с выбранной сложностью
        MemoryGameWindow *gameWindow = new MemoryGameWindow(difficulty);
        gameWindow->setAttribute(Qt::WA_DeleteOnClose);

        connect(gameWindow, &MemoryGameWindow::gameWon, this, &MainMenu::onGameWon);
        connect(gameWindow, &MemoryGameWindow::gameLost, this, &MainMenu::onGameLost);
        connect(gameWindow, &QWidget::destroyed, this, &MainMenu::onGameWindowClosed);

        gameWindow->show();
    });

    diffWindow->show();
}

void MainMenu::onStylesClicked()
{
    clickSound->setPosition(0);
    clickSound->play();

    StylesWindow *stylesWindow = new StylesWindow(coins, this);
    stylesWindow->setAttribute(Qt::WA_DeleteOnClose);

    // Подписываемся на изменение монет, чтобы обновить их в меню сразу после покупки
    connect(stylesWindow, &StylesWindow::coinsChanged, this, [this](int newCoins) {
        this->coins = newCoins;
        this->updateCoinLabel();
        this->saveCoins();
    });

    stylesWindow->show();
}

void MainMenu::onSettingsClicked()
{
    clickSound->setPosition(0);
    clickSound->play();
    SettingsWindow settingsWindow(this);
    // exec() запускает окно в модальном режиме (блокирует остальные окна пока открыто)
    settingsWindow.exec();
    applyAudioSettings(); // Применяем настройки сразу после закрытия окна настроек
}

void MainMenu::onGameWon(int moves, double multiplier)
{
    // Расчет награды: чем меньше ходов, тем больше монет (но не меньше 500)
    int baseReward = qMax(500, 1000 - moves * 10);
    // Умножаем на коэффициент сложности
    int finalReward = static_cast<int>(baseReward * multiplier);

    coins += finalReward;
    saveCoins();

    QMessageBox::information(this, "Победа!",
                             QString("Поздравляем! Вы нашли все пары за %1 ходов.\nНаграда: %2 💰")
                                 .arg(moves).arg(finalReward));
    updateCoinLabel();
}

void MainMenu::onGameLost(int pairsFound, double multiplier)
{
    int baseReward = pairsFound * 50;
    int finalReward = static_cast<int>(baseReward * multiplier);

    coins += finalReward;
    saveCoins();

    QMessageBox::information(this, "Поражение",
                             QString("Игра окончена.\nНаграда: %2 💰").arg(pairsFound).arg(finalReward));
    updateCoinLabel();
}

void MainMenu::onGameWindowClosed()
{
    this->show(); // Показываем меню снова
    loadCoins();
    updateCoinLabel();
    menuBGMPlayer->play();
}
