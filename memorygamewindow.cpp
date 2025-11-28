#include "memorygamewindow.h"
#include <QSettings>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QRandomGenerator>
#include <algorithm>
#include <QDebug>
#include <QIcon>
#include <QMap>
#include <QCoreApplication>

const QString ORGANIZATION_NAME = "AmNyamm";
const QString APPLICATION_NAME = "MemoryGame";

// Загружаем настройки звука (вкл/выкл)
void MemoryGameWindow::applyAudioSettings()
{
    QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
    QCoreApplication::setApplicationName(APPLICATION_NAME);

    QSettings settings;

    // 1. Музыка
    bool musicEnabled = settings.value("audio/music_enabled", true).toBool();
    float musicVolume = musicEnabled ? 0.1f : 0.0f;

    if (gameAudioOutput) {
        gameAudioOutput->setVolume(musicVolume);
    }

    // 2. Звуковые эффекты
    bool soundEnabled = settings.value("audio/sound_enabled", true).toBool();
    float soundVolume = soundEnabled ? 0.8f : 0.0f;

    if (flipAudioOutput) flipAudioOutput->setVolume(soundVolume);
    if (victoryAudioOutput) victoryAudioOutput->setVolume(soundVolume);
    if (defeatAudioOutput) defeatAudioOutput->setVolume(soundVolume);
}

MemoryGameWindow::MemoryGameWindow(GameDifficulty* difficulty, QWidget *parent)
    : QMainWindow(parent)
    , currentDifficulty(difficulty)
    , gameBGMPlayer(new QMediaPlayer(this))
    , gameAudioOutput(new QAudioOutput(this))
    , flipPlayer(new QMediaPlayer(this))
    , flipAudioOutput(new QAudioOutput(this))
    , victoryPlayer(new QMediaPlayer(this))
    , victoryAudioOutput(new QAudioOutput(this))
    , defeatPlayer(new QMediaPlayer(this))
    , defeatAudioOutput(new QAudioOutput(this))
{
    this->setWindowIcon(QIcon(":/icons/game_icon.ico"));
    // Берем параметры из переданного объекта сложности
    rows = difficulty->getRows();
    cols = difficulty->getCols();
    memoryTime = difficulty->getMemoryTime();
    coinMultiplier = difficulty->getCoinMultiplier();
    gameTotalTime = difficulty->getGameTime();

    totalPairs = (rows * cols) / 2; // Количество пар = половина от всех карт

    setWindowTitle("Найди Пару!");

    // Рассчитываем размер окна под количество карт
    int width = cols * 110 + 40;
    int height = rows * 110 + 150;
    setMinimumSize(width, height);
    setMaximumSize(width + 50, height + 50);

    setStyleSheet("QMainWindow { background-color: #5f9ea0; }");

    // Таймер основного времени игры
    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &MemoryGameWindow::gameTimerTimeout);

    // Таймер для показа карт в начале
    tempShowTimer = new QTimer(this);
    tempShowTimer->setSingleShot(true); // Сработает только один раз
    tempShowTimer->setInterval(memoryTime * 1000); // Переводим секунды в миллисекунды
    connect(tempShowTimer, &QTimer::timeout, this, &MemoryGameWindow::hideAllCardsTimeout);

    // Таймер для переворота карт обратно при ошибке
    flipBackTimer = new QTimer(this);
    flipBackTimer->setSingleShot(true);
    flipBackTimer->setInterval(1000); // 1 секунда задержки
    connect(flipBackTimer, &QTimer::timeout, this, &MemoryGameWindow::flipBackTimeout);

    // --- Настройка Аудио ---
    gameBGMPlayer->setAudioOutput(gameAudioOutput);
    gameAudioOutput->setVolume(0.1f);
    gameBGMPlayer->setSource(QUrl("qrc:/audios/game_bgm.mp3"));
    gameBGMPlayer->setLoops(QMediaPlayer::Infinite);

    flipPlayer->setAudioOutput(flipAudioOutput);
    flipPlayer->setSource(QUrl("qrc:/audios/card_flip.mp3"));

    victoryPlayer->setAudioOutput(victoryAudioOutput);
    victoryPlayer->setSource(QUrl("qrc:/audios/victory.mp3"));

    defeatPlayer->setAudioOutput(defeatAudioOutput);
    defeatPlayer->setSource(QUrl("qrc:/audios/defeat.mp3"));

    applyAudioSettings();

    // Загружаем сохраненный стиль карт
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    currentStyleId = settings.value("current_style", 1).toInt();

    setupUI();
    startNewGame();
}

