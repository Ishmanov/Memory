#include "memorygamewindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QRandomGenerator>
#include <algorithm>
#include <QDebug>
#include <QIcon>

// --- Конструктор и Инициализация ---

MemoryGameWindow::MemoryGameWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Настройка основного окна
    setWindowTitle("Ам-Ням: Найди Пару!");
    setMinimumSize(490, 500);
    setMaximumSize(500, 650);

    // Установка желтого фона, как в WPF
    setStyleSheet("QMainWindow { background-color: #fdee02; }");

    // Инициализация таймеров
    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &MemoryGameWindow::gameTimerTimeout);

    tempShowTimer = new QTimer(this);
    tempShowTimer->setSingleShot(true);
    tempShowTimer->setInterval(MEMORY_TIME * 1000); // 7 секунд
    connect(tempShowTimer, &QTimer::timeout, this, &MemoryGameWindow::hideAllCardsTimeout);

    flipBackTimer = new QTimer(this);
    flipBackTimer->setSingleShot(true);
    flipBackTimer->setInterval(1000); // 1 секунда
    connect(flipBackTimer, &QTimer::timeout, this, &MemoryGameWindow::flipBackTimeout);

    setupUI();
    startNewGame();
}

MemoryGameWindow::~MemoryGameWindow() {}

// --- Функции UI и Инициализации ---

void MemoryGameWindow::setupUI() {
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // Верхняя панель (DockPanel Dock=Top)
    QWidget* topPanel = new QWidget();
    topPanel->setStyleSheet("background-color: LightGreen; max-height: 50px;");
    QHBoxLayout* topLayout = new QHBoxLayout(topPanel);
    topLayout->setContentsMargins(10, 5, 10, 5); // Аналог Margin

    newGameButton = new QPushButton("Новая игра");
    newGameButton->setFixedSize(120, 30);
    connect(newGameButton, &QPushButton::clicked, this, &MemoryGameWindow::startNewGameClicked);

    // Применение стиля из XAML (только самые необходимые свойства)
    QString buttonStyle =
        "QPushButton {"
        "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FFB6F772, stop:1 #FF7ED957);"
        "  border: 2px solid #FF3F9E2F;"
        "  border-radius: 12px;"
        "  font-weight: bold;"
        "  font-size: 16px;"
        "  color: DarkGreen;"
        "  padding: 4px;"
        "  margin: 5px;"
        "}"
        "QPushButton:hover { border-color: Green; }"; // Простая имитация DropShadowEffect

    newGameButton->setStyleSheet(buttonStyle);

    attemptsLabel = new QLabel("Попытки: 0");
    attemptsLabel->setFont(QFont("Arial", 16));

    timerLabel = new QLabel(QString("Осталось: %1 сек").arg(TOTAL_TIME));
    timerLabel->setFont(QFont("Arial", 16));

    topLayout->addWidget(newGameButton);
    topLayout->addWidget(attemptsLabel);
    topLayout->addWidget(timerLabel);
    topLayout->addStretch(1); // Добавляет пустое пространство справа

    mainLayout->addWidget(topPanel);

    // Основная сетка игры (Grid)
    QWidget* gridContainer = new QWidget();
    mainGridLayout = new QGridLayout(gridContainer);
    mainGridLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->addWidget(gridContainer);
}

void MemoryGameWindow::startNewGame() {
    gameTimer->stop();
    tempShowTimer->stop();
    flipBackTimer->stop();

    // Очистка старой сетки
    if (mainGridLayout) {
        QLayoutItem *item;
        while ((item = mainGridLayout->takeAt(0)) != 0) {
            if (item->widget()) {
                item->widget()->deleteLater();
            }
            delete item;
        }
    }

    // Переинициализация данных
    buttons.assign(ROWS, std::vector<QPushButton*>(COLS, nullptr));
    imagePaths.assign(ROWS, std::vector<std::string>(COLS, ""));
    selectedButtons.clear();
    loadedImages.clear();

    attempts = 0;
    mistakes = 0;
    matchedPairs = 0;
    timeLeft = TOTAL_TIME;
    gameStarted = false;

    // Обновление UI
    attemptsLabel->setText("Попытки: 0");
    timerLabel->setText(QString("Осталось: %1 сек").arg(TOTAL_TIME));

    createGrid();
    fillImagePaths();
    showAllImagesTemporarily();
}

void MemoryGameWindow::createGrid() {
    // Установка отступов для ячеек
    mainGridLayout->setSpacing(10);

    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            QPushButton* btn = new QPushButton();
            btn->setFixedSize(100, 100);

            // Сохранение координат в свойстве (аналог Tag="i,j" в WPF)
            btn->setProperty("row", i);
            btn->setProperty("col", j);

            // Стиль кнопки (тот же что и для newGameButton)
            btn->setStyleSheet(newGameButton->styleSheet());

            // Подключение сигнала нажатия к слоту
            connect(btn, &QPushButton::clicked, this, &MemoryGameWindow::handleButtonClick);

            mainGridLayout->addWidget(btn, i, j);
            buttons[i][j] = btn;

            // Отключаем кнопки перед временным показом
            btn->setEnabled(false);
        }
    }
}

void MemoryGameWindow::fillImagePaths() {
    std::vector<std::string> names;
    // 1. Создаем 20 путей (10 пар)
    for (int n = 1; n <= TOTAL_PAIRS; ++n) {
        // !!! ВАЖНО: Используем путь ресурсов Qt ":/префикс/имяфайла.png"
        std::string path = "://images/image" + std::to_string(n) + ".png";

        // Добавляем ПАРУ путей для каждой картинки
        names.push_back(path);
        names.push_back(path);
    }
    // names теперь ГАРАНТИРОВАННО содержит 20 элементов (10 пар)

    // 2. Перемешивание (аналог Concat и OrderBy(Guid.NewGuid()))
    std::shuffle(names.begin(), names.end(), *QRandomGenerator::global());

    // 3. Заполнение сетки (20 элементов)
    int index = 0;
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            // Условие 'if (index < names.size())' больше не нужно,
            // так как names.size() == ROWS * COLS
            imagePaths[i][j] = names[index++];
        }
    }
}

