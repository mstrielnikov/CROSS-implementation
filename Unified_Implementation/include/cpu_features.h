#ifndef CPU_FEATURES_H
#define CPU_FEATURES_H

#include <stdbool.h>

typedef struct {
    bool has_avx2;
    bool has_sse41;
    bool has_popcnt;
} cpu_features_t;

static inline cpu_features_t detect_cpu_features(void) {
    cpu_features_t features = {false, false, false};

#if defined(__x86_64__) || defined(_M_X64)
    if (__builtin_cpu_supports("avx2")) {
        features.has_avx2 = true;
    }
    if (__builtin_cpu_supports("sse4.1")) {
        features.has_sse41 = true;
    }
    if (__builtin_cpu_supports("popcnt")) {
        features.has_popcnt = true;
    }
#endif

    return features;
}

#endif /* CPU_FEATURES_H */
