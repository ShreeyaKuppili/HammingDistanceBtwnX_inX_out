#include <stdio.h>
#include <stdlib.h>
#include "rp.h"

#define BUFFER_SIZE (16 * 1024)
#define M 25  // Number of bits to compare
#define N 4  // Samples per bit

float *x_in;
float *x_out;  // Data acquired from the Red Pitaya device
uint32_t buff_size = BUFFER_SIZE;

// Function to find the position with the minimum Hamming distance
int find_min_hamming_position(const float *x_in, const float *x_out, int length) {
    int min_position = -1;
    int num_subsets = buff_size - length + 1;

    for (int i = 0; i < num_subsets; i++) {
        const float *subset = &x_out[i];
        int hamming_distance = 0;

        for (int j = 0; j < length; j++) {
            if (x_in[j] != subset[j]) {
                hamming_distance++;
            }
        }

        if (hamming_distance == 0) {
            min_position = i;
            break; // Exit the loop after finding the first match with Hamming distance 0
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

    // Allocate memory for ADC buffer (x_in)
    x_in = (float *)malloc(buff_size * sizeof(float));

    if (x_in == NULL) {
        fprintf(stderr, "Memory allocation for x_in failed!\n");
        return 1;
    }

    // Generate random data for DAC (x_out)
    srand(time(NULL)); // Seed the random number generator
    for (int i = 0; i < buff_size; i++) {
        x_out[i] = (float)(rand() % 2); // Generates random binary data (0 or 1)
    }

    // Perform data acquisition from Channel 2 (RP_CH_2) where the signal is routed externally
    rp_AcqStart();
    rp_AcqSetTriggerSrc(RP_TRIG_SRC_NOW);
    if (rp_AcqGetOldestDataV(RP_CH_2, &buff_size, x_out) != RP_OK) {
        fprintf(stderr, "Data acquisition failed!\n");
        return 1;
    }
    rp_AcqStop();

    // Find the position with the least Hamming distance
    int position = find_min_hamming_position(x_in, x_out, M * N);

    if (position != -1) {
        printf("Position of the subset with the least Hamming distance: %d\n", position);
    } else {
        printf("No valid position found.\n");
    }

    // Release Red Pitaya resources and free memory
    rp_Release();
    free(x_in);

    return 0;
}
