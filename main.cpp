#include "mainmenu.h" // Подключаем наше меню
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainMenu w; // Создаем экземпляр MainMenu
    w.show();

    return a.exec();
}
