#ifndef DUJOYSTICKMANAGER_H
#define DUJOYSTICKMANAGER_H

#include <QThread>
#include <iostream>
#include <queue>

using namespace std;

enum JoystickButton
{
    BUTTON0,
    BUTTON1,
    BUTTON2,
    BUTTON3,
    BUTTON4,
    BUTTON5,
    BUTTON6,
    BUTTON7,
    BUTTON8,
    BUTTON9,
    BUTTON10,
    BUTTON11,

    HAT_0,
    HAT_UP,
    HAT_DOWN,
    HAT_LEFT,
    HAT_RIGHT,

    X,
    Y,
    Z,
    RZ
};

enum JoystickAxis
{
    AXIS_HORIZONTAL,
    AXIS_VERTICAL,
    CAXIS_HORIZONTAL,
    CAXIS_VERTICAL
};

struct cmdStruct{
     //uint8_t cmdCount;
     uint8_t cmdType;
     int16_t cmdValue;
};

class DuJoystickManager : public QThread
{
    Q_OBJECT
public:
    DuJoystickManager(QObject *parent = nullptr);
    //cmdStruct stickCmd[50];
    //uint8_t cmdCount = 0;
    bool mutexObject = false;
    queue <cmdStruct> queueCmd;
    mutex cmdMutex;
signals:
    void SDL_joyButtonDown(int);
    //void SDL_joyAxisMotion(int);
protected:
    void run() override;
};

#endif // DUJOYSTICKMANAGER_H
