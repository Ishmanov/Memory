#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include <QSettings>

class QCheckBox;

// Окно настроек с галочками для включения музыки и звуков
class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

private slots:
    // Слоты для сохранения состояния чекбоксов
    void onMusicToggle(int state);
    void onSoundToggle(int state);

private:
    void setupUI();
    void loadSettings();

    QCheckBox *musicCheckBox;
    QCheckBox *soundCheckBox;

    // Ключи (имена переменных), под которыми настройки хранятся в системе
    const QString MusicKey = "audio/music_enabled";
    const QString SoundKey = "audio/sound_enabled";
};

#endif // SETTINGSWINDOW_H
