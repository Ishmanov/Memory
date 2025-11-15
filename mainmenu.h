#ifndef MAINMENU_H
#define MAINMENU_H

#include <QWidget>
#include <QMediaPlayer> // Для фоновой музыки
#include <QSoundEffect> // Для звуковых эффектов
#include <QAudioOutput> // НОВОЕ: Для управления громкостью QMediaPlayer в Qt 6

class QPushButton;
class QLabel; // Теперь нужен, т.к. добавили в класс
class MemoryGameWindow;
class QSettings;
class QCloseEvent; // Для protected-метода

class SettingsWindow;

// Класс главного меню.
class MainMenu : public QWidget
{
    Q_OBJECT // Макрос Qt для работы с сигналами и слотами

public:
    /**
     * Конструктор класса MainMenu
     * parent Родительский виджет (обычно nullptr для главного окна)
     */
    explicit MainMenu(QWidget *parent = nullptr);

    ~MainMenu();

protected:
    // ВОТ ОДНО ИЗ МЕСТ, ГДЕ НУЖНА БЫЛА РЕАЛИЗАЦИЯ:
    // Используется для сохранения монет при выходе.
    void closeEvent(QCloseEvent *event) override;

private slots:
    // Слот, вызываемый при нажатии кнопки Играть
    void onPlayClicked();

    // Слот, вызываемый при нажатии кнопки Стили
    void onStylesClicked();

    // Слот, вызываемый при нажатии кнопки Настройки
    void onSettingsClicked();

    // ВОТ ОДНО ИЗ МЕСТ, ГДЕ НУЖНА БЫЛА РЕАЛИЗАЦИЯ:
    // Слот, вызываемый, когда игра сообщает о победе.
    // moves Количество ходов, затраченных на победу.
    void onGameWon(int moves);

    // ВОТ ОДНО ИЗ МЕСТ, ГДЕ НУЖНА БЫЛА РЕАЛИЗАЦИЯ:
    // Слот, вызываемый, когда игра сообщает о поражении.
    // pairsFound Количество найденных пар до поражения.
    void onGameLost(int pairsFound);

    // ВОТ ОДНО ИЗ МЕСТ, ГДЕ НУЖНА БЫЛА РЕАЛИЗАЦИЯ:
    // Слот, вызываемый при закрытии окна игры.
    void onGameWindowClosed();

    // НОВОЕ: Слот для загрузки и применения настроек аудио (громкость BGM)
    void applyAudioSettings();

private:
    // Настраивает пользовательский интерфейс (UI) виджета
    void setupUI();

    // Применяет стили QSS к виджету
    void applyStyles();

    // Загружает, сохраняет и обновляет количество монет.
    void loadCoins();
    void saveCoins();
    void updateCoinLabel(); // Вспомогательный метод для обновления метки монет

    // --- Члены класса ---
    int coins;              // Текущее количество монет
    QLabel *coinLabel;      // Метка для отображения монет

    QMediaPlayer *menuBGMPlayer; // Фоновая музыка меню
    QAudioOutput *menuAudioOutput; // НОВОЕ: Выход для аудио в Qt 6
    QMediaPlayer *clickSound;    // Звук нажатия кнопки
    QAudioOutput *clickAudioOutput;
};

#endif
