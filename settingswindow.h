#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include <QSettings>

class QCheckBox; // Используем forward declaration

/**
 * @brief Окно настроек, содержащее переключатели для звука и музыки.
 */
class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

private slots:
    // Слот для сохранения состояния переключателя музыки
    void onMusicToggle(int state);

    // Слот для сохранения состояния переключателя звука
    void onSoundToggle(int state);

private:
    // Настраивает UI окна.
    void setupUI();

    // Загружает сохраненные настройки и применяет их к чекбоксам.
    void loadSettings();

    QCheckBox *musicCheckBox;
    QCheckBox *soundCheckBox;

    // Ключи для QSettings
    const QString MusicKey = "audio/music_enabled";
    const QString SoundKey = "audio/sound_enabled";
};

#endif // SETTINGSWINDOW_H
