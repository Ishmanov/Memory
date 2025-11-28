#ifndef MEMORYGAMEWINDOW_H
#define MEMORYGAMEWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QTimer>
#include <QTime>
#include <QPixmap>
#include <vector>
#include <string>
#include <map>

#include <QMediaPlayer>
#include <QAudioOutput>

// Подключаем определение сложностей
#include "difficulties.h"

// Основной класс окна самой игры
class MemoryGameWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MemoryGameWindow(GameDifficulty* difficulty, QWidget *parent = nullptr);
    ~MemoryGameWindow();

signals:
    // Сигналы для сообщения главному меню о результате
    void gameWon(int moves, double coinMultiplier);
    void gameLost(int pairsFound, double coinMultiplier);

private slots:
    // Обработка клика по карточке
    void handleButtonClick();
    void startNewGameClicked();
    // Срабатывает каждую секунду
    void gameTimerTimeout();
    // Срабатывает, когда нужно скрыть карты после предпросмотра
    void hideAllCardsTimeout();
    // Срабатывает, когда нужно перевернуть карты обратно (если не совпали)
    void flipBackTimeout();

private:
    void applyAudioSettings();
    void setupUI();
    void createGrid(); // Создание сетки кнопок
    void fillImagePaths(); // Заполнение массива путями к картинкам
    void showAllImagesTemporarily(); // Показ всех карт в начале
    void startCountdown();
    void showImage(QPushButton* button, const std::string& path);
    void showGameOver(const QString& reason);
    void showVictoryScreen();
    // Блокировка/разблокировка всех кнопок
    void enableAllButtons(bool enable);

    QString getButtonStyle();

    void startNewGame();

    // --- Параметры Игры ---
    int rows; // Строки
    int cols; // Колонки
    int totalPairs; // Всего пар для поиска
    int memoryTime; // Время на запоминание в начале
    int gameTotalTime; // Общее время на игру
    double coinMultiplier;

    // Максимальное число ошибок перед проигрышем
    const int MAX_MISTAKES = 10;

    // --- Игровое Состояние ---
    int attempts = 0; // Количество попыток
    int matchedPairs = 0; // Найдено пар
    int mistakes = 0; // Сделано ошибок
    int timeLeft = 0;
    bool gameStarted = false;
    int currentStyleId = 1;

    // --- Элементы интерфейса ---
    QLabel* attemptsLabel;
    QLabel* timerLabel;
    QPushButton* newGameButton;
    QGridLayout* mainGridLayout;
    QWidget* centralWidget;

    // Двумерный массив кнопок (само поле)
    std::vector<std::vector<QPushButton*>> buttons;
    // Двумерный массив путей к картинкам (что скрыто под кнопкой)
    std::vector<std::vector<std::string>> imagePaths;
    // Список нажатых кнопок (сейчас может быть 0, 1 или 2)
    std::vector<QPushButton*> selectedButtons;

    // Кэш загруженных картинок, чтобы не грузить их с диска каждый раз
    std::map<std::string, QPixmap> loadedImages;

    // --- Таймеры ---
    QTimer* gameTimer;
    QTimer* tempShowTimer;
    QTimer* flipBackTimer;

    // --- Аудио ---
    QMediaPlayer *gameBGMPlayer;
    QAudioOutput *gameAudioOutput;

    QMediaPlayer *flipPlayer;
    QAudioOutput *flipAudioOutput;

    QMediaPlayer *victoryPlayer;
    QAudioOutput *victoryAudioOutput;

    QMediaPlayer *defeatPlayer;
    QAudioOutput *defeatAudioOutput;

    // Указатель на объект сложности
    GameDifficulty* currentDifficulty;
};

#endif
