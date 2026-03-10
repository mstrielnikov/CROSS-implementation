#include <stdio.h>
#include <string.h>
#include "cpu_features.h"
#include "CROSS_dispatch.h"
#include "api.h"

int main() {
    printf("Cryptographic Constants:\n");
    printf("  CRYPTO_PUBLICKEYBYTES: %d\n", (int)CRYPTO_PUBLICKEYBYTES);
    printf("  CRYPTO_SECRETKEYBYTES: %d\n", (int)CRYPTO_SECRETKEYBYTES);
    printf("  CRYPTO_BYTES:          %d\n", (int)CRYPTO_BYTES);
    printf("  sizeof(CROSS_sig_t):   %d\n", (int)sizeof(CROSS_sig_t));

    cpu_features_t features = detect_cpu_features();
    printf("\nCPU Features detected:\n");
    printf("  AVX2:  %s\n", features.has_avx2  ? "YES" : "NO");
    printf("  SSE41: %s\n", features.has_sse41 ? "YES" : "NO");
    printf("  POPCNT:%s\n", features.has_popcnt ? "YES" : "NO");

    /* Ensure we have non-zero sizes to avoid zero-size array warnings/errors */
    if (CRYPTO_PUBLICKEYBYTES == 0 || CRYPTO_SECRETKEYBYTES == 0 || CRYPTO_BYTES == 0) {
        printf("\nERROR: Cryptographic constants are zero! Check parameters.h and defines.\n");
        return 1;
    }

    unsigned char pk[CRYPTO_PUBLICKEYBYTES];
    unsigned char sk[CRYPTO_SECRETKEYBYTES];

    printf("\nTesting crypto_sign_keypair (dispatching...)\n");
    int ret = crypto_sign_keypair(pk, sk);
    
    if (ret == 0) {
        printf("Keypair generation: SUCCESS\n");
    } else {
        printf("Keypair generation: FAILURE (%d)\n", ret);
        return 1;
    }

    /* Simple check that we can sign/verify */
    unsigned char message[] = "Hello CROSS!";
    /* sm MUST be large enough for message + signature */
    unsigned char sm[sizeof(message) + CRYPTO_BYTES];
    unsigned long long sm_len;

    printf("Testing crypto_sign...\n");
    ret = crypto_sign(sm, &sm_len, message, sizeof(message), sk);
    if (ret == 0) {
        printf("Signature generation: SUCCESS (smlen: %llu)\n", sm_len);
    } else {
        printf("Signature generation: FAILURE (%d)\n", ret);
        return 1;
    }

    printf("Testing crypto_sign_open...\n");
    unsigned char m_out[sizeof(message)];
    unsigned long long m_len_out;
    ret = crypto_sign_open(m_out, &m_len_out, sm, sm_len, pk);
    if (ret == 0) {
        printf("Signature verification: SUCCESS\n");
        if (m_len_out != sizeof(message) || memcmp(m_out, message, sizeof(message)) != 0) {
            printf("ERROR: Message mismatch after verification!\n");
            return 1;
        }
    } else {
        printf("Signature verification: FAILURE (%d)\n", ret);
        return 1;
    }

    return 0;
}
