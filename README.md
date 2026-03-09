# Boomerang Attack on ARX Ciphers

A C++ research toolkit for performing **Boomerang** and **Retracing Boomerang** attacks on ARX (Add-Rotate-XOR) stream ciphers. This project implements and experimentally evaluates differential cryptanalysis techniques against reduced-round variants of **ChaCha**, **ChaCha\***, **Salsa**, and **Forro**.

---

## Table of Contents

- [Background](#background)
- [Ciphers Implemented](#ciphers-implemented)
- [Project Structure](#project-structure)
- [Attack Framework](#attack-framework)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Results](#results)
- [References](#references)

---

## Background

**ARX ciphers** build their security entirely from three cheap, hardware-friendly operations:
- **Add** — modular 32-bit addition
- **Rotate** — left/right circular bit rotation
- **XOR** — bitwise exclusive-or

This project studies differential attacks on these ciphers, specifically:

- **Boomerang Attack**: A chosen-plaintext / chosen-ciphertext attack that decomposes the cipher into two sub-ciphers *E₀* and *E₁* and exploits independent differential trails in each half through a "boomerang" query structure. The attack probability is approximately *p₁² · p₂ · q₁ · q₂*, where *p* and *q* are differential probabilities for *E₀* and *E₁* respectively.

- **Retracing Boomerang Attack**: A refinement that exploits additional structure in the cipher to improve the overall attack probability, using a four-tuple query with correlated differentials (α, β, γ, δ, μ).

Both attacks are implemented as experimental (statistical) verifiers over large plaintext batches (2²⁰ samples by default), measuring empirical differential probabilities.

---

## Ciphers Implemented

| Cipher       | File             | QR Style | Rotation Constants          | Constants Position |
|:-------------|:-----------------|:---------|:----------------------------|:-------------------|
| **ChaCha20** | `chacha.hpp`     | 4-word   | 16, 12, 8, 7                | Words 0–3          |
| **ChaCha\*** | `chachaStar.hpp` | 4-word   | 19, 17, 25, 11              | Words 0–3          |
| **Salsa20**  | `salsa.hpp`      | 4-word   | 7, 9, 13, 18                | Words 0, 5, 10, 15 |
| **Forro**    | `forro.hpp`      | 5-word   | 10, 27, 8                   | Words 6, 7, 14, 15 |

All ciphers operate on a **4×4 matrix of 32-bit words** (512-bit block). Full forward encryption and inverse decryption are implemented for each, supporting arbitrary round counts including half-round and quarter-round granularity.

---

## Project Structure

```
arx_ciphers/
│
├── arx_utils_efficient.hpp     # Core primitives: word/block structs,
│                               # modular add/sub, rotations, XOR, RNG,
│                               # block print utilities, complexity calculator
│
├── chacha.hpp                  # ChaCha20 cipher implementation
├── chachaStar.hpp              # ChaCha* (modified rotation constants)
├── salsa.hpp                   # Salsa20 cipher implementation
├── forro.hpp                   # Forro cipher implementation
│
├── boomerangAttack.cpp         # Main boomerang attack driver
│                               # (configure E0/E1 rounds and ID/OD here)
│
├── retracing_boomerang_attack.cpp  # Retracing boomerang attack on ChaCha
│
├── differentialTrail/          # Differential analysis experiments
│   ├── arx_utils_efficient.hpp
│   ├── chacha.hpp
│   └── differentialAnalysis.cpp
│
├── chachaStar/                 # ChaCha* specific experiments
│   ├── arx_utils_efficient.hpp
│   ├── chachaStar.hpp
│   └── backwardBias6.cpp
│
├── misc/                       # Standalone utility experiments
│   ├── backwardBias6.cpp       # Backward bias (6-round)
│   ├── backwardBias7.cpp       # Backward bias (7-round)
│   ├── differentialAnalysis.cpp
│   ├── findIDOD.cpp            # Input/Output differential search
│   ├── probabilistic_neutral_bits.cpp  # PNB analysis
│   └── test_ciphers.cpp
│
├── archive/                    # Archived experiments and older versions
│
└── results/                    # Experimental output logs
    ├── forroBA.txt             # Forro boomerang attack results
    ├── forroBA3.txt            # Forro 3-round boomerang results
    ├── forroBA5.25.txt         # Forro 5.25-round boomerang results
    ├── forroBA5.5.txt          # Forro 5.5-round boomerang results
    ├── forroIDOD.txt           # Forro ID/OD analysis
    ├── forro4.txt              # Forro 4-round analysis
    ├── salsaBA.txt             # Salsa boomerang results
    ├── PNB_TOY_CHACHA.txt      # PNB results for toy ChaCha
    ├── pnbFor6Round.txt        # PNB results for Forro (6 rounds)
    ├── pnbFor7Rounds.txt       # PNB results for Forro (7 rounds)
    └── ...
```

---

## Attack Framework

### Boomerang Attack (`boomerangAttack.cpp`)

Configure the attack by editing the two enums at the top of the file:

```cpp
// Decomposition: E = E1 ∘ E0
enum boomerangParameters {
    e0Rounds     = 2,     // Number of full rounds in E0
    e0HalfRound  = false, // Append a half-round to E0?
    e0QuarterRound = false,
    e1Rounds     = 3,     // Number of full rounds in E1
    e1HalfRound  = true,  // Append a half-round to E1?
    e1QuarterRound = false
};

// Plaintext input differential (ID) and ciphertext output differential (OD)
enum IDOD {
    idWordIndex = 12,  // Word index of the input differential bit
    idBitIndex  = 0,   // Bit position of the input differential
    odWordIndex = 15,  // Word index of the output differential bit
    odBitIndex  = 16   // Bit position of the output differential
};
```

**To switch between ciphers**, change the `#include` on line 1:
```cpp
#include "forro.hpp"    // Forro (default)
// #include "chacha.hpp"
// #include "salsa.hpp"
// #include "chachaStar.hpp"
```

The attack outputs the individual differential probabilities **p₁, p₂, q₁, q₂**, their product **p² · q²**, the corresponding **bias**, and the **data complexity** expressed as **2^x**.

### Retracing Boomerang Attack (`retracing_boomerang_attack.cpp`)

Uses the same round-decomposition pattern with additional differential indices (α, β, γ, δ_L, δ_R, μ_L, μ_R) for the more refined query structure. Computes **p · q₁ · q₂ᴿ** squared times **q₂ᴸ** as the distinguishing probability.

---

## Getting Started

### Prerequisites

- A C++17-compatible compiler (e.g., `g++`, `clang++`)
- Standard library only — no external dependencies

### Compile & Run

**Boomerang Attack (Forro, default configuration):**
```bash
g++ -O2 -o boomerang boomerangAttack.cpp
./boomerang
```

**Retracing Boomerang Attack (ChaCha):**
```bash
g++ -O2 -o retracing retracing_boomerang_attack.cpp
./retracing
```

**Differential Trail Analysis:**
```bash
g++ -O2 -o diffAnalysis differentialTrail/differentialAnalysis.cpp
./diffAnalysis
```

> **Note**: The default plaintext batch size is `ptSize = 1 << 20` (≈ 1 million samples). Larger values produce more accurate probability estimates but increase runtime. Adjust the `#define ptSize` macro to trade off speed vs. accuracy.

---

## Usage

### Scanning All Input/Output Differentials

Use the `findAttackOnGivenID` function in `boomerangAttack.cpp` to exhaustively search all 16×32 output differential positions for a fixed input differential:

```cpp
int main() {
    findAttackOnGivenID(12, 0);  // Scan all ODs for ID = (word 12, bit 0)
    return 0;
}
```

### Scanning Multiple Input Differentials

Use `attackOnMultiID` to evaluate a list of (wordIndex, bitIndex) pairs against a fixed output differential:

```cpp
int main() {
    std::vector<pair<uint32_t,uint32_t>> ids = {{12,0},{13,0},{14,0}};
    attackOnMultiID(ids, 15, 16);
    return 0;
}
```

### Single Attack Evaluation

Use `attack()` to run the attack with the constants defined in the `IDOD` enum.

---

## Results

Experimental result logs are stored in the `results/` directory. Key findings include:

| Target  | Attack Type      | Rounds | Data Complexity |
|:--------|:-----------------|:-------|:----------------|
| Forro   | Boomerang        | 5.5    | See `forroBA5.5.txt` |
| Forro   | Boomerang        | 5.25   | See `forroBA5.25.txt` |
| Forro   | Boomerang        | 3      | See `forroBA3.txt` |
| Salsa20 | Boomerang        | —      | See `salsaBA.txt` |
| ChaCha  | PNB (Toy)        | —      | See `PNB_TOY_CHACHA.txt` |
| Forro   | PNB              | 6, 7   | See `pnbFor6Round.txt`, `pnbFor7Rounds.txt` |

---

## References

- Bernstein, D. J. (2008). *ChaCha, a variant of Salsa20*. SASC 2008.
- Bernstein, D. J. (2005). *The Salsa20 family of stream ciphers*. New Stream Cipher Designs.
- Cardoso dos Santos, L. et al. (2022). *Forro: An ARX Cipher with Higher Diffusion*. IACR ePrint.
- Biham, E., Dunkelman, O., & Keller, N. (2005). *Related-Key Boomerang and Rectangle Attacks*. EUROCRYPT 2005.
- Coutinho, M., & Neto, T. C. S. (2021). *Improved Linear Approximations to ARX Ciphers and Attacks on ChaCha*. EUROCRYPT 2021.
