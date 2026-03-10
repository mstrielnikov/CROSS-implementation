# CROSS Compilation Guide

## Original Authors

This implementation is authored by Alessandro Barenghi, Marco Gianvecchio, Patrick Karl, Gerardo Pelosi, and Jonas Schupp. The code is placed in the public domain.

## Build System

This project uses a simple **Makefile** as the single build tool.

## Quick Build

```bash
# Build reference + optimized + test
make all

# Run functional tests
./cross_test
```

## Build Targets

| Target | Description |
|--------|-------------|
| `make all` | Build ref + opt libraries + test binary |
| `make ref` | Build reference library (libcross_ref.a) |
| `make opt` | Build optimized library (libcross_opt.a) |
| `make test` | Build and run test binary |
| `make clean` | Remove build artifacts |

## Parameters

```bash
make all CATEGORY=1|3|5 VARIANT=RSDP|RSDPG TARGET=BALANCED|SPEED|SIG_SIZE
```

| Parameter | Options | Default |
|-----------|---------|---------|
| CATEGORY | 1, 3, 5 | 1 |
| VARIANT | RSDP, RSDPG | RSDP |
| TARGET | BALANCED, SPEED, SIG_SIZE | BALANCED |
| IMPL | ref, opt | ref |

## Examples

```bash
# Default build
make all

# Category 3, RSDPG variant, SPEED target
make all CATEGORY=3 VARIANT=RSDPG TARGET=SPEED

# Run test with optimized implementation
make test IMPL=opt
```

## Manual Compilation

If you need custom compilation:

```bash
# Reference
gcc -O3 -Wall -Wextra -std=c11 \
    -IReference_Implementation/include -IUnified_Implementation/include \
    -DCATEGORY_1=1 -DBALANCED=1 -DRSDP=1 \
    Reference_Implementation/lib/*.c -o cross -lm

# Optimized (note: Optimized include path first!)
gcc -O3 -Wall -Wextra -std=c11 \
    -IOptimized_Implementation/include -IReference_Implementation/include -IUnified_Implementation/include \
    -DCATEGORY_1=1 -DBALANCED=1 -DRSDP=1 \
    Optimized_Implementation/lib/CROSS.c Optimized_Implementation/lib/merkle.c \
    Optimized_Implementation/lib/seedtree.c Optimized_Implementation/lib/fips202x4.c \
    Optimized_Implementation/lib/KeccakP-1600-times4-SIMD256.c \
    Reference_Implementation/lib/csprng_hash.c Reference_Implementation/lib/pack_unpack.c \
    Reference_Implementation/lib/keccakf1600.c Reference_Implementation/lib/fips202.c \
    Reference_Implementation/lib/sign.c -o cross -lm
```

## Linking

Provide a `randombytes` function and link with `-lm`:

```c
void randombytes(unsigned char *x, unsigned long long xlen);
```

## Troubleshooting

- **Missing headers:** Ensure `-IUnified_Implementation/include` is in path
- **Missing SIMD functions:** Put Optimized path BEFORE Reference in include path
- **Link errors:** Provide `randombytes` implementation and link `-lm`
