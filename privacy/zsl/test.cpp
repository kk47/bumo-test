#include <iostream>
#include <string>

#include "api.hpp"
#include "utils/util.h"
#include "merkle_tree.hpp"
#include "zsl/snark/zsl.h"

unsigned char rho[32];
unsigned char pk[32];
unsigned char sk[32];
uint64_t value = 0;

bool shielding()
{
    get_keypair(sk, pk);
    
    std::cout << "a_pk:";
    print_char_array(pk, 32);
    std::cout << "a_sk:";
    print_char_array(sk, 32);
    
    value = 2378237 ;
    get_randomness(rho, 32);
    //std::cout << "rho:";
    //print_char_array(rho, 32);


    // zsl_prove_shielding(void *rho, void *pk, uint64_t value, void *output_proof);
    unsigned char proof_str[584];
    std::cout << "here" << std::endl;
    zsl_prove_shielding(rho, pk, value, proof_str);
    //std::cout << "shielding proof:";
    //print_char_array(proof_str, 32);
    
    unsigned char send_nf[32];
    unsigned char cm[32];
    computeSendNullifier(rho, send_nf);
    computeCommitment(rho, pk, value, cm);
    std::cout << "send_nf:";
    print_char_array(send_nf, 32);
    std::cout << "cm:";
    print_char_array(cm, 32);

    bool ret = zsl_verify_shielding(proof_str, send_nf, cm, value);
    if(!ret) {
        std::cout << "shield verify failed" << std::endl;
    } else {
        std::cout << "shield verify done" << std::endl;
    }
    return true;
}

int main(int argc, char *argv[])
{
    /*MerkleTree mt(29);
    mt.add_commitment(cm_str);
    std::cout << "commitment is:" << cm_str << std::endl;
    
    mt.debug_string();

    std::cout << "empty roots is:" << std::endl;
    for(int i = 0; i < mt.empty_roots.size(); i++) {
        std::cout << mt.empty_roots[i] << std::endl;
    }

    std::cout << "commitments map:" << std::endl;
    std::map<uint32_t, std::string>::iterator it = mt.map_commitments.begin();
    for ( ; it != mt.map_commitments.end(); it++) {
        std::cout << it->first << ":" << it->second << std::endl;
    }

    uint32_t index = 0;
    std::vector<std::string> uncles;
    mt.get_witness(cm_str, index, uncles);
    std::cout << index << std::endl;
    for(int i = 0; i < uncles.size(); i++) {
        std::cout << uncles[i] << std::endl;
    }
    
    uint64_t value = 2378237;
    unsigned char proof_str[584];
    zsl_prove_shielding(rho, a_pk, value, proof_str);
    //std::cout << "proof_str:";
    //print_char_array(proof_str, 584);

    bool ret = zsl_verify_shielding(proof_str, send_nf, cm, value);
    if(!ret) {
        std::cout << "shield verify failed" << std::endl;
    } else {
        std::cout << "shield verify done" << std::endl;
    }*/
    bool ret = shielding();

    return 0;
}
