#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SAMPLES_PER_BIT 16
#define BITS_TO_COMPARE 25

// Define the lengths for x_in and x_out
#define K 20

#define X_IN_LENGTH 1600
#define X_OUT_LENGTH 2720

bool x_in[X_IN_LENGTH];
bool x_out[X_OUT_LENGTH];

// Function to calculate Hamming distance between two arrays
int calculate_hamming_distance(const bool *x1, const bool *x2, int length) {
    int distance = 0;

    for (int i = 0; i < length; i++) {
        if (x1[i] != x2[i]) {
            distance++;
        }
    }

    return distance;
}

int find_min_hamming_position(const bool *x_in, const bool *x_out, int in_length, int out_length) {
    int min_distance = X_OUT_LENGTH + 1;
    int min_position = -1;

    for(int i = 0; i < X_OUT_LENGTH - BITS_TO_COMPARE; i++)
    {
        int distance = calculate_hamming_distance(x_in, &x_out[i], BITS_TO_COMPARE);
        if (distance < min_distance) {
            min_distance = distance;
            min_position = i;
        }
        if(min_distance == 0) break;
    }
    return min_position;
}

int main() {
    // Seed the random number generator
    srand(time(NULL));

    // Generate random binary data for x_out
    for (int i = 0; i < X_OUT_LENGTH; i+= SAMPLES_PER_BIT) {
        x_out[i] = rand() & 1;
        for(int j = 1; j < SAMPLES_PER_BIT; j++){
            x_out[i+j] = x_out[i];
        }
    }

    // Generate x_in by taking 25 random digits from x_out
    for (int i = 0; i < X_IN_LENGTH; i++) {
        x_in[i] = x_out[i + K];
    }

    int min_position = find_min_hamming_position(x_in, x_out, BITS_TO_COMPARE * SAMPLES_PER_BIT, X_OUT_LENGTH);
    int min_distance = calculate_hamming_distance(x_in, &x_out[min_position], BITS_TO_COMPARE * SAMPLES_PER_BIT);

    printf("Minimum Hamming Distance: %d\n", min_distance);
    printf("Position with Minimum Distance: %d\n", min_position);

    return 0;
}
