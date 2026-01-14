#include "settingswindow.h"

#include <QVBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QFont>
#include <QCoreApplication>

const QString ORGANIZATION_NAME = "AmNyamm";
const QString APPLICATION_NAME = "MemoryGame";

SettingsWindow::SettingsWindow(QWidget *parent)
    : QDialog(parent)
{
    // Устанавливаем данные, чтобы QSettings знал, куда сохранять настройки в реестре/файле
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

    QLabel *titleLabel = new QLabel("Настройки");
    QFont font = titleLabel->font();
    font.setPointSize(16);
    font.setBold(true);
    titleLabel->setFont(font);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setObjectName("settingsTitle");
    mainLayout->addWidget(titleLabel);

    // Чекбокс (галочка) для музыки
    musicCheckBox = new QCheckBox("Фоновая музыка", this);
    musicCheckBox->setObjectName("settingCheckBox");
    mainLayout->addWidget(musicCheckBox);

    // Чекбокс для звуков
    soundCheckBox = new QCheckBox("Звуковые эффекты", this);
    soundCheckBox->setObjectName("settingCheckBox");
    mainLayout->addWidget(soundCheckBox);

    mainLayout->addStretch(1);

    // stateChanged срабатывает при постановке/снятии галочки
    connect(musicCheckBox, &QCheckBox::stateChanged, this, &SettingsWindow::onMusicToggle);
    connect(soundCheckBox, &QCheckBox::stateChanged, this, &SettingsWindow::onSoundToggle);

    QString styleSheet = R"(
        QDialog {
            background-color: #5f9ea0;
        }
        #settingsTitle {
            color: #480607;
            font-size: 25px;
            font-weight: 600;
            margin-bottom: 10px;
        }
        QCheckBox {
            background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #98f5ff, stop:1 #7ac5cd);
            font-size: 18px;
            font-weight: 600;
            padding: 5px;
        }
    )";
    this->setStyleSheet(styleSheet);

    setLayout(mainLayout);
}

void SettingsWindow::loadSettings()
{
    // Создаем объект настроек. Он сам найдет файл конфигурации
    QSettings settings(qApp->applicationDirPath() + "/save.ini", QSettings::IniFormat);

    // Читаем значение. Если его нет, вернем true (по умолчанию включено)
    bool musicEnabled = settings.value(MusicKey, true).toBool();
    musicCheckBox->setChecked(musicEnabled);

    bool soundEnabled = settings.value(SoundKey, true).toBool();
    soundCheckBox->setChecked(soundEnabled);
}

void SettingsWindow::onMusicToggle(int state)
{
    QSettings settings(qApp->applicationDirPath() + "/save.ini", QSettings::IniFormat);
    // Сохраняем: если галочка стоит (Qt::Checked), то true, иначе false
    settings.setValue(MusicKey, (state == Qt::Checked));
}

void SettingsWindow::onSoundToggle(int state)
{
    QSettings settings(qApp->applicationDirPath() + "/save.ini", QSettings::IniFormat);
    settings.setValue(SoundKey, (state == Qt::Checked));
}
