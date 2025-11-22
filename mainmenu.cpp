#include "mainmenu.h"
#include "memorygamewindow.h"
#include "styleswindow.h"
#include "settingswindow.h"
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
    QSettings settings("AmNyamm", "MemoryGame");
    coins = settings.value("coins", 1000).toInt();
}

void MainMenu::saveCoins()
{
    QSettings settings("AmNyamm", "MemoryGame");
    settings.setValue("coins", coins);
}

void MainMenu::updateCoinLabel()
{
    if (coinLabel) {
        coinLabel->setText(QString("Монеты: %1 💰").arg(coins));
    }
}

void MainMenu::applyAudioSettings()
{
    QSettings settings("AmNyamm", "MemoryGame");
    bool musicEnabled = settings.value("audio/music_enabled", true).toBool();
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

    menuBGMPlayer->setAudioOutput(menuAudioOutput);
    menuBGMPlayer->setSource(QUrl("qrc:/audios/menu_bgm.mp3"));
    menuBGMPlayer->setLoops(QMediaPlayer::Infinite);

    clickSound->setAudioOutput(clickAudioOutput);
    clickSound->setSource(QUrl("qrc:/audios/button_click.mp3"));

    applyAudioSettings();
    menuBGMPlayer->play();
}

MainMenu::~MainMenu()
{
}

void MainMenu::closeEvent(QCloseEvent *event)
{
    saveCoins();
    QWidget::closeEvent(event);
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
    clickSound->setPosition(0);
    clickSound->play();
    menuBGMPlayer->stop();
    this->hide();

    MemoryGameWindow *gameWindow = new MemoryGameWindow();
    gameWindow->setAttribute(Qt::WA_DeleteOnClose);

    connect(gameWindow, &MemoryGameWindow::gameWon, this, &MainMenu::onGameWon);
    connect(gameWindow, &MemoryGameWindow::gameLost, this, &MainMenu::onGameLost);
    connect(gameWindow, &QWidget::destroyed, this, &MainMenu::onGameWindowClosed);

    gameWindow->show();
}

void MainMenu::onStylesClicked()
{
    clickSound->setPosition(0);
    clickSound->play();

    StylesWindow *stylesWindow = new StylesWindow(coins, this);
    stylesWindow->setAttribute(Qt::WA_DeleteOnClose);

    // ВАЖНО: Подключаем обновление монет при покупке
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
    settingsWindow.exec();
    applyAudioSettings();
}

void MainMenu::onGameWon(int moves)
{
    int reward = qMax(500, 1000 - moves * 10);
    coins += reward;
    saveCoins();

    QMessageBox::information(this, "Победа!",
                             QString("Поздравляем! Вы нашли все пары за %1 ходов.\nНаграда: %2 💰").arg(moves).arg(reward));
    updateCoinLabel();
}

void MainMenu::onGameLost(int pairsFound)
{
    int reward = pairsFound * 50;
    coins += reward;
    saveCoins();

    QMessageBox::information(this, "Поражение",
                             QString("Игра окончена.\nНаграда: %2 💰").arg(pairsFound).arg(reward));
    updateCoinLabel();
}

void MainMenu::onGameWindowClosed()
{
    this->show();
    loadCoins();
    updateCoinLabel();
    menuBGMPlayer->play();
}
