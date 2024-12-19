#include "gesture.h"
#include "GYRO_DISCO_SPI.h"

#include "main.h"

static float (*key_sequence)[3] = new float[MATCH_WINDOW_SIZE][3];
static float (*trial_sequence)[3] = new float[MATCH_WINDOW_SIZE][3];

void reset_gesture()
{
    for (int i = 0; i < MATCH_WINDOW_SIZE; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            key_sequence[i][j] = 0.0f;
            trial_sequence[i][j] = 0.0f;
        }
    }
}

bool match_gesture()
{
    float total_diff = 0.0f;
    for (int i = 0; i < MATCH_WINDOW_SIZE; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            float diff = key_sequence[i][j] - trial_sequence[i][j];
            total_diff += diff * diff;
        }
    }
    total_diff /= MATCH_WINDOW_SIZE;
    if (DEBUG_ON)
    {
        printf("========== Total difference: %f ==========\n", total_diff);
    }
    double energy = fabs(total_diff);
    return total_diff < MATCH_THRESHOLD;
}

void record_gesture(GYRO_DISCO_SPI &gyro, bool is_key_sequence)
{
    float(*seq)[3] = is_key_sequence ? key_sequence : trial_sequence;
    GYRO_DISCO_SPI::GyroData data;
    for (int i = 0; i < MATCH_WINDOW_SIZE; i++)
    {
        data = gyro.read();
        for (int j = 0; j < 3; j++)
        {
            seq[i][j] = data.data[j];
        }
    }
    if (DEBUG_ON)
    {
        printf("Recorded gesture: %s\n", is_key_sequence ? "key" : "trial");
        for (int j = 0; j < 3; j++)
        {
            for (int i = 0; i < MATCH_WINDOW_SIZE; i++)
            {
                printf("%8.4f, ", seq[i][j]);
            }
            printf("\n");
        }
        printf("\n");
    }
}
