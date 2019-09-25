#include "DuJoystickManager.h"
#include <SDL2/SDL.h>
#include <QDebug>
#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <mutex>          // std::mutex

using namespace std;

DuJoystickManager::DuJoystickManager(QObject *parent)
    : QThread(parent)
{
    SDL_JoystickOpen(0);
}


//Use PUSH to QUEUE to store joyStick value
//FRONT to get first value, in widget.cpp
//POP to delete, in widget.cpp
void DuJoystickManager::run()
{
    while (true) {
        SDL_Event event;
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_JOYBUTTONDOWN) { //digital button
               emit SDL_joyButtonDown(event.jbutton.button);
               qDebug() << "Btn " << event.jbutton.button << "\n";
               std::lock_guard<std::mutex> guard(cmdMutex);
               queueCmd.push(cmdStruct{event.jbutton.button,0});
            } else if (event.type == SDL_JOYAXISMOTION) {
                if(event.jaxis.axis == AXIS_HORIZONTAL){
                    //qDebug() << "AXIS_X " << event.jaxis.value << "\n";
                    emit SDL_joyButtonDown(X);
                    std::lock_guard<std::mutex> guard(cmdMutex);
                    queueCmd.push(cmdStruct{X,event.jaxis.value});
                }else{
                    if(event.jaxis.axis == AXIS_VERTICAL){
                        //qDebug() << "AXIS_Y " << event.jaxis.value << "\n";
                        emit SDL_joyButtonDown(Y);
                        std::lock_guard<std::mutex> guard(cmdMutex);
                        queueCmd.push(cmdStruct{Y,event.jaxis.value});
                    }else{
                        if(event.jaxis.axis == CAXIS_HORIZONTAL){
                            //qDebug() << "CAXIS_Z" << event.jaxis.value << "\n" ;
                            emit SDL_joyButtonDown(Z);
                            std::lock_guard<std::mutex> guard(cmdMutex);
                            queueCmd.push(cmdStruct{Z,event.jaxis.value});
                        }else{
                            if(event.jaxis.axis == CAXIS_VERTICAL){
                                //qDebug() << "CAXIS_RZ" << event.jaxis.value << "\n" ;
                                emit SDL_joyButtonDown(RZ);
                                std::lock_guard<std::mutex> guard(cmdMutex);
                                queueCmd.push(cmdStruct{RZ,event.jaxis.value});
                            }
                        }
                    }
                }
            }else{
                if (event.type == SDL_JOYHATMOTION){ //digital button
                    if(event.jhat.value == 0){
                        qDebug() << "HAT_" << event.jhat.value << "\n" ;
                        emit SDL_joyButtonDown(HAT_0);
                        lock_guard<mutex> guard(cmdMutex);
                        queueCmd.push(cmdStruct{HAT_0,0});
                    }else{
                        if(event.jhat.value == 1){
                            qDebug() << "HAT_" << event.jhat.value << "\n" ;
                            emit SDL_joyButtonDown(HAT_UP);
                            std::lock_guard<std::mutex> guard(cmdMutex);
                            queueCmd.push(cmdStruct{HAT_UP,1});
                        }else{
                            if(event.jhat.value == 2){
                                qDebug() << "HAT_" << event.jhat.value << "\n" ;
                                emit SDL_joyButtonDown(HAT_RIGHT);
                                std::lock_guard<std::mutex> guard(cmdMutex);
                                queueCmd.push(cmdStruct{HAT_RIGHT,2});
                            }else{
                                if(event.jhat.value == 4){
                                    qDebug() << "HAT_" << event.jhat.value << "\n" ;
                                    emit SDL_joyButtonDown(HAT_DOWN);
                                    std::lock_guard<std::mutex> guard(cmdMutex);
                                    queueCmd.push(cmdStruct{HAT_DOWN,4});
                                }else{
                                    if(event.jhat.value == 8){
                                        qDebug() << "HAT_" << event.jhat.value << "\n" ;
                                        emit SDL_joyButtonDown(HAT_LEFT);
                                        std::lock_guard<std::mutex> guard(cmdMutex);
                                        queueCmd.push(cmdStruct{HAT_LEFT,8});
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
