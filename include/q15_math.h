#ifndef Q15_MATH_H
#define Q15_MATH_H

#include <stdint.h>

/**
 * @brief Computes y = alpha * x + y (AXPY) in Q15 fixed-point format.
 * * @param x      Pointer to input vector x
 * @param y      Pointer to input/output vector y
 * @param n      Number of samples to process
 * @param alpha  Scalar gain factor
 */
void dsp_axpy_q15(const int16_t *x, int16_t *y, uint32_t n, int16_t alpha);

#endif // Q15_MATH_H
