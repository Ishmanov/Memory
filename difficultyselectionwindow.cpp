#include "difficultyselectionwindow.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QApplication>

DifficultySelectionWindow::DifficultySelectionWindow(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    applyStyles();
}

DifficultySelectionWindow::~DifficultySelectionWindow()
{
}

void DifficultySelectionWindow::setupUI()
{
    setWindowTitle("Выбор сложности");
    setFixedSize(350, 400);

    // Создаем вертикальный компоновщик (Layout)
    // Он будет автоматически расставлять элементы сверху вниз
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(20); // Расстояние между элементами
    layout->setContentsMargins(30, 30, 30, 30); // Отступы от краев окна

    QLabel *titleLabel = new QLabel("Выберите сложность");
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel); // Добавляем текст в компоновщик

    QPushButton *easyBtn = new QPushButton("Лёгкая\n(4x4, больше времени)");
    easyBtn->setObjectName("menuButton");

    // Соединяем нажатие кнопки (сигнал clicked) с нашей функцией (слот onEasyClicked)
    // Когда пользователь нажмет кнопку, выполнится код внутри onEasyClicked
    connect(easyBtn, &QPushButton::clicked, this, &DifficultySelectionWindow::onEasyClicked);
    layout->addWidget(easyBtn);

    QPushButton *mediumBtn = new QPushButton("Средняя\n(5x4, стандарт)");
    mediumBtn->setObjectName("menuButton");
    connect(mediumBtn, &QPushButton::clicked, this, &DifficultySelectionWindow::onMediumClicked);
    layout->addWidget(mediumBtn);

    QPushButton *hardBtn = new QPushButton("Сложная\n(6x4, меньше времени)");
    hardBtn->setObjectName("menuButton");
    connect(hardBtn, &QPushButton::clicked, this, &DifficultySelectionWindow::onHardClicked);
    layout->addWidget(hardBtn);

    // Добавляем "пружину" в конец, чтобы сдвинуть все элементы вверх, если останется место
    layout->addStretch();
}

void DifficultySelectionWindow::applyStyles()
{
    // Здесь мы задаем внешний вид (CSS-подобный синтаксис)
    // R"(...)" позволяет писать строку в несколько линий без лишних кавычек
    QString styleSheet = R"(
        QDialog {
            background-color: #5f9ea0;
            font-family: 'Segoe UI', 'Roboto', Arial, sans-serif;
            color: #480607;
        }
        #titleLabel {
            font-size: 24px;
            font-weight: bold;
            color: #480607;
            padding: 10px;
        }
        #menuButton {
            background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #98f5ff, stop:1 #7ac5cd);
            color: #800020;
            border: none;
            padding: 15px;
            font-size: 16px;
            font-weight: 600;
            border-radius: 10px;
            min-height: 50px;
        }
        #menuButton:hover {
            background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #7ac5cd, stop:1 #53868b);
        }
        #menuButton:pressed {
            background-color: #1a5276;
        }
    )";
    this->setStyleSheet(styleSheet);
}

void DifficultySelectionWindow::onEasyClicked()
{
    // Отправляем сигнал с созданным объектом сложности
    emit difficultySelected(new EasyDifficulty());
    // Закрываем окно с результатом "Успех"
    accept();
}

void DifficultySelectionWindow::onMediumClicked()
{
    emit difficultySelected(new MediumDifficulty());
    accept();
}

void DifficultySelectionWindow::onHardClicked()
{
    emit difficultySelected(new HardDifficulty());
    accept();
}
