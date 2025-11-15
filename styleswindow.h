#ifndef STYLESWINDOW_H
#define STYLESWINDOW_H

#include <QDialog> // Используем QDialog для модального или немодального окна

class QLabel;

// @brief Окно "Стили" (Магазин).
class StylesWindow : public QDialog
{
    Q_OBJECT

public:
    /**
     * Конструктор.
     * currentCoins Текущее количество монет для отображения.
     * parent Родительский виджет.
     */
    explicit StylesWindow(int currentCoins, QWidget *parent = nullptr);
    ~StylesWindow();

private:
    /**
     * Настраивает UI окна.
     * coins Количество монет для отображения.
     */
    void setupUI(int coins);

    // Применяет стили QSS к этому окну.
    void applyStyles();

    QLabel *coinDisplayLabel; // Метка для отображения монет
};

#endif
