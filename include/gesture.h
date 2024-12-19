#pragma once

#ifndef __GESTURE_H
#define __GESTURE_H

#ifdef TARGET_DISCO_F429ZI

#define MATCH_THRESHOLD 1.0f

// Time window for matching the gesture in seconds
#define MATCH_SEQ_LENGTH 2

#include "GYRO_DISCO_SPI.h"

#define MATCH_WINDOW_SIZE (MATCH_SEQ_LENGTH * SAMPLE_RATE)

void reset_gesture();

bool match_gesture();

void record_gesture(GYRO_DISCO_SPI &gyro, bool is_key_sequence = false);

#else
#error "This class must be used with DISCO_F429ZI board only."
#endif // TARGET_DISCO_F429ZI

#endif // __GESTURE_H