MemoryGameWindow::~MemoryGameWindow() {
    if (gameBGMPlayer->playbackState() == QMediaPlayer::PlayingState) {
        gameBGMPlayer->stop();
    }
    // Удаляем объект сложности вручную, так как мы создали его через new
    if (currentDifficulty) {
        delete currentDifficulty;
    }
}

// Возвращает CSS для кнопок (цвет и градиент рубашки)
QString MemoryGameWindow::getButtonStyle() {
    QString gradient;
    QString borderColor;

    if (currentStyleId == 2) {
        gradient = "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #4facfe, stop:1 #00f2fe)";
        borderColor = "#00c6fb";
    } else if (currentStyleId == 3) {
        gradient = "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #fa709a, stop:1 #fee140)";
        borderColor = "#fa709a";
    } else if (currentStyleId == 4) {
        gradient = "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 	#ffff99, stop:1 #78866b)";
        borderColor = "#ffff99";
    } else {
        // Дефолтный стиль
        gradient = "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FFB6F772, stop:1 #FF7ED957)";
        borderColor = "#FF3F9E2F";
    }

    return QString(
               "QPushButton {"
               "  background: %1;"
               "  border: 2px solid %2;"
               "  border-radius: 12px;"
               "  font-weight: bold;"
               "  font-size: 16px;"
               "  color: white;"
               "  padding: 4px;"
               "  margin: 5px;"
               "}"
               "QPushButton:hover { border-color: white; }"
               ).arg(gradient).arg(borderColor);
}

void MemoryGameWindow::setupUI() {
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // Верхняя панель с кнопкой и таймером
    QWidget* topPanel = new QWidget();
    topPanel->setStyleSheet("background-color: #5f9ea0; max-height: 50px;");
    QHBoxLayout* topLayout = new QHBoxLayout(topPanel);
    topLayout->setContentsMargins(10, 5, 10, 5);

    newGameButton = new QPushButton("Новая игра");
    newGameButton->setFixedSize(120, 30);
    connect(newGameButton, &QPushButton::clicked, this, &MemoryGameWindow::startNewGameClicked);

    newGameButton->setStyleSheet(getButtonStyle());

    attemptsLabel = new QLabel("Попытки: 0");
    attemptsLabel->setFont(QFont("Arial", 16));

    timerLabel = new QLabel(QString("Осталось: %1 сек").arg(gameTotalTime));
    timerLabel->setFont(QFont("Arial", 16));

    topLayout->addWidget(newGameButton);
    topLayout->addWidget(attemptsLabel);
    topLayout->addWidget(timerLabel);
    topLayout->addStretch(1);

    mainLayout->addWidget(topPanel);

    // Контейнер для сетки карт
    QWidget* gridContainer = new QWidget();
    mainGridLayout = new QGridLayout(gridContainer);
    mainGridLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->addWidget(gridContainer);
}

