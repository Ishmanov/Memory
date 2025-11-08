#include "memorygamewindow.h" // Include the correct window header
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Original line: MainWindow w;
    // New line:
    MemoryGameWindow w; // <--- Change to MemoryGameWindow

    w.show();
    return a.exec();
}
