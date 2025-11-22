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

class MemoryGameWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MemoryGameWindow(QWidget *parent = nullptr);
    ~MemoryGameWindow();

signals:
    void gameWon(int moves);
    void gameLost(int pairsFound);

private slots:
    void handleButtonClick();
    void startNewGameClicked();
    void gameTimerTimeout();
    void hideAllCardsTimeout();
    void flipBackTimeout();

private:
    void applyAudioSettings();
    void setupUI();
    void createGrid();
    void fillImagePaths();
    void showAllImagesTemporarily();
    void startCountdown();
    void showImage(QPushButton* button, const std::string& path);
    void showGameOver(const QString& reason);
    void showVictoryScreen();
    void enableAllButtons(bool enable);

    // Вспомогательный метод для получения стиля кнопок (цвета рубашки)
    QString getButtonStyle();

    void startNewGame();

    // --- Константы ---
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
    int currentStyleId = 1; // ID текущего стиля

    // --- UI и Данные ---
    QLabel* attemptsLabel;
    QLabel* timerLabel;
    QPushButton* newGameButton;
    QGridLayout* mainGridLayout;
    QWidget* centralWidget;

    std::vector<std::vector<QPushButton*>> buttons;
    std::vector<std::vector<std::string>> imagePaths;
    std::vector<QPushButton*> selectedButtons;

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
};

#endif
