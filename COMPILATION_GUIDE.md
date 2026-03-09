# CROSS Compilation and Linking Guide

This document provides instructions for compiling the CROSS implementation and linking it into your projects.

## Prerequisites

- **C11 Compiler**: GCC (9.4.0+ recommended) or Clang.
- **Build System**: CMake (3.7+) for existing benchmarks and tests.
- **Dependencies**:
  - `libkeccak` (optional, fallback implementation provided).
  - Standard C libraries (`math`, `ssl`, `crypto`).

## 1. Building Benchmarks and KAT Generation

The project includes pre-configured CMake setups for performance benchmarking and Known Answer Test (KAT) generation.

### Benchmarking

To build the benchmarking binaries:

1. `cd Additional_Implementations/Benchmarking`
2. `mkdir build && cd build`
3. `cmake ../ -DREFERENCE=1` (use `-DREFERENCE=0` for AVX2 optimized version)
4. `make -j$(nproc)`

The binaries will be generated in the `bin/` directory.

### KAT Generation

To build the KAT generation binaries:

1. `cd Additional_Implementations/KAT_Generation`
2. `mkdir build && cd build`
3. `cmake ../ -DREFERENCE=1`
4. `make -j$(nproc)`

## 2. Manual Compilation (Reference Implementation)

If you wish to compile the library manually or integrate it into an existing build system without CMake, follow these steps.

### Required Source Files

For a basic reference implementation, you need:

- `Reference_Implementation/lib/CROSS.c`
- `Reference_Implementation/lib/sign.c`
- `Reference_Implementation/lib/merkle.c`
- `Reference_Implementation/lib/seedtree.c`
- `Reference_Implementation/lib/csprng_hash.c`
- `Reference_Implementation/lib/pack_unpack.c`
- `Reference_Implementation/lib/keccakf1600.c`
- `Reference_Implementation/lib/fips202.c`

### Include Paths

Headers are located in `Reference_Implementation/include/`.

### Preprocessor Definitions

You **must** define the security category and variant at compile time. Example for Category 1, RSDP balanced:
`-DCATEGORY_1=1 -DBALANCED=1 -DRSDP=1`

### Example Compilation Command

```bash
gcc -O3 -IReference_Implementation/include \
    -DCATEGORY_1=1 -DBALANCED=1 -DRSDP=1 \
    Reference_Implementation/lib/*.c -o cross_reference
```

## 3. Linking as a Library

To link CROSS into your own C project, you should:

1. Include the API header: `#include "api.h"`
2. Link against the compiled objects or static library.
3. **Provide a `randombytes` implementation**. The library expects a function with the following signature:
   ```c
   void randombytes(unsigned char *x, unsigned long long xlen);
   ```

### Core API Functions

```c
int crypto_sign_keypair(unsigned char *pk, unsigned char *sk);
int crypto_sign(unsigned char *sm, unsigned long long *smlen, const unsigned char *m, unsigned long long mlen, const unsigned char *sk);
int crypto_sign_open(unsigned char *m, unsigned long long *mlen, const unsigned char *sm, unsigned long long smlen, const unsigned char *pk);
```

For a concrete example, see [Makefile.example](file:///home/max/projects/CROSS-implementation/Makefile.example).
