#include <iostream>
#include <string>

#include ""
#include "group.h"

#include "modules/rangeproof/pedersen_impl.h"
#include "modules/rangeproof/rangeproof_impl.h"

int pedersen_commit_test(const secp256k1_context* ctx)
{
    secp256k1_ge genp;
    secp256k1_gej rj;
    secp256k1_ge r;
    secp256k1_scalar sec;
    uint64_t value = 100;
    int overflow;
    secp256k1_pedersen_commitment commit;
    
    unsigned char blind[32];
    for (i = 0; i < 32; i++) {
        blind[i] = i + 1;
    }

    secp256k1_generator_load(&genp, secp256k1_generator_h);
    secp256k1_scalar_set_b32(&sec, blind, &overflow);
    if(overflow) {
        std::cout << "set blind scalar overflow" << std::endl;
        return 1;
    }

    secp256k1_pedersen_ecmult(&ctx->ecmult_gen_ctx, rj, sec, value, genp);
    
    if (!secp256k1_gej_is_infinity(&rj)) {
        secp256k1_ge_set_gej(&r, &rj);
        secp256k1_pedersen_commitment_save(&commit, &r);
    }
    secp256k1_gej_clear(&rj);
    secp256k1_ge_clear(&r);
    secp256k1_scalar_clear(&sec);

    return 0;
}


int main()
{
    secp256k1_context* ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
    if(!ctx) {
        std::cout << "Failed to initialize secp256k1_context" << std::endl;
        return 1;
    }
    
    pedersen_commit_test(ctx);
    
    std::cout << "secp256k1 test" << std::endl;
    return 0;
}
