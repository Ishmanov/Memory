#include "styleswindow.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>

// Константы стилей
const int STYLE_COST = 10000;
const int TOTAL_STYLES = 3; // Количество доступных стилей

StylesWindow::StylesWindow(int currentCoins, QWidget *parent)
    : QDialog(parent)
    , currentCoins(currentCoins)
    , settings("AmNyamm", "MemoryGame")
{
    setupUI();
    applyStyles();
    refreshGrid();
}

StylesWindow::~StylesWindow()
{
}

void StylesWindow::setupUI()
{
    setWindowTitle("Магазин Стилей");
    setFixedSize(600, 700);

    // Главная вертикальная компоновка
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Верхняя горизонтальная компоновка (для счетчика монет)
    QHBoxLayout *topBarLayout = new QHBoxLayout();

    QLabel *title = new QLabel("Выберите стиль карт");
    title->setStyleSheet("font-size: 24px; font-weight: bold; color: #480607;");
    topBarLayout->addWidget(title);

    topBarLayout->addStretch(1);

    // Метка для счетчика монет
    coinDisplayLabel = new QLabel(QString("💰 %1").arg(currentCoins));
    coinDisplayLabel->setObjectName("coinDisplayLabel");
    topBarLayout->addWidget(coinDisplayLabel);

    mainLayout->addLayout(topBarLayout);

    // Область с прокруткой или просто виджет для сетки
    gridContainer = new QWidget();
    stylesGridLayout = new QGridLayout(gridContainer);
    stylesGridLayout->setSpacing(15);

    mainLayout->addWidget(gridContainer, 1); // Растягиваем сетку

    setLayout(mainLayout);
}

void StylesWindow::refreshGrid()
{
    // Очистка сетки перед перерисовкой
    QLayoutItem *child;
    while ((child = stylesGridLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->deleteLater();
        }
        delete child;
    }

    // Получаем список купленных стилей и текущий выбранный
    // Формат unlocked_styles: строка "1,2,3"
    QString unlockedStr = settings.value("unlocked_styles", "1").toString();
    QStringList unlockedList = unlockedStr.split(",");

    int currentStyle = settings.value("current_style", 1).toInt();

    // Генерация карточек стилей
    // Стиль 1
    stylesGridLayout->addWidget(createStyleCard(1, 0, "Ам-Ням", "#7ED957"), 0, 0);

    // Стиль 2
    stylesGridLayout->addWidget(createStyleCard(2, 10000, "Океан", "#4facfe"), 0, 1);

    // Стиль 3
    stylesGridLayout->addWidget(createStyleCard(3, 10000, "Закат", "#fa709a"), 1, 0);

    // Стиль 4
    stylesGridLayout->addWidget(createStyleCard(4, 10000, "Закат", "#ffff99"), 1, 1);
}

QWidget* StylesWindow::createStyleCard(int styleId, int cost, const QString& name, const QString& colorHex)
{
    QWidget *card = new QWidget();
    card->setFixedSize(160, 220);

    // Определяем состояние
    QString unlockedStr = settings.value("unlocked_styles", "1").toString();
    QStringList unlockedList = unlockedStr.split(",");
    bool isUnlocked = unlockedList.contains(QString::number(styleId));
    int currentStyle = settings.value("current_style", 1).toInt();
    bool isSelected = (currentStyle == styleId);

    // Стиль карточки
    QString border = isSelected ? "4px solid #f1c40f" : "2px solid #555";
    card->setStyleSheet(QString(
                            "QWidget { background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #98f5ff, stop:1 #7ac5cd); border-radius: 10px; border: %1; }"
                            "QLabel { border: none; color: #800020; }"
                            ).arg(border));

    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(10, 10, 10, 10);

    // 1. Изображение (Превью) - берем "style - image1.png"
    QLabel *imgLabel = new QLabel();
    imgLabel->setFixedSize(130, 100);
    imgLabel->setAlignment(Qt::AlignCenter);

    // Путь к картинке
    QString imgPath = QString("://images/%1 - image1.png").arg(styleId);
    QPixmap pix(imgPath);
    if (!pix.isNull()) {
        imgLabel->setPixmap(pix.scaled(130, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        imgLabel->setText("Нет картинки\n" + imgPath);
        imgLabel->setStyleSheet("font-size: 10px; color: #aaa;");
    }
    layout->addWidget(imgLabel);

    // 2. Название
    QLabel *nameLabel = new QLabel(name);
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setStyleSheet("font-weight: bold; font-size: 14px; margin-top: 5px;");
    layout->addWidget(nameLabel);

    // 3. Кнопка действия
    QPushButton *actionBtn = new QPushButton();
    actionBtn->setCursor(Qt::PointingHandCursor);

    if (isSelected) {
        actionBtn->setText("Выбрано");
        actionBtn->setEnabled(false);
        actionBtn->setStyleSheet("background-color: #27ae60; color: white; border: none; border-radius: 5px; padding: 5px;");
    } else if (isUnlocked) {
        actionBtn->setText("Выбрать");
        actionBtn->setStyleSheet("background-color: #3498db; color: white; border: none; border-radius: 5px; padding: 5px;");
        // Подключаем выбор
        connect(actionBtn, &QPushButton::clicked, this, [this, styleId](){
            onStyleClicked(styleId, 0);
        });
    } else {
        actionBtn->setText(QString("Купить\n%1").arg(cost));
        actionBtn->setStyleSheet("background-color: #e74c3c; color: white; border: none; border-radius: 5px; padding: 5px;");
        // Подключаем покупку
        connect(actionBtn, &QPushButton::clicked, this, [this, styleId, cost](){
            onStyleClicked(styleId, cost);
        });
    }

    layout->addWidget(actionBtn);

    return card;
}

void StylesWindow::onStyleClicked(int styleId, int cost)
{
    QString unlockedStr = settings.value("unlocked_styles", "1").toString();
    QStringList unlockedList = unlockedStr.split(",");
    bool isUnlocked = unlockedList.contains(QString::number(styleId));

    if (isUnlocked) {
        // Просто выбираем стиль
        settings.setValue("current_style", styleId);
        refreshGrid();
    } else {
        // Покупка
        if (currentCoins >= cost) {
            // Списываем монеты
            currentCoins -= cost;
            emit coinsChanged(currentCoins); // Уведомляем MainMenu

            // Сохраняем новые монеты в settings (дублируем логику MainMenu для надежности, или полагаемся на сигнал)
            settings.setValue("coins", currentCoins);

            // Открываем стиль
            unlockedList.append(QString::number(styleId));
            settings.setValue("unlocked_styles", unlockedList.join(","));

            // Сразу выбираем купленный стиль
            settings.setValue("current_style", styleId);

            // Обновляем UI
            coinDisplayLabel->setText(QString("💰 %1").arg(currentCoins));
            QMessageBox::information(this, "Успех", "Стиль успешно куплен!");
            refreshGrid();
        } else {
            QMessageBox::warning(this, "Ошибка", "Недостаточно монет!");
        }
    }
}

void StylesWindow::applyStyles()
{
    this->setStyleSheet(R"(
        QDialog {
            background-color: #5f9ea0;
            color: #800020;
            font-family: 'Segoe UI', Arial, sans-serif;
        }
        #coinDisplayLabel {
            font-size: 20px;
            font-weight: bold;
            color: #800020;
            background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #98f5ff, stop:1 #7ac5cd);
            padding: 8px 12px;
            border-radius: 15px;
            border: 2px solid #480607;
        }
    )");
}
