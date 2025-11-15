#include "styleswindow.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIcon> // Для иконки

// Конструктор StylesWindow.
StylesWindow::StylesWindow(int currentCoins, QWidget *parent)
    : QDialog(parent)
{
    setupUI(currentCoins);
    applyStyles();
}

// Деструктор StylesWindow.
StylesWindow::~StylesWindow()
{
}

// Настройка UI.
void StylesWindow::setupUI(int coins)
{
    setWindowTitle("Магазин Стилей");
    setFixedSize(500, 400);

    // Главная вертикальная компоновка
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);

    // Верхняя горизонтальная компоновка (для счетчика монет)
    QHBoxLayout *topBarLayout = new QHBoxLayout();
    topBarLayout->setContentsMargins(0, 0, 0, 10);

    // "Пружина", толкающая счетчик вправо
    topBarLayout->addStretch(1);

    // Метка для счетчика монет
    // Используем Unicode-эмодзи монеты 💰
    coinDisplayLabel = new QLabel(QString("💰 %1").arg(coins));
    coinDisplayLabel->setObjectName("coinDisplayLabel");
    coinDisplayLabel->setToolTip("Ваши монеты");

    // Добавляем счетчик в верхнюю компоновку
    topBarLayout->addWidget(coinDisplayLabel);

    // 3. Центральная часть (заглушка)
    QLabel *placeholderLabel = new QLabel("Здесь будет магазин стилей\n(покупка новых рубашек для карт и т.д.)");
    placeholderLabel->setObjectName("placeholderLabel");
    placeholderLabel->setAlignment(Qt::AlignCenter);

    // 4. Сборка
    mainLayout->addLayout(topBarLayout);   // Добавляем верхнюю панель
    mainLayout->addWidget(placeholderLabel, 1); // Добавляем заглушку (растягивается)

    setLayout(mainLayout);
}

// Применение стилей QSS.
void StylesWindow::applyStyles()
{
    // Стили для окна стилей
    this->setStyleSheet(R"(
        /* Фон самого окна */
        QDialog {
            background-color: #3c4a3d; /* Темно-синий (темнее меню) */
            color: #1e2b38;
            font-family: 'Segoe UI', Arial, sans-serif;
        }

        /* Стиль для счетчика монет в углу */
        #coinDisplayLabel {
            font-size: 20px;
            font-weight: bold;
            color: #f1c40f; /* Золотой цвет */
            background-color: #2c3e50; /* Фон меню */
            padding: 8px 12px;
            border-radius: 15px;
            border: 2px solid #f1c40f;
        }

        /* Стиль для текста-заглушки */
        #placeholderLabel {
            font-size: 18px;
            color: #1e2b38; /* Серый */
        }
    )");
}
