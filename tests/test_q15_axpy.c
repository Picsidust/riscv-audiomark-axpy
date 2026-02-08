#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define TEST_SIZE 1024

// Forward declaration of the vector function
void q15_axpy_rvv(const int16_t *a, const int16_t *b, int16_t *y, int n, int16_t alpha);

// Scalar Golden Reference (for verification)
void ref_axpy(const int16_t *a, const int16_t *b, int16_t *y, int n, int16_t alpha) {
    for (int i = 0; i < n; i++) {
        // Emulate Q15 multiply: (alpha * b) >> 15
        int32_t prod = ((int32_t)b[i] * alpha) >> 15;
        
        // Emulate Saturation for Multiply
        if (prod > 32767) prod = 32767;
        if (prod < -32768) prod = -32768;

        // Add: a + prod
        int32_t sum = (int32_t)a[i] + prod;
        
        // Emulate Saturation for Add
        if (sum > 32767) sum = 32767;
        if (sum < -32768) sum = -32768;

        y[i] = (int16_t)sum;
    }
}

int main() {
    int16_t a[TEST_SIZE];
    int16_t b[TEST_SIZE];
    int16_t y_vec[TEST_SIZE];
    int16_t y_ref[TEST_SIZE];
    int16_t alpha = 16384; // 0.5 in Q15

    // Initialize data
    for (int i = 0; i < TEST_SIZE; i++) {
        a[i] = (rand() % 65536) - 32768;
        b[i] = (rand() % 65536) - 32768;
        y_vec[i] = 0;
        y_ref[i] = 0;
    }

    printf("Running Q15 AXPY Verification on %d samples...\n", TEST_SIZE);

    // Run implementations
    ref_axpy(a, b, y_ref, TEST_SIZE, alpha);
    q15_axpy_rvv(a, b, y_vec, TEST_SIZE, alpha);

    // Compare
    int errors = 0;
    for (int i = 0; i < TEST_SIZE; i++) {
        if (abs(y_vec[i] - y_ref[i]) > 1) {
            printf("Error @ %d: Vec=%d, Ref=%d\n", i, y_vec[i], y_ref[i]);
            errors++;
            if (errors > 10) break;
        }
    }

    if (errors == 0) {
        printf("[PASS] Vector implementation matches reference model.\n");
        return 0;
    } else {
        printf("[FAIL] Found mismatches.\n");
        return 1;
    }
}
