# Steering Vector Precomputation Tool

This tool precomputes steering vectors for a **uniform planar square** array (2×2 microphones, row-major indexing), generating data for 256 uniformly distributed angles and 32 frequencies in the range 687.5 Hz to 1687.5 Hz.

## Parameters

The parameters are defined in `src/MusicConstants.hpp` and shared with the main application:

- **Microphones (M)**: 4
- **Angles**: 256 (uniformly distributed from 0 to 2π)
- **Frequencies**: 32 (687.5 Hz to 1687.5 Hz)
- **Array spacing**: 5 cm
- **Speed of sound**: 343 m/s

## Generated Files

The tool generates a single C++ header file `data/steering_vectors/steering_vectors.hpp` containing:

- System constants: sampling frequency (8000 Hz), FFT size (256)
- Algorithm constants: number of microphones, angles, frequencies
- An `std::array` of 32 Eigen matrices, one for each frequency

## Usage in Code

Include the header file and access steering vectors by frequency index:

```cpp
#include "data/steering_vectors/steering_vectors.hpp"

// Access steering vectors for frequency index i (0-31)
auto steering_vector = steering_vectors[i].col(angle_index);

// Constants are also available
static_assert(SAMPLING_FREQ == 8000.0f);
static_assert(FFT_SIZE == 256);
static_assert(N_FREQUENCIES == 32);
```

## Usage

Run the generation script from the precompute_steering_vectors directory:

```bash
./generate_steering_vectors.sh
```

This will:
1. Build the precomputation tool
2. Run it to generate steering vectors
3. Move the files to `../../data/steering_vectors/`
