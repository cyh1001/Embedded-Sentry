// /*
//  * Project: STM32F429-Gesture-Lock-NYU-RTES
//  * Course: ECE-GY 6483 Real Time Embedded Systems
//  * School: NYU Tandon School of Engineering
//  * Team Members:
//  * 1. Yihan Cao (yc7683)
//  * 2. Xianda Wu (xw3444)
//  * 3. Yihao (Ethan) Wang (yw7486)
//  *
//  * A gesture-based authentication system using STM32F429I-DISC1's I3G4250D gyroscope.
//  * Part of the Embedded Sentry security system project.
//  */

#include "mbed.h"
#include "GYRO_DISCO_SPI.h"

#include "main.h"
#include "screen.h"
#include "gesture.h"

// Gyroscope Declaration
GYRO_DISCO_SPI gyro;

// Arrays to store movement sequences
double key_pattern[MATCH_SEQ_LENGTH][3] = {0};   // Array to store key gesture
double trial_pattern[MATCH_SEQ_LENGTH][3] = {0}; // Array to store trial gesture

// State variables
bool is_capturing = false;
bool is_matching = false;
uint8_t sample_count = 0;
bool pattern_stored = false;

int main()
{
    gyro.init(MOVING_AVERAGE, 10);

    init_screen();
    draw_ui();

    gyro.calibrate(100);

    while(true) {
        GYRO_DISCO_SPI::GyroData motion = gyro.read();
        show_data(motion);
        process_touch(pattern_stored, is_capturing, is_matching, sample_count);

        if(is_capturing || is_matching) {
            if (sample_count < MATCH_SEQ_LENGTH)
            {
                double *pattern = is_capturing ? key_pattern[sample_count] : trial_pattern[sample_count];
                for (int i = 0; i < 3; i++)
                {
                    pattern[i] = motion.data[i];
                }

                printf("Sample %d: %f, %f, %f\n", sample_count, pattern[0], pattern[1], pattern[2]);

                show_count(sample_count);
                sample_count++;
            }
            else
            {
                if(is_capturing) {
                    pattern_stored = true;
                    update_status("Pattern recorded!");
                } else {
                    bool matched = match_gesture(key_pattern, trial_pattern);
                    show_result(matched);
                }
                is_capturing = false;
                is_matching = false;
                sample_count = 0;
                thread_sleep_for(1500);
                draw_ui();
            }
        }

        thread_sleep_for(20);
    }
}
