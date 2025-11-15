#include "settingswindow.h"

#include <QVBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QFont>
#include <QCoreApplication> // Для QCoreApplication::setOrganizationName/setApplicationName

// Константы для QSettings (должны совпадать с теми, что используются в mainmenu.cpp)
const QString ORGANIZATION_NAME = "AmNyamm";
const QString APPLICATION_NAME = "MemoryGame";

SettingsWindow::SettingsWindow(QWidget *parent)
    : QDialog(parent)
{
    // Установка имени организации и приложения для QSettings
    QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
    QCoreApplication::setApplicationName(APPLICATION_NAME);

    setupUI();
    loadSettings();
}

SettingsWindow::~SettingsWindow()
{
}

void SettingsWindow::setupUI()
{
    setWindowTitle("Настройки");
    setFixedSize(300, 200);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Заголовок
    QLabel *titleLabel = new QLabel("Настройки Аудио");
    QFont font = titleLabel->font();
    font.setPointSize(16);
    font.setBold(true);
    titleLabel->setFont(font);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setObjectName("settingsTitle");
    mainLayout->addWidget(titleLabel);

    // 1. Переключатель Музыки
    musicCheckBox = new QCheckBox("Фоновая музыка", this);
    musicCheckBox->setObjectName("settingCheckBox");
    mainLayout->addWidget(musicCheckBox);

    // 2. Переключатель Звука (эффектов)
    soundCheckBox = new QCheckBox("Звуковые эффекты", this);
    soundCheckBox->setObjectName("settingCheckBox");
    mainLayout->addWidget(soundCheckBox);

    mainLayout->addStretch(1); // "Пружина" для прижатия элементов к верху

    // Подключение слотов для немедленного сохранения
    connect(musicCheckBox, &QCheckBox::stateChanged, this, &SettingsWindow::onMusicToggle);
    connect(soundCheckBox, &QCheckBox::stateChanged, this, &SettingsWindow::onSoundToggle);

    // Применение базовых стилей
    QString styleSheet = R"(
        QDialog {
            background-color: #f0f0f0;
        }
        #settingsTitle {
            color: #2c3e50;
        }
        QCheckBox {
            font-size: 18px;
            padding: 5px;
        }
    )";
    this->setStyleSheet(styleSheet);

    setLayout(mainLayout);
}

void SettingsWindow::loadSettings()
{
    // QSettings автоматически найдет настройки по ORGANIZATION_NAME и APPLICATION_NAME
    QSettings settings;

    // Загрузка состояния музыки (по умолчанию: Включена - true)
    bool musicEnabled = settings.value(MusicKey, true).toBool();
    musicCheckBox->setChecked(musicEnabled);

    // Загрузка состояния звука (по умолчанию: Включен - true)
    bool soundEnabled = settings.value(SoundKey, true).toBool();
    soundCheckBox->setChecked(soundEnabled);
}

void SettingsWindow::onMusicToggle(int state)
{
    QSettings settings;
    // Сохраняем состояние как bool (true/false)
    settings.setValue(MusicKey, (state == Qt::Checked));
}

void SettingsWindow::onSoundToggle(int state)
{
    QSettings settings;
    // Сохраняем состояние как bool (true/false)
    settings.setValue(SoundKey, (state == Qt::Checked));
}
