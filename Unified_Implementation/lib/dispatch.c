#include <string.h>
#include "cpu_features.h"
#include "CROSS.h"
#include "api.h"

/* Declare the namespaced functions for Reference and Optimized implementations */
/* Note: These will be available because we compile with -DCROSS_NAMESPACE=ref_ and -DCROSS_NAMESPACE=opt_ */

#define REF_NAMESPACE ref_
#define OPT_NAMESPACE opt_

#define CONCAT_INNER(prefix, func) prefix ## func
#define CONCAT(prefix, func) CONCAT_INNER(prefix, func)

/* Prototypes for namespaced functions */
void CONCAT(REF_NAMESPACE, CROSS_keygen)(sk_t *SK, pk_t *PK);
void CONCAT(REF_NAMESPACE, CROSS_sign)(const sk_t *SK, const char *m, uint64_t mlen, CROSS_sig_t *sig);
int CONCAT(REF_NAMESPACE, CROSS_verify)(const pk_t *PK, const char *m, uint64_t mlen, const CROSS_sig_t *sig);

void CONCAT(OPT_NAMESPACE, CROSS_keygen)(sk_t *SK, pk_t *PK);
void CONCAT(OPT_NAMESPACE, CROSS_sign)(const sk_t *SK, const char *m, uint64_t mlen, CROSS_sig_t *sig);
int CONCAT(OPT_NAMESPACE, CROSS_verify)(const pk_t *PK, const char *m, uint64_t mlen, const CROSS_sig_t *sig);

/* Dispatcher state */
typedef enum {
    IMPL_UNINITIALIZED,
    IMPL_REFERENCE,
    IMPL_OPTIMIZED
} impl_t;

static impl_t active_impl = IMPL_UNINITIALIZED;

static void initialize_dispatch(void) {
    if (active_impl != IMPL_UNINITIALIZED) return;
    
    cpu_features_t features = detect_cpu_features();
    if (features.has_avx2) {
        active_impl = IMPL_OPTIMIZED;
    } else {
        active_impl = IMPL_REFERENCE;
    }
}

/* NIST API Implementation */

int crypto_sign_keypair(unsigned char *pk, unsigned char *sk) {
    initialize_dispatch();
    if (active_impl == IMPL_OPTIMIZED) {
        CONCAT(OPT_NAMESPACE, CROSS_keygen)((sk_t *)sk, (pk_t *)pk);
    } else {
        CONCAT(REF_NAMESPACE, CROSS_keygen)((sk_t *)sk, (pk_t *)pk);
    }
    return 0;
}

int crypto_sign(unsigned char *sm, unsigned long long *smlen,
                const unsigned char *m, unsigned long long mlen,
                const unsigned char *sk) {
    initialize_dispatch();
    memcpy(sm, m, mlen);
    if (active_impl == IMPL_OPTIMIZED) {
        CONCAT(OPT_NAMESPACE, CROSS_sign)((const sk_t *)sk, (const char *)m, mlen, (CROSS_sig_t *)(sm + mlen));
    } else {
        CONCAT(REF_NAMESPACE, CROSS_sign)((const sk_t *)sk, (const char *)m, mlen, (CROSS_sig_t *)(sm + mlen));
    }
    *smlen = mlen + sizeof(CROSS_sig_t);
    return 0;
}

int crypto_sign_open(unsigned char *m, unsigned long long *mlen,
                     const unsigned char *sm, unsigned long long smlen,
                     const unsigned char *pk) {
    initialize_dispatch();
    *mlen = smlen - sizeof(CROSS_sig_t);
    memcpy(m, sm, *mlen);
    int ok;
    if (active_impl == IMPL_OPTIMIZED) {
        ok = CONCAT(OPT_NAMESPACE, CROSS_verify)((const pk_t *)pk, (const char *)m, *mlen, (const CROSS_sig_t *)(sm + *mlen));
    } else {
        ok = CONCAT(REF_NAMESPACE, CROSS_verify)((const pk_t *)pk, (const char *)m, *mlen, (const CROSS_sig_t *)(sm + *mlen));
    }
    return ok - 1;
}

/* Helper to check which implementation is active */
const char* get_active_implementation(void) {
    initialize_dispatch();
    return (active_impl == IMPL_OPTIMIZED) ? "Optimized (AVX2)" : "Reference";
}
