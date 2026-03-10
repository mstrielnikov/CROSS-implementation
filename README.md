# CROSS - Cryptographic Signature Scheme

CROSS is an asymmetric signature scheme based on the Rank Syndrome Decoding (RSD) problem, submitted to the NIST Post-Quantum Cryptography standardization process.

## Original Authors

This implementation is authored by:

- Alessandro Barenghi <alessandro.barenghi@polimi.it>
- Marco Gianvecchio <marco.gianvecchio@mail.polimi.it>
- Patrick Karl <patrick.karl@tum.de>
- Gerardo Pelosi <gerardo.pelosi@polimi.it>
- Jonas Schupp <jonas.schupp@tum.de>

The code is hereby placed in the public domain.

## Project Structure

- `Reference_Implementation/` - Portable C11 reference implementation
- `Optimized_Implementation/` - AVX2 optimized implementation  
- `Unified_Implementation/` - Runtime dispatch between ref/opt
- `KAT/` - Known Answer Test vectors
- `Additional_Implementations/` - Benchmarking and KAT generation tools

## Quick Start

```bash
# Build everything (reference + optimized + test)
make all

# Run tests
./cross_test

# Build with custom parameters
make all CATEGORY=3 VARIANT=RSDPG TARGET=SPEED
```

## Build Options

| Parameter | Options | Default |
|-----------|---------|---------|
| CATEGORY | 1, 3, 5 | 1 |
| VARIANT | RSDP, RSDPG | RSDP |
| TARGET | BALANCED, SPEED, SIG_SIZE | BALANCED |

## Usage

```bash
# Libraries
make ref          # libcross_ref.a
make opt          # libcross_opt.a

# Test binary
make test         # uses reference by default
make test IMPL=opt  # uses optimized

# Custom build
make all CATEGORY=5 VARIANT=RSDPG TARGET=SIG_SIZE

# Clean
make clean
```

## API

Link against `libcross_ref.a` or `libcross_opt.a` and provide:

```c
void randombytes(unsigned char *x, unsigned long long xlen);

int crypto_sign_keypair(unsigned char *pk, unsigned char *sk);
int crypto_sign(unsigned char *sm, unsigned long long *smlen, 
                const unsigned char *m, unsigned long long mlen, 
                const unsigned char *sk);
int crypto_sign_open(unsigned char *m, unsigned long long *mlen, 
                    const unsigned char *sm, unsigned long long smlen, 
                    const unsigned char *pk);
```

## Contributions & Fixes

This repository includes the following enhancements:

- **Build System**: Simplified single Makefile replacing complex CMake setup
- **Documentation**: Updated build instructions for easier project setup
- **Fixed Include Paths**: Resolved header inclusion order issues between implementations
- **Unified Implementation**: Runtime CPU feature detection (AVX2/SSE4.1) with automatic selection between Reference and Optimized implementations
- **Namespacing**: `CROSS_namespace.h` strategy allowing both implementations to coexist

## License

Public domain. See LICENSE file.
