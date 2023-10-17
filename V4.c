#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <time.h> // Added for random number generation
#include "rp.h"

#define BUFFER_SIZE (16 * 1024)
#define SAMPLES_PER_BIT 16
#define BITS_TO_COMPARE 25

float *x_in;
float *x_out;
uint32_t buff_size = BUFFER_SIZE;
float min_hamming_distance = -1.0;
int min_hamming_position = -1;

// Function to calculate Hamming distance between two arrays
float calculate_hamming_distance(const float *x1, const float *x2, int length) {
    float distance = 0.0;

    for (int i = 0; i < length; i++) {
        distance += fabs(x1[i] - x2[i]);
    }

    return distance;
}

// Function to find the position with the minimum Hamming distance
int find_min_hamming_position(const float *x_in, const float *x_out) {
    int min_distance = 25 * SAMPLES_PER_BIT;
    int min_position = -1;

    int length = BITS_TO_COMPARE * SAMPLES_PER_BIT;
    int num_subsets = buff_size - length + 1;

    for (int i = 0; i < num_subsets; i++) {
        const float *subset = &x_out[i];
        float distance = calculate_hamming_distance(x_in, subset, length);

        if (distance < min_distance) {
            min_distance = distance;
            min_position = i;
        }
        if (min_distance == 0) {
            break; // If minimum distance is zero, we found the first match
        }
    }

    return min_position;
}

int main(int argc, char **argv) {
    // Initialize Red Pitaya resources
    if (rp_Init() != RP_OK) {
        fprintf(stderr, "Red Pitaya initialization failed!\n");
        return 1;
    }

    // Configure ADC
    rp_AcqReset();
    rp_AcqSetDecimation(1);

    // Generate random data for DAC (x_out)
    srand(time(NULL)); // Seed the random number generator
    x_out = (float *)malloc(buff_size * sizeof(float));
    for (int i = 0; i < buff_size; i++) {
        x_out[i] = (float)(rand() % 2); // Generates random binary data (0 or 1)
    }

    // Allocate memory for ADC buffer
    x_in = (float *)malloc(buff_size * sizeof(float));

    if (x_in == NULL || x_out == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        return 1;
    }

    // Perform data acquisition
    rp_AcqStart();
    rp_AcqSetTriggerSrc(RP_TRIG_SRC_NOW);
    rp_AcqGetOldestDataV(RP_CH_1, &buff_size, x_in);
    rp_AcqStop();

    int position = find_min_hamming_position(x_in, x_out);

    if (position != -1) {
        printf("Position of the subset with the least Hamming distance: %d\n", position);
    } else {
        printf("No valid position found.\n");
    }

    // Release Red Pitaya resources and free memory
    rp_Release();
    free(x_in);
    free(x_out);

    return 0;
}
