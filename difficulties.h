#ifndef DIFFICULTIES_H
#define DIFFICULTIES_H

#include <QString>

// Базовый класс для параметров сложности
class GameDifficulty {
public:
    virtual ~GameDifficulty() = default;

    virtual int getRows() const = 0;
    virtual int getCols() const = 0;
    virtual int getMemoryTime() const = 0; // Время показа карт в начале (сек)
    virtual int getGameTime() const = 0;   // Общее время на игру (сек)
    virtual double getCoinMultiplier() const = 0; // Множитель монет
    virtual QString getName() const = 0;
};

// Лёгкая сложность
// Поле 4x4, запоминание 10 сек, время 90 сек, меньше монет
class EasyDifficulty : public GameDifficulty {
public:
    int getRows() const override { return 4; }
    int getCols() const override { return 4; }
    int getMemoryTime() const override { return 10; }
    int getGameTime() const override { return 90; } // 90 секунд
    double getCoinMultiplier() const override { return 1.0 / 1.5; }
    QString getName() const override { return "Easy"; }
};

// Средняя сложность
// Поле 5x4, запоминание 7 сек, время 75 сек, множитель 1.0
class MediumDifficulty : public GameDifficulty {
public:
    int getRows() const override { return 5; }
    int getCols() const override { return 4; }
    int getMemoryTime() const override { return 7; }
    int getGameTime() const override { return 75; } // 75 секунд
    double getCoinMultiplier() const override { return 1.0; }
    QString getName() const override { return "Medium"; }
};

// Сложная сложность
// Поле 6x4, запоминание 4 сек, время 60 сек, больше монет
class HardDifficulty : public GameDifficulty {
public:
    int getRows() const override { return 6; }
    int getCols() const override { return 4; }
    int getMemoryTime() const override { return 4; }
    int getGameTime() const override { return 60; } // 60 секунд
    double getCoinMultiplier() const override { return 1.5; }
    QString getName() const override { return "Hard"; }
};

#endif // DIFFICULTIES_H
