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

// QTimer - это аналог DispatcherTimer в C#

class MemoryGameWindow : public QMainWindow
{
    Q_OBJECT // Обязательно для использования сигналов и слотов

public:
    explicit MemoryGameWindow(QWidget *parent = nullptr);
    ~MemoryGameWindow();

private slots:
    void handleButtonClick();       // Обработчик нажатия на игровую кнопку
    void startNewGameClicked();     // Обработчик кнопки "Новая игра"
    void gameTimerTimeout();        // Слот для основного игрового таймера (каждую секунду)
    void hideAllCardsTimeout();     // Слот для таймера, скрывающего карты после показа
    void flipBackTimeout();         // Слот для таймера, переворачивающего несовпавшие карты

private:
    void setupUI();
    void createGrid();
    void fillImagePaths();
    void showAllImagesTemporarily();
    void startCountdown();
    void showImage(QPushButton* button, const std::string& path);
    void showGameOver(const QString& reason);
    void showVictoryScreen();
    void enableAllButtons(bool enable);

    void startNewGame();

    // --- Константы (Конфигурация Игры) ---
    const int ROWS = 5;
    const int COLS = 4;
    const int TOTAL_PAIRS = 10;
    const int MAX_MISTAKES = 10;
    const int TOTAL_TIME = 90;
    const int MEMORY_TIME = 7;

    // --- Игровое Состояние ---
    int attempts = 0;
    int matchedPairs = 0;
    int mistakes = 0;
    int timeLeft = TOTAL_TIME;
    bool gameStarted = false;

    // --- UI и Данные ---
    QLabel* attemptsLabel;
    QLabel* timerLabel;
    QPushButton* newGameButton;
    QGridLayout* mainGridLayout;
    QWidget* centralWidget;

    std::vector<std::vector<QPushButton*>> buttons;
    std::vector<std::vector<std::string>> imagePaths;
    std::vector<QPushButton*> selectedButtons;

    // Используем map для кеширования загруженных QPixmap
    std::map<std::string, QPixmap> loadedImages;

    // --- Таймеры ---
    QTimer* gameTimer;      // Основной таймер игры
    QTimer* tempShowTimer;  // Таймер для временного показа карт
    QTimer* flipBackTimer;  // Таймер для задержки при несовпадении
};

#endif // MEMORYGAMEWINDOW_H
