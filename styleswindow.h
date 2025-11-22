#ifndef STYLESWINDOW_H
#define STYLESWINDOW_H

#include <QDialog>
#include <QGridLayout>
#include <QSettings>

class QLabel;
class QPushButton;

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

signals:
    // Сигнал, сообщающий главному меню, что количество монет изменилось
    void coinsChanged(int newCoins);

private slots:
    // Слот для обработки нажатия на стиль (выбор или покупка)
    void onStyleClicked(int styleId, int cost);

private:
    /**
     * Настраивает UI окна.
     */
    void setupUI();

    // Обновляет сетку стилей (перерисовывает кнопки в зависимости от состояния)
    void refreshGrid();

    // Применяет стили QSS к этому окну.
    void applyStyles();

    // Вспомогательный метод для создания карточки стиля
    QWidget* createStyleCard(int styleId, int cost, const QString& name, const QString& colorHex);

    int currentCoins;
    QLabel *coinDisplayLabel; // Метка для отображения монет
    QGridLayout *stylesGridLayout; // Сетка для стилей
    QWidget *gridContainer; // Контейнер для сетки

    // Настройки
    QSettings settings;
};

#endif
