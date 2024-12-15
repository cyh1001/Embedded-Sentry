#include "BUTTON.h"

ButtonPressDetector::ButtonPressDetector(
    PinName pin,
    int longPressThreshold)
    : button(*new InterruptIn(pin)),
      longPressThreshold(longPressThreshold)
{
    button.rise(callback(this, &ButtonPressDetector::onPress));
    button.fall(callback(this, &ButtonPressDetector::onRelease));
}

ButtonPressDetector::~ButtonPressDetector()
{
    delete &button;
}

void ButtonPressDetector::reset()
{
    timer.reset();
    buttonReleased = false;
    pressDuration = 0;
}

void ButtonPressDetector::onPress()
{
    timer.start();
    buttonReleased = false;
}

void ButtonPressDetector::onRelease()
{
    timer.stop();
    pressDuration = std::chrono::duration_cast<std::chrono::milliseconds>(timer.elapsed_time()).count(); // equivalent to Timer::read_ms(); Timer::read_ms() is deprecated
    buttonReleased = true;
}

bool ButtonPressDetector::released()
{
    return buttonReleased;
}

bool ButtonPressDetector::isShortPress()
{
    return pressDuration < longPressThreshold;
}

bool ButtonPressDetector::isLongPress()
{
    return pressDuration >= longPressThreshold;
}
