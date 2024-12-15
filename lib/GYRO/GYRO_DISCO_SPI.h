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
#define SCALING_FACTOR (17.5f * 0.0174532925199432957692236907684886f / 1000.0f)

// Window Size for Moving Average Window
#define DEFAULT_WINDOW_SIZE 10

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
     * @param average_type - The type of average to use. Defaults to MOVING_AVERAGE.
     * @return None
     */
    void init(AverageType average_type = MOVING_AVERAGE, int window_size = DEFAULT_WINDOW_SIZE);

    /**
     * @brief Reads the GYRO sensor.
     * @param None
     * @return None
     */
    void read(float &gx, float &gy, float &gz);

private:
    SPI spi;
    AverageType average_type;
    // float gx, gy, gz;

    EventFlags flags;
    uint8_t write_buf[32], read_buf[32];

    uint16_t raw_gx, raw_gy, raw_gz;
    float window_gx[DEFAULT_WINDOW_SIZE], window_gy[DEFAULT_WINDOW_SIZE], window_gz[DEFAULT_WINDOW_SIZE];
    int window_index;
    int window_size;

    void spi_callback(int event);
    void data_callback();
};

#else
#error "This class must be used with DISCO_F429ZI board only."
#endif // TARGET_DISCO_F429ZI

#endif // __GYRO_DISCO_SPI_H