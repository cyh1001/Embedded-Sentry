#pragma once

#ifndef __GYRO_DISCO_SPI_H
#define __GYRO_DISCO_SPI_H

#ifdef TARGET_DISCO_F429ZI

#include "mbed.h"

// Control Register 1
#define CTRL_REG1 0x20
#define CTRL_REG1_CONFIG 0b01'10'1'1'1'1

// Control Register 4
#define CTRL_REG4 0x23
#define CTRL_REG4_CONFIG 0b0'0'01'0'00'0

// Control Register 3
#define CTRL_REG3 0x22
#define CTRL_REG3_CONFIG 0b0'0'0'0'1'000

// Output Register --> X axis
#define OUT_X_L 0x28

// Define Flag bits for the EventFlags object
#define SPI_FLAG 1
#define DATA_READY_FLAG 2

// Scaling Factor for data conversion dps --> rps (make sure its the right vale?!)
#define SCALING_FACTOR 17.5f * 0.0175f / 1000.0f

// Window Size for Moving Average Window
#define DEFAULT_WINDOW_SIZE 10

// Data sampling
#define SAMPLE_RATE 20 // Hz
#define SAMPLE_TIMEOUT 1000 / SAMPLE_RATE // ms

// Calibration Samples
#define CALIBRATION_SAMPLES 100

enum AverageType
{
    NO_AVERAGE,
    MOVING_AVERAGE
};

/*
    This class drives the SPI interface for the GYRO sensor present on DISCO_F429ZI board.

    Usage:

    #include "mbed.h"
    #include "GYRO_DISCO_SPI.h"

    GYRO_DISCO_SPI gyro;

    int main()
    {
        gyro.init();
        while(1)
        {
            gyro.read();
            printf("gx: %4.5f, gy: %4.5f, gz: %4.5f\n", gyro.gx, gyro.gy, gyro.gz);
            thread_sleep_for(50);
        }
    }
*/
class GYRO_DISCO_SPI
{
public:
    GYRO_DISCO_SPI();
    ~GYRO_DISCO_SPI();

    /**
     * @brief Initializes the GYRO sensor.
     * @param average_type - The type of average to use. Defaults to NO_AVERAGE.
     * @param window_size - The size of the window for moving average. Defaults to DEFAULT_WINDOW_SIZE.
     * @param debug - Whether to enable debug mode. Defaults to false.
     * @return None
     */
    void init(AverageType average_type = NO_AVERAGE, int window_size = DEFAULT_WINDOW_SIZE, bool debug = false);

    /**
     * @brief Calibrates the GYRO sensor.
     * @param samples - The number of samples to take for calibration. Defaults to 100.
     * @return None
     */
    void calibrate(int samples = CALIBRATION_SAMPLES);

    /**
     * @brief Reads the GYRO sensor.
     * @param gx - The X axis data.
     * @param gy - The Y axis data.
     * @param gz - The Z axis data.
     * @param calibrate - Whether to use the calibration offsets. Defaults to true.
     * @return None
     */
    void read(float &gx, float &gy, float &gz, bool calibrate = true);

private:
    SPI spi;
    AverageType average_type;

    Timer timer;
    int timeout;

    EventFlags flags;
    uint8_t write_buf[32], read_buf[32];

    int16_t raw_gx, raw_gy, raw_gz;
    float gx_offset = 0, gy_offset = 0, gz_offset = 0;
    float *window_gx, *window_gy, *window_gz;
    int window_size;
    int window_index = 0;
    float cache_sum_gx = 0.0f, cache_sum_gy = 0.0f, cache_sum_gz = 0.0f;

    bool debug = false;

    void spi_callback(int event);
    void data_callback();

    /**
     * @brief Reads the original GYRO sensor data.
     * @param None
     * @return None
     */
    void read_raw();
};

#else
#error "This class must be used with DISCO_F429ZI board only."
#endif // TARGET_DISCO_F429ZI

#endif // __GYRO_DISCO_SPI_H