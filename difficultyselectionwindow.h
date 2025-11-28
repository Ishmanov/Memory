#ifndef DIFFICULTYSELECTIONWINDOW_H
#define DIFFICULTYSELECTIONWINDOW_H

#include <QDialog>
#include "difficulties.h"

// Этот класс отвечает за всплывающее окно выбора сложности
// Мы наследуемся от QDialog, так как это диалоговое окно
class DifficultySelectionWindow : public QDialog
{
    // Макрос Q_OBJECT обязателен для любого класса, который использует сигналы и слоты
    Q_OBJECT

public:
    explicit DifficultySelectionWindow(QWidget *parent = nullptr);
    ~DifficultySelectionWindow();

signals:
    // Сигнал — это сообщение, которое этот класс отправляет другим частям программы
    // Здесь мы сообщаем, какую сложность выбрал игрок
    void difficultySelected(GameDifficulty* difficulty);

private slots:
    // Слоты — это функции, которые запускаются в ответ на сигналы (например, нажатие кнопки)
    void onEasyClicked();
    void onMediumClicked();
    void onHardClicked();

private:
    void setupUI();
    void applyStyles();
};

#endif // DIFFICULTYSELECTIONWINDOW_H