void MemoryGameWindow::startNewGame() {
    gameBGMPlayer->play();
    gameTimer->stop();
    tempShowTimer->stop();
    flipBackTimer->stop();

    // Очищаем сетку от старых кнопок
    if (mainGridLayout) {
        QLayoutItem *item;
        while ((item = mainGridLayout->takeAt(0)) != 0) {
            if (item->widget()) item->widget()->deleteLater(); // Удаляем виджет
            delete item; // Удаляем элемент лейаута
        }
    }

    // Пересоздаем массивы для хранения данных
    buttons.assign(rows, std::vector<QPushButton*>(cols, nullptr));
    imagePaths.assign(rows, std::vector<std::string>(cols, ""));
    selectedButtons.clear();
    loadedImages.clear();

    attempts = 0;
    mistakes = 0;
    matchedPairs = 0;
    timeLeft = gameTotalTime;
    gameStarted = false;

    attemptsLabel->setText("Попытки: 0");
    timerLabel->setText(QString("Осталось: %1 сек").arg(timeLeft));

    createGrid();
    fillImagePaths();
    showAllImagesTemporarily();
}

void MemoryGameWindow::createGrid() {
    mainGridLayout->setSpacing(10);
    QString style = getButtonStyle();

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            QPushButton* btn = new QPushButton();
            btn->setFixedSize(100, 100);
            // Запоминаем координаты кнопки внутри самой кнопки
            btn->setProperty("row", i);
            btn->setProperty("col", j);

            btn->setStyleSheet(style);

            connect(btn, &QPushButton::clicked, this, &MemoryGameWindow::handleButtonClick);
            mainGridLayout->addWidget(btn, i, j);
            buttons[i][j] = btn;
            btn->setEnabled(false); // Сначала кнопки неактивны (идет показ)
        }
    }
}

void MemoryGameWindow::fillImagePaths() {
    std::vector<std::string> names;

    // Заполняем список парами картинок
    for (int n = 0; n < totalPairs; ++n) {
        int imgIndex = n + 1;
        std::string path = "://images/" + std::to_string(currentStyleId) + " - image" + std::to_string(imgIndex) + ".png";
        names.push_back(path);
        names.push_back(path); // Добавляем дважды (для пары)
    }

    // Перемешиваем массив случайным образом
    std::shuffle(names.begin(), names.end(), *QRandomGenerator::global());

    // Распределяем перемешанные картинки по сетке
    int index = 0;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            imagePaths[i][j] = names[index++];
        }
    }
}

void MemoryGameWindow::showImage(QPushButton* btn, const std::string& path) {
    // Кэшируем загрузку, чтобы не читать файл с диска каждый раз
    if (loadedImages.find(path) == loadedImages.end()) {
        loadedImages[path] = QPixmap(QString::fromStdString(path));
    }

    QPixmap originalPixmap = loadedImages[path];
    if (originalPixmap.isNull()) {
        btn->setText("Image\nNot Found");
        return;
    }
    btn->setText("");

    QSize buttonSize(100, 100);
    // Масштабируем картинку под размер кнопки
    QPixmap scaledPixmap = originalPixmap.scaled(buttonSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QIcon icon(scaledPixmap);
    btn->setIcon(icon);
    btn->setIconSize(buttonSize);
}

void MemoryGameWindow::showAllImagesTemporarily() {
    newGameButton->setEnabled(false);
    enableAllButtons(false);

    // Показываем картинки на всех кнопках
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            showImage(buttons[i][j], imagePaths[i][j]);
        }
    }
    // Запускаем таймер, который скроет их
    tempShowTimer->start();
}

void MemoryGameWindow::hideAllCardsTimeout() {
    // Скрываем картинки (ставим пустую иконку)
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            buttons[i][j]->setIcon(QIcon());
            buttons[i][j]->setEnabled(true); // Теперь можно нажимать
        }
    }
    newGameButton->setEnabled(true);
    startCountdown();
}

void MemoryGameWindow::startCountdown() {
    gameStarted = true;
    gameTimer->start(1000); // Запуск таймера (тик раз в секунду)
}

void MemoryGameWindow::gameTimerTimeout() {
    timeLeft--;
    timerLabel->setText(QString("Осталось: %1 сек").arg(timeLeft));

    if (timeLeft <= 0) {
        gameTimer->stop();
        showGameOver("Время вышло!");
    }
}

