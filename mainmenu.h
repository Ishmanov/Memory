#ifndef MAINMENU_H
#define MAINMENU_H

#include <QWidget>
#include <QMediaPlayer> // Для воспроизведения музыки
#include <QSoundEffect>
#include <QAudioOutput> // Для управления громкостью в Qt 6

// Предварительное объявление классов (Forward declaration)
// Это ускоряет компиляцию, мы просто говорим компилятору, что такие классы существуют
class QPushButton;
class QLabel;
class MemoryGameWindow;
class QSettings;
class QCloseEvent;
class SettingsWindow;

// Класс главного меню
class MainMenu : public QWidget
{
    Q_OBJECT

public:
    // Конструктор. parent = nullptr означает, что у этого окна нет родителя (оно главное)
    explicit MainMenu(QWidget *parent = nullptr);

    ~MainMenu();

protected:
    // Переопределяем стандартное событие закрытия окна
    // Это нужно, чтобы успеть сохранить монеты перед выходом
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onPlayClicked();
    void onStylesClicked();
    void onSettingsClicked();

    // Слот, который вызовется при победе
    void onGameWon(int moves, double multiplier);

    // Слот, который вызовется при поражении
    void onGameLost(int pairsFound, double multiplier);

    // Когда окно игры закроется, мы снова покажем меню
    void onGameWindowClosed();

    // Обновляет громкость звука
    void applyAudioSettings();

private:
    void setupUI();
    void applyStyles();

    // Функции для работы с сохранениями (монеты)
    void loadCoins();
    void saveCoins();
    void updateCoinLabel();

    // --- Переменные класса ---
    int coins;
    QLabel *coinLabel;

    // Плееры для фоновой музыки и звуков клика
    QMediaPlayer *menuBGMPlayer;
    QAudioOutput *menuAudioOutput;
    QMediaPlayer *clickSound;
    QAudioOutput *clickAudioOutput;
};

#endif
