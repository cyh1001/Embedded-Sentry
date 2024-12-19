#include "GYRO_DISCO_SPI.h"

GYRO_DISCO_SPI::GYRO_DISCO_SPI() : spi(PF_9, PF_8, PF_7, PC_1, use_gpio_ssel)
{
}

GYRO_DISCO_SPI::~GYRO_DISCO_SPI()
{
    delete &spi;

    delete[] window_gx;
    delete[] window_gy;
    delete[] window_gz;
}

void GYRO_DISCO_SPI::write_reg(uint8_t reg, uint8_t val) {
    write_buf[0] = reg;
    write_buf[1] = val;
    spi.transfer(write_buf, 2, read_buf, 2, callback(this, &GYRO_DISCO_SPI::spi_callback));
    flags.wait_all(SPI_FLAG);
}

void GYRO_DISCO_SPI::init(AverageType average_type, int window_size, bool debug)
{
    this->debug = debug;

    this->average_type = average_type;
    this->window_size = window_size;
    this->timeout = SAMPLE_TIMEOUT;

    if (average_type == MOVING_AVERAGE)
    {
        window_gx = new float[window_size];
        window_gy = new float[window_size];
        window_gz = new float[window_size];

        for (int i = 0; i < window_size; ++i)
        {
            window_gx[i] = 0.0f;
            window_gy[i] = 0.0f;
            window_gz[i] = 0.0f;
        }
    }

    spi.format(8, 0);
    spi.frequency(1000000);

    // Configuration
    write_reg(CTRL_REG1, CTRL_REG1_CONFIG);
    write_reg(CTRL_REG2, CTRL_REG2_CONFIG);
    // write_reg(CTRL_REG3, CTRL_REG3_CONFIG);
    write_reg(CTRL_REG4, CTRL_REG4_CONFIG);
    write_reg(CTRL_REG5, CTRL_REG5_CONFIG);

    timer.start();
}

void GYRO_DISCO_SPI::read_raw()
{
    // Wait for data ready flag (timeout 500 ms)
    flags.wait_all(DATA_READY_FLAG, 500, true);

    // Read GYRO Data using SPI transfer --> 6 bytes!
    write_buf[0] = OUT_X_L | 0x80 | 0x40;
    spi.transfer(write_buf, 7, read_buf, 7, callback(this, &GYRO_DISCO_SPI::spi_callback));
    flags.wait_all(SPI_FLAG);

    // Extract raw 16-bit gyroscope data for X, Y, Z
    raw_gx = (((uint16_t)read_buf[2]) << 8) | ((uint16_t)read_buf[1]);
    raw_gy = (((uint16_t)read_buf[4]) << 8) | ((uint16_t)read_buf[3]);
    raw_gz = (((uint16_t)read_buf[6]) << 8) | ((uint16_t)read_buf[5]);
}

GYRO_DISCO_SPI::GyroData GYRO_DISCO_SPI::read(bool calibrate)
{
    while (std::chrono::duration_cast<std::chrono::milliseconds>(timer.elapsed_time()).count() < timeout)
    {
        // wait for data ready flag
    }
    flags.set(DATA_READY_FLAG);

    read_raw();
    timer.reset();

    GyroData data;
    data.raw[0] = raw_gx;
    data.raw[1] = raw_gy;
    data.raw[2] = raw_gz;

    // Convert raw data to radians per second!
    data.data[0] = ((float)raw_gx) * SCALING_FACTOR;
    data.data[1] = ((float)raw_gy) * SCALING_FACTOR;
    data.data[2] = ((float)raw_gz) * SCALING_FACTOR;

    if (calibrate)
    {
        data.data[0] -= gx_offset;
        data.data[1] -= gy_offset;
        data.data[2] -= gz_offset;
    }

    if (debug)
        printf("gx = %8.5f, gy = %8.5f, gz = %8.5f\n", data.data[0], data.data[1], data.data[2]);

    // Moving Average FIR
    if (average_type == MOVING_AVERAGE)
    {
        cache_sum_gx += (data.data[0] - window_gx[window_index]);
        cache_sum_gy += (data.data[1] - window_gy[window_index]);
        cache_sum_gz += (data.data[2] - window_gz[window_index]);
        window_gx[window_index] = data.data[0];
        window_gy[window_index] = data.data[1];
        window_gz[window_index] = data.data[2];

        data.data[0] = cache_sum_gx / window_size;
        data.data[1] = cache_sum_gy / window_size;
        data.data[2] = cache_sum_gz / window_size;
        window_index = (window_index + 1) % window_size;
    }

    return data;
}

void GYRO_DISCO_SPI::calibrate(int samples)
{
    printf("========== Calibrating ==========\n");
    printf("Please keep the sensor still for %d samples...\n", samples);

    float sum_gx = 0, sum_gy = 0, sum_gz = 0;

    for (int i = 0; i < samples; i++)
    {
        GyroData data = read(false);
        if (debug)
            printf("Sample %d: gx: %f, gy: %f, gz: %f\n", i, data.data[0], data.data[1], data.data[2]);
        sum_gx += data.data[0];
        sum_gy += data.data[1];
        sum_gz += data.data[2];
    }
    gx_offset = sum_gx / samples;
    gy_offset = sum_gy / samples;
    gz_offset = sum_gz / samples;

    flags.set(DATA_READY_FLAG);

    printf("========== Calibration complete! ==========\n");
    printf("X Offset: %f, Y Offset: %f, Z Offset: %f\n", gx_offset, gy_offset, gz_offset);
}

// Callback function for SPI Transfer Completion
void GYRO_DISCO_SPI::spi_callback(int event)
{
    flags.set(SPI_FLAG);
}

// Callback function for Data Ready Interrupt
void GYRO_DISCO_SPI::data_callback()
{
    flags.set(DATA_READY_FLAG);
}