#include <stddef.h>
#include <stdint.h>
#include <riscv_vector.h>

/**
 * @file q15_axpy.c
 * @author Akansha Dharmraj Yadav (R3spacex)
 * @brief RISC-V Vector implementation of Q15 AXPY kernel
 * @version 1.0
 * @date 2026-02-08
 */

/**
 * q15_axpy_rvv - Computes y = a + alpha * b with Q15 saturation.
 * @a:     Input vector A (Q15 fixed-point)
 * @b:     Input vector B (Q15 fixed-point)
 * @y:     Output vector Y
 * @n:     Number of elements
 * @alpha: Scalar multiplier
 *
 * Implementation Notes:
 * - Uses RISC-V Vector (RVV) 1.0 intrinsics.
 * - Configured with LMUL=8 (8 vector registers per group) to maximize 
 * compute throughput and minimize strip-mining overhead.
 * - Saturation is handled natively by hardware instructions (vsmul, vsadd).
 */
void q15_axpy_rvv(const int16_t *a, const int16_t *b, int16_t *y, int n, int16_t alpha) {
    size_t vl;

    for (; n > 0; n -= vl) {
        // Request max vector length for 16-bit elements with LMUL=8
        // This makes the code Vector-Length Agnostic (VLA)
        vl = __riscv_vsetvl_e16m8(n);

        // Load input vectors
        vint16m8_t va = __riscv_vle16_v_i16m8(a, vl);
        vint16m8_t vb = __riscv_vle16_v_i16m8(b, vl);

        // 1. Multiply: (alpha * b)
        // vsmul.vx performs Q15 fixed-point multiply (high part + shift) with saturation.
        vint16m8_t v_prod = __riscv_vsmul_vx_i16m8(vb, alpha, vl);

        // 2. Add: a + (alpha * b)
        // vsadd.vv performs saturating addition to prevent overflow wrapping.
        vint16m8_t v_res = __riscv_vsadd_vv_i16m8(va, v_prod, vl);

        // 3. Store result to Output Y
        __riscv_vse16_v_i16m8(y, v_res, vl);

        // Bump pointers
        a += vl;
        b += vl;
        y += vl;
    }
}
