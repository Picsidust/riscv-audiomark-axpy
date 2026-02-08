# RISC-V Vector Optimized Q15 AXPY

This repository contains an optimized implementation of the **Q15 AXPY** kernel (`y = sat_q15(a + alpha * b)`), developed for the RISC-V Audiomark Coding Challenge.

The solution is implemented using **RISC-V Vector (RVV) 1.0 Intrinsics** and is verified to be bit-exact against a scalar reference model.

## Project Structure
```text
.
├── src/
│   └── q15_axpy.c       # Optimized Vector Implementation
├── include/
│   └── q15_math.h       # Header definitions
├── tests/
│   └── test_q15_axpy.c  # Verification Test Bench
└── Makefile             # Build System (GCC + QEMU)
```
## Design Choices & Reasoning

### 1. Vector Configuration (LMUL=8)
I utilized the `__riscv_vsetvl_e16m8` intrinsic.

* **Reasoning:** `LMUL=8` groups 8 vector registers per operand. This maximizes the effective vector length (VL), allowing the processor to handle more data elements per instruction. This reduces the frequency of the strip-mining loop overhead (pointer bumps, branch prediction) compared to using `LMUL=1`.

### 2. Saturating Arithmetic
Audio processing requires strict handling of overflows to prevent distortion.

* **Reasoning:** Instead of using manual comparison branches (which are slow), I used hardware-native instructions:
    * `vsmul.vx`: Performs the Q15 multiply (multiply high + shift) with saturation.
    * `vsadd.vv`: Performs the addition with saturation clamping to `[INT16_MIN, INT16_MAX]`.

### 3. Vector-Length Agnostic (VLA)
The solution uses the standard RISC-V strip-mining loop (`n -= vl`).

* **Reasoning:** This ensures the code is portable across any RISC-V implementation, whether it is a small embedded core (VLEN=128) or a high-performance core (VLEN=512+), satisfying the requirement to be buildable on RV32 or RV64.

## Performance Analysis (Back-of-the-Envelope)

The coding challenge requests an estimation of the expected speed-up.

**Scenario:** Processing `N` elements on a core with `VLEN=128` bits.

* **Scalar Code:** Requires a loop with ~7 instructions per element (Load A, Load B, Mul, Shift, Add, Store, Branch).
    * *Total Operations:* `7 * N`

* **Vector Code (LMUL=8):**
    * With `VLEN=128` and `LMUL=8`, we process `8 * (128 / 16) = 64` elements per instruction.
    * The loop body has ~5 vector instructions (Load A, Load B, Mul, Add, Store).
    * *Total Operations:* `5 * (N / 64)`

**Theoretical Speedup:**
Ignoring memory bandwidth bottlenecks, the reduction in instruction count is approximately:

`Speedup ≈ (7 * N) / (5 * (N / 64)) ≈ 89x`

*Note: In practice, performance will likely be memory-bound, but the reduction in pipeline control overhead is significant.*

## Build & Verification

The project is structured to be compiled with the **RISC-V GNU Toolchain** (GCC).

**To compile and run the verification harness (via QEMU):**
```bash
make
make run
```

**Expected Output:**

```text
Running Q15 AXPY Verification on 1024 samples...
[PASS] Vector implementation matches reference model.
```

## Author

**Akansha Dharmraj Yadav**

* **LFX Username:** R3spacex
* **Focus:** Embedded Systems, RISC-V Architecture, & DSP