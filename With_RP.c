#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "rp.h"

#define SAMPLES_PER_BIT 16
#define BITS_TO_COMPARE 25

// Define the lengths for x_in and x_out
#define X_IN_LENGTH 1600
#define X_OUT_LENGTH 2720

float *x_in; // Change to a dynamically allocated array
float x_out[X_OUT_LENGTH]; // Keep x_out as a fixed-size array

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

    // Allocate memory for ADC buffer (x_in)
    x_in = (float *)malloc(X_IN_LENGTH * sizeof(float));
    if (x_in == NULL) {
        fprintf(stderr, "Memory allocation for x_in failed!\n");
        return 1;
    }

    // Generate random data for DAC (x_out)
    srand(time(NULL)); // Seed the random number generator
    for (int i = 0; i < X_OUT_LENGTH; i++) {
        x_out[i] = (float)(rand() % 2); // Generates random binary data (0 or 1)
    }

    // Rest of your code remains the same

    // Release Red Pitaya resources and free memory
    rp_Release();
    free(x_in);

    return 0;
}
