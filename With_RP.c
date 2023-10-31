#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "rp.h"

#define BUFFER_SIZE (10000)
#define SAMPLES_PER_BIT 16
#define BITS_TO_COMPARE 25

// Define the lengths for x_in and x_out
#define X_IN_LENGTH 4000
#define X_OUT_LENGTH 4000

float x_in[X_IN_LENGTH];
float x_out[X_OUT_LENGTH];
uint32_t buff_size = BUFFER_SIZE;

// Function to calculate Hamming distance between two arrays
int calculate_hamming_distance(const float *x1, const float *x2, int length) {
    int distance = 0;

    for (int i = 0; i < length; i++) {
        if (x1[i] != x2[i]) {
            distance++;
        }
    }

    return distance;
}

int find_min_hamming_position(const float *x_in, const float *x_out, int in_length, int out_length) {
    int min_distance = X_OUT_LENGTH + 1;
    int min_position = -1;

    for (int i = 0; i < X_OUT_LENGTH - BITS_TO_COMPARE; i++) {
        int distance = calculate_hamming_distance(x_in, &x_out[i], BITS_TO_COMPARE);
        if (distance < min_distance) {
            min_distance = distance;
            min_position = i;
        }
        if (min_distance == 0) break;
    }
    return min_position;
}

int main(int argc, char **argv) {
    // Seed the random number generator
    srand(time(NULL));

    // Initialize Red Pitaya resources
    if (rp_Init() != RP_OK) {
        fprintf(stderr, "Red Pitaya initialization failed!\n");
        return 1;
    }

    // Configure ADC
    rp_AcqReset();
    rp_AcqSetDecimation(1);

    // Generate random data for DAC (x_out)
    for (int i = 0; i < buff_size; i++) {
        x_out[i] = (float)rand() / RAND_MAX; // Generates random floating-point values between 0 and 1
    }

    // Perform data acquisition from Channel 2 (RP_CH_2) where the signal is routed externally
    rp_AcqStart();
    rp_AcqSetTriggerSrc(RP_TRIG_SRC_NOW);
    if (rp_AcqGetOldestDataV(RP_CH_2, &buff_size, x_out) != RP_OK) {
        fprintf(stderr, "Data acquisition failed!\n");
        return 1;
    }
    rp_AcqStop();

    int min_position = find_min_hamming_position(x_in, x_out, BITS_TO_COMPARE * SAMPLES_PER_BIT, X_OUT_LENGTH);
    int min_distance = calculate_hamming_distance(x_in, &x_out[min_position], BITS_TO_COMPARE * SAMPLES_PER_BIT);

    printf("Minimum Hamming Distance: %d\n", min_distance);
    printf("Position with Minimum Distance: %d\n", min_position);

    // Release Red Pitaya resources (no need to free x_in as it's not dynamically allocated)
    rp_Release();

    return 0;
}
