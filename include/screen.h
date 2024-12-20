#pragma once

#ifndef __SCREEN_H
#define __SCREEN_H

#ifdef TARGET_DISCO_F429ZI

#include "mbed.h"
#include "TS_DISCO_F429ZI.h"
#include "LCD_DISCO_F429ZI.h"
#include "GYRO_DISCO_SPI.h"

#define RECORD_BUTTON_X_MIN 20
#define VERIFY_BUTTON_X_MIN 130

#define BUTTON_Y_MIN 150
#define BUTTON_WIDTH 90
#define BUTTON_HEIGHT 60

#define TEXT_X_OFFSET 10
#define TEXT_Y_OFFSET 20

void init_screen();

void draw_ui();

void update_status(const char *status);

void show_data(const GYRO_DISCO_SPI::GyroData &motion);

void show_count(uint8_t count);

void show_result(bool success);

uint16_t map_touch_x(uint16_t x);

uint16_t map_touch_y(uint16_t y);

void process_touch(bool pattern_stored, bool &is_capturing, bool &is_matching, uint8_t &sample_count);

#else
#error "This class must be used with DISCO_F429ZI board only."
#endif // TARGET_DISCO_F429ZI

#endif // __SCREEN_H
