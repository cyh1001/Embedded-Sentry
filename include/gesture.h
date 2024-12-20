#pragma once

#ifndef __GESTURE_H
#define __GESTURE_H

#ifdef TARGET_DISCO_F429ZI

#define MATCH_THRESHOLD 0.65f

// Time window for matching the gesture in seconds
#define MATCH_SEQ_LENGTH 30

#include "GYRO_DISCO_SPI.h"

bool match_gesture(double key_pattern[MATCH_SEQ_LENGTH][3], double trial_pattern[MATCH_SEQ_LENGTH][3]);

#else
#error "This class must be used with DISCO_F429ZI board only."
#endif // TARGET_DISCO_F429ZI

#endif // __GESTURE_H
