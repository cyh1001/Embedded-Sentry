#pragma once

#ifndef __BUTTON_H
#define __BUTTON_H

#ifdef TARGET_DISCO_F429ZI

#include "mbed.h"

class ButtonPressDetector
{
public:
    ButtonPressDetector(PinName pin = BUTTON1, int longPressThreshold = 1000);
    ~ButtonPressDetector();

    void reset();
    void onPress();
    void onRelease();
    bool released();
    bool isShortPress();
    bool isLongPress();

private:
    InterruptIn &button;
    const int longPressThreshold;
    
    Timer timer;
    bool buttonReleased;
    int pressDuration;
};

#else
#error "This class must be used with DISCO_F429ZI board only."
#endif // TARGET_DISCO_F429ZI

#endif // __BUTTON_H