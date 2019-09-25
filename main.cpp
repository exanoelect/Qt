#include "Widget.h"
#include <SDL2/SDL.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SDL_Init(SDL_INIT_JOYSTICK);
    Widget w;
    w.show();
    //w.showFullScreen();
    return a.exec();
}
