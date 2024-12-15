//
//  main.c
//  Task4
//
//  Created by 宋政泽 on 12/12/24.
//

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define SAMPLING_RATE 50         // 50 Hz sampling rate
#define SIMULATION_TIME 300      // Simulation time of 300 seconds
#define PI 3.14159

// Constants for dynamic thresholding
#define WINDOW_SIZE 10           // Time window size (e.g., 0.2s to 0.4s)
#define SENSITIVITY 0.5          // Sensitivity

// Simulate generating acceleration data
void generate_acceleration_data(float *x, float *y, float *z, int samples) {
    for (int i = 0; i < samples; i++) {
        float t = (float)i / SAMPLING_RATE;
        x[i] = 0.5 * sin(2 * PI * 1.5 * t);
        y[i] = 0.2 * sin(2 * PI * 1.2 * t + 1);
        z[i] = 1.0 + 0.6 * sin(2 * PI * 1.8 * t);
    }
}

// Update dynamic threshold
float update_dynamic_threshold(float max, float min, float *buffer, int size) {
    float avg = (max + min) / 2.0;
    for (int i = 1; i < size; i++) {
        buffer[i - 1] = buffer[i];
    }
    buffer[size - 1] = avg;
    float sum = 0;
    for (int i = 0; i < size; i++) {
        sum += buffer[i];
    }
    return sum / size;
}

// Step detection function
int detect_steps(float *x, float *y, float *z, int samples) {
    int total_steps = 0;                     // Total steps
    int possible_steps = 0;                  // Current possible steps count
    int regulation_mode = 0;                 // Whether in step stabilization mode
    float dynamic_threshold_buffer[4] = {0}; // Dynamic threshold buffer
    float max_peak = -1e9, min_peak = 1e9;   // Initialize max and min peaks
    int window_counter = 0;                  // Window counter

    for (int i = 1; i < samples - 1; i++) {
        // Calculate the total absolute value
        float sum = fabs(x[i]) + fabs(y[i]) + fabs(z[i]);

        // Update max and min peaks
        if (sum > max_peak) max_peak = sum;
        if (sum < min_peak) min_peak = sum;

        window_counter++;

        // Detect peaks when reaching the center of the window
        if (window_counter >= WINDOW_SIZE) {
            // Check if the step condition is met
            if ((max_peak - min_peak) > SENSITIVITY) {
                float dynamic_threshold = update_dynamic_threshold(max_peak, min_peak, dynamic_threshold_buffer, 4);
                if (max_peak > (dynamic_threshold + SENSITIVITY / 2) &&
                    min_peak < (dynamic_threshold - SENSITIVITY / 2)) {
                    possible_steps++;
                }
            }

            // Check if step stabilization mode should be entered
            if (possible_steps >= 8) {
                regulation_mode = 1; // Enter stabilization mode
                total_steps += 8;    // Record the first 8 steps
                possible_steps = 0;  // Reset possible steps count
            }

            if (regulation_mode) {
                total_steps++; // In stabilization mode, increment each step
            }

            // Reset peaks and window counter
            max_peak = -1e9;
            min_peak = 1e9;
            window_counter = 0;
        }
    }

    return total_steps;
}

int main() {
    int total_samples = SAMPLING_RATE * SIMULATION_TIME;
    float x[total_samples], y[total_samples], z[total_samples];

    // Generate simulated acceleration data
    generate_acceleration_data(x, y, z, total_samples);

    // Detect steps
    int step_count = detect_steps(x, y, z, total_samples);

    // Print the detection result
    printf("Detected Steps: %d\n", step_count);

    // Save data to file
    FILE *file = fopen("steps.txt", "w");
    if (file) {
        fprintf(file, "Steps Detected: %d\n", step_count);
        fclose(file);
        printf("Data saved to steps.txt\n");
    } else {
        printf("Error saving data\n");
    }

    return 0;
}
