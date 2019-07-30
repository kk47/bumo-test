#include <iostream>
#include <string>

#include "note.hpp"
#include "utils/util.h"
#include "merkle_tree.hpp"
#include "zsl/snark/zsl.h"
#include "utils/sha256.h"

unsigned char rho[32];
unsigned char pk[32];
unsigned char sk[32];
unsigned char send_nf[32];
unsigned char cm[32];
unsigned char proof_s[584];
uint64_t value = 0;


bool shielding()
{
    /*get_keypair(sk, pk);
    get_randomness(rho, 32);
    
    std::cout << "a_pk:";
    print_char_array(pk, 32);
    std::cout << "a_sk:";
    print_char_array(sk, 32);
    std::cout << "rho:";
    print_char_array(rho, 32);
    */
    
    std::string sk_str = "f0f0f0f00f0f0ffffffffff000000f0f0f0f0f00f0000f0f00f00f0f0f0f00ff";
    std::string pk_str = "e8e55f617b4b693083f883f70926dd5673fa434cefa3660828759947e2276348";
    std::string rho_str = "dedeffdddedeffdddedeffdddedeffdddedeffdddedeffdddedeffdddedeffdd";
    
    hex_str_to_array(sk_str, sk);
    hex_str_to_array(pk_str, pk);
    hex_str_to_array(rho_str, rho);

    value = 2378237 ;

    // zsl_prove_shielding(void *rho, void *pk, uint64_t value, void *output_proof);
    zsl_prove_shielding(rho, pk, value, proof_s);
    //std::cout << "shielding proof:";
    //print_char_array(proof_str, 32);
    
    std::cout << "rho:";
    print_char_array(rho, 32);
    std::cout << "pk:";
    print_char_array(pk, 32);

    computeSendNullifier(rho, send_nf);
    std::cout << "send_nf:";
    print_char_array(send_nf, 32);

    computeCommitment(rho, pk, value, cm);
    std::cout << "cm:";
    print_char_array(cm, 32);

    bool ret = zsl_verify_shielding(proof_s, send_nf, cm, value);
    return ret;
}

bool unshielding()
{
    std::string cm_str = array_to_hex_str(cm, 32);
    MerkleTree mt(29);
    
    /*std::cout << "empty roots is:" << std::endl;
    for(int i = 0; i < mt.empty_roots.size(); i++) {
        std::cout << mt.empty_roots[i] << std::endl;
    }*/

    mt.add_commitment(cm_str);
    std::cout << "commitment is:" << cm_str << std::endl;
    std::cout << "root is:" << mt.root() << std::endl;


    /*std::cout << "commitments map:" << std::endl;
    std::map<uint32_t, std::string>::iterator it = mt.map_commitments.begin();
    for ( ; it != mt.map_commitments.end(); it++) {
        std::cout << it->first << ":" << it->second << std::endl;
    }*/

    uint32_t index = 0;
    std::vector<std::string> uncles;

    mt.get_witness(cm_str, index, uncles);

    std::cout << index << std::endl;
    unsigned char auth_path[29][32];
    for(int i = 0; i < uncles.size(); i++) {
        unsigned char item[32];
        hex_str_to_array(uncles[i], item);
        for(int j = 0; j < 32; j++) {
            auth_path[i][j] = item[j];
        }
        std::cout << uncles[i] << std::endl;
    }

    unsigned char spend_nf[32];
    computeSpendNullifier(rho, sk, spend_nf);
    std::cout << "spend_nf:";
    print_char_array(spend_nf, 32);
    
    unsigned char root[32];
    //std::string rt_str = "8610652739ac0c6bb6b5353649bb822b26543f0ebe88f32a489a56843cd04f03";
    hex_str_to_array(mt.root(), root);
    
    unsigned char proof_u[584] = {0x00};
    //zsl_prove_unshielding(rho, sk, value, 0, auth_path, proof_u);
    zsl_prove_unshielding(rho, sk, value, index, auth_path, proof_u);
    
    bool ret = zsl_verify_unshielding(proof_u, spend_nf, root, value);
    return ret;

}

int main(int argc, char *argv[])
{
    zsl_initialize();

    // shielding zk-snark test
    //zsl_paramgen_shielding();
    bool ret = shielding();
    if(!ret) {
        std::cout << "shield verify failed" << std::endl;
    } else {
        std::cout << "shield verify done" << std::endl;
    }

    // unsheilding zk-snark test
    //zsl_paramgen_unshielding();
    ret = unshileding();
    if(!ret) {
        std::cout << "unshield verify failed" << std::endl;
    } else {
        std::cout << "unshield verify done" << std::endl;
    }

    // Transfer shield asset
    // 1. create shield asset



    unsigned char spend_nf[32];
    computeSpendNullifier(rho, sk, spend_nf);
    std::cout << "spend_nf:";
    print_char_array(spend_nf, 32);
    
    // output cm1
    

    // output cm2 or change



    return 0;
}
