#include <stdio.h>
#include "CROSS_dispatch.h"

/* Declare the diagnostic helper from dispatch.c */
const char* get_active_implementation(void);

int main() {
    printf("--- CROSS Runtime Dispatch Diagnostic ---\n");
    
    const char* impl = get_active_implementation();
    printf("Detected Active Implementation: %s\n", impl);
    
    unsigned char pk[CRYPTO_PUBLICKEYBYTES];
    unsigned char sk[CRYPTO_SECRETKEYBYTES];
    
    printf("Testing keypair generation...\n");
    if (crypto_sign_keypair(pk, sk) == 0) {
        printf("Success: Keypair generated using %s path.\n", impl);
    } else {
        printf("Failure: Keypair generation failed!\n");
        return 1;
    }
    
    printf("----------------------------------------\n");
    return 0;
}