void MemoryGameWindow::handleButtonClick() {
    // Если идет анимация возврата карт или игра не идет - игнорируем клик
    if (!gameStarted || flipBackTimer->isActive()) return;

    flipPlayer->setPosition(0);
    flipPlayer->play();

    // sender() возвращает указатель на объект, который послал сигнал (нажатую кнопку)
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (!btn || !btn->isEnabled()) return;

    // Достаем координаты из свойств кнопки
    int i = btn->property("row").toInt();
    int j = btn->property("col").toInt();

    showImage(btn, imagePaths[i][j]);
    btn->setEnabled(false); // Блокируем нажатую кнопку
    selectedButtons.push_back(btn);

    // Если открыто 2 карты
    if (selectedButtons.size() == 2) {
        attempts++;
        attemptsLabel->setText(QString("Попытки: %1").arg(attempts));

        QPushButton* btn1 = selectedButtons[0];
        QPushButton* btn2 = selectedButtons[1];

        int i1 = btn1->property("row").toInt(), j1 = btn1->property("col").toInt();
        int i2 = btn2->property("row").toInt(), j2 = btn2->property("col").toInt();

        // Если картинки не совпали
        if (imagePaths[i1][j1] != imagePaths[i2][j2]) {
            mistakes++;
            if (mistakes >= MAX_MISTAKES) {
                gameTimer->stop();
                showGameOver("Слишком много ошибок!");
                return;
            }
            enableAllButtons(false); // Блокируем всё поле, чтобы игрок не тыкал дальше
            flipBackTimer->start(); // Ждем секунду перед переворотом обратно
        } else {
            // Если совпали
            matchedPairs++;

            btn1->setEnabled(false);
            btn2->setEnabled(false);

            // Подсвечиваем совпавшие (зеленым)
            btn1->setStyleSheet("border: 4px solid blue; background-color: #aaf0aa;");
            btn2->setStyleSheet("border: 4px solid blue; background-color: #aaf0aa;");

            QSize buttonSize(100, 100);
            btn1->setIconSize(buttonSize);
            btn2->setIconSize(buttonSize);

            selectedButtons.clear();
            if (matchedPairs == totalPairs) {
                gameTimer->stop();
                showVictoryScreen();
            }
        }
    }
}

void MemoryGameWindow::flipBackTimeout() {
    // Переворачиваем выбранные карты обратно рубашкой вверх
    for (QPushButton* b : selectedButtons) {
        b->setIcon(QIcon());
        b->setEnabled(true);
    }
    selectedButtons.clear();
    flipBackTimer->stop();
    enableAllButtons(true); // Разблокируем поле
}

void MemoryGameWindow::enableAllButtons(bool enable) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            QPushButton* btn = buttons[i][j];
            // Не включаем кнопки, которые уже найдены и отключены
            if (!btn->isEnabled() && !enable) continue;

            bool isSelected = false;
            // Проверяем, не является ли кнопка одной из сейчас открытых
            for (QPushButton* selBtn : selectedButtons) {
                if (btn == selBtn) {
                    isSelected = true;
                    break;
                }
            }
            if (!isSelected) btn->setEnabled(enable);
        }
    }
}

void MemoryGameWindow::showVictoryScreen() {
    gameTimer->stop();
    enableAllButtons(false);
    gameBGMPlayer->stop();
    victoryPlayer->setPosition(0);
    victoryPlayer->play();
    emit gameWon(attempts, coinMultiplier);
    this->close();
}

void MemoryGameWindow::showGameOver(const QString& reason) {
    Q_UNUSED(reason); // Макрос чтобы компилятор не ругался на неиспользуемую переменную
    gameTimer->stop();
    enableAllButtons(false);
    gameBGMPlayer->stop();
    defeatPlayer->setPosition(0);
    defeatPlayer->play();
    emit gameLost(matchedPairs, coinMultiplier);
    this->close();
}

void MemoryGameWindow::startNewGameClicked() {
    if (gameBGMPlayer->playbackState() == QMediaPlayer::PlayingState) {
        gameBGMPlayer->stop();
    }
    startNewGame();
}
