#ifndef STYLESWINDOW_H
#define STYLESWINDOW_H

#include <QDialog>
#include <QGridLayout>
#include <QSettings>

class QLabel;
class QPushButton;

// Окно "Стили" (Магазин), где можно купить новые рубашки карт
class StylesWindow : public QDialog
{
    Q_OBJECT

public:
    explicit StylesWindow(int currentCoins, QWidget *parent = nullptr);
    ~StylesWindow();

signals:
    // Сообщаем главному окну, что количество монет изменилось (после покупки)
    void coinsChanged(int newCoins);

private slots:
    // Обработка клика по карточке товара
    void onStyleClicked(int styleId, int cost);

private:
    void setupUI();

    // Перерисовка сетки товаров (обновление кнопок "Купить"/"Выбрать")
    void refreshGrid();

    void applyStyles();

    // Создает виджет одной карточки товара
    QWidget* createStyleCard(int styleId, int cost, const QString& name, const QString& colorHex);

    int currentCoins;
    QLabel *coinDisplayLabel;
    QGridLayout *stylesGridLayout;
    QWidget *gridContainer;

    QSettings settings;
};

#endif