// --- Логика Игры ---

void MemoryGameWindow::showImage(QPushButton* btn, const std::string& path) {
    if (loadedImages.find(path) == loadedImages.end()) {
        // Загрузка и кэширование изображения
        loadedImages[path] = QPixmap(QString::fromStdString(path));
    }

    QPixmap originalPixmap = loadedImages[path];

    // Кнопки созданы с setFixedSize(100, 100)
    QSize buttonSize(100, 100);

    // Масштабирование QPixmap до размера кнопки (100x100)
    // Используем Qt::KeepAspectRatio, чтобы избежать искажения
    QPixmap scaledPixmap = originalPixmap.scaled(buttonSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QIcon icon(scaledPixmap);

    // Отображение изображения на кнопке
    btn->setIcon(icon);

    // Установка размера иконки, соответствующего размеру кнопки
    btn->setIconSize(buttonSize);
}

void MemoryGameWindow::showAllImagesTemporarily() {
    newGameButton->setEnabled(false);
    enableAllButtons(false);

    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            showImage(buttons[i][j], imagePaths[i][j]);
        }
    }

    // Запуск таймера временного показа
    tempShowTimer->start();
}

void MemoryGameWindow::hideAllCardsTimeout() {
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            buttons[i][j]->setIcon(QIcon()); // Скрыть изображение
            buttons[i][j]->setEnabled(true);
        }
    }
    newGameButton->setEnabled(true);

    startCountdown();
}

void MemoryGameWindow::startCountdown() {
    gameStarted = true;
    gameTimer->start(1000); // Интервал 1000 мс (1 секунда)
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
    if (!gameStarted || flipBackTimer->isActive()) return; // Защита от кликов во время задержки

    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (!btn || !btn->isEnabled()) return;

    int i = btn->property("row").toInt();
    int j = btn->property("col").toInt();

    showImage(btn, imagePaths[i][j]);
    btn->setEnabled(false);
    selectedButtons.push_back(btn);

    if (selectedButtons.size() == 2) {
        attempts++;
        attemptsLabel->setText(QString("Попытки: %1").arg(attempts));

        QPushButton* btn1 = selectedButtons[0];
        QPushButton* btn2 = selectedButtons[1];

        int i1 = btn1->property("row").toInt(), j1 = btn1->property("col").toInt();
        int i2 = btn2->property("row").toInt(), j2 = btn2->property("col").toInt();

        if (imagePaths[i1][j1] != imagePaths[i2][j2]) {
            // Не совпали
            mistakes++;
            if (mistakes >= MAX_MISTAKES) {
                gameTimer->stop();
                showGameOver("Слишком много ошибок!");
                return;
            }

            // Запуск таймера для скрытия несовпавших карт
            enableAllButtons(false); // Блокируем клики на время задержки
            flipBackTimer->start();
        } else {
            // Совпали
            matchedPairs++;

            btn1->setEnabled(false); // Отключаем кнопки навсегда
            btn2->setEnabled(false);

            btn1->setStyleSheet("border: 4px solid blue; background-color: #aaf0aa;");
            btn2->setStyleSheet("border: 4px solid blue; background-color: #aaf0aa;");

            QSize buttonSize(100, 100);
            btn1->setIconSize(buttonSize);
            btn2->setIconSize(buttonSize);

            selectedButtons.clear();
            if (matchedPairs == TOTAL_PAIRS) {
                gameTimer->stop();
                showVictoryScreen();
            }
        }
    }
}

void MemoryGameWindow::flipBackTimeout() {
    for (QPushButton* b : selectedButtons) {
        b->setIcon(QIcon()); // Скрыть изображение
        // Кнопка остается заблокированной, если она была частью пары, но это не наш случай (тут пара не совпала)
        b->setEnabled(true);
    }
    selectedButtons.clear();
    flipBackTimer->stop();
    enableAllButtons(true); // Разблокируем клики
}

void MemoryGameWindow::enableAllButtons(bool enable) {
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            QPushButton* btn = buttons[i][j];
            // Если кнопка уже отключена (найдена пара), не трогаем ее
            if (!btn->isEnabled() && !enable) {
                // Если мы отключаем все, и кнопка уже отключена, пропускаем
                continue;
            }

            // Проверяем, что кнопка не является одной из уже выбранных
            bool isSelected = false;
            for (QPushButton* selBtn : selectedButtons) {
                if (btn == selBtn) {
                    isSelected = true;
                    break;
                }
            }

            // Блокируем/разблокируем только те кнопки, которые НЕ ВЫБРАНЫ
            if (!isSelected) {
                btn->setEnabled(enable);
            }
        }
    }
}


// --- Экраны Завершения ---

void MemoryGameWindow::showVictoryScreen() {
    QMessageBox::information(this,
                             "Победа!",
                             QString("Ам-Ням счастлив! Вы победили!\nПопыток: %1").arg(attempts));
    enableAllButtons(false);
}

void MemoryGameWindow::showGameOver(const QString& reason) {
    QMessageBox::warning(this,
                         "Ам-Ням остался голодным",
                         QString("Игра окончена: %1").arg(reason));
    enableAllButtons(false);
}

void MemoryGameWindow::startNewGameClicked() {
    startNewGame();
}
