#include <iostream>
#include <string>

#include "note.hpp"
#include "utils/util.h"
#include "zsl/snark/zsl.h"
#include "merkle_tree.hpp"

unsigned char rho[32];
unsigned char pk[32];
unsigned char sk[32];
unsigned char spend_nf[32];
unsigned char send_nf[32];
unsigned char cm[32];
unsigned char proof_s[584];
uint64_t value = 0;

MerkleTree mt(29);

bool shielding()
{
    get_keypair(sk, pk);
    get_randomness(rho, 32);
    
    std::cout << "a_pk:";
    print_char_array(pk, 32);
    std::cout << "a_sk:";
    print_char_array(sk, 32);
    std::cout << "rho:";
    print_char_array(rho, 32);
    
    /*std::string sk_str = "f0f0f0f00f0f0ffffffffff000000f0f0f0f0f00f0000f0f00f00f0f0f0f00ff";
    std::string pk_str = "e8e55f617b4b693083f883f70926dd5673fa434cefa3660828759947e2276348";
    std::string rho_str = "dedeffdddedeffdddedeffdddedeffdddedeffdddedeffdddedeffdddedeffdd";
    
    hex_str_to_array(sk_str, sk);
    hex_str_to_array(pk_str, pk);
    hex_str_to_array(rho_str, rho);
    std::cout << "rho:";
    print_char_array(rho, 32);
    std::cout << "pk:";
    print_char_array(pk, 32);
    std::cout << "sk:";
    print_char_array(sk, 32);*/

    value = 2378237 ;

    // zsl_prove_shielding(void *rho, void *pk, uint64_t value, void *output_proof);
    zsl_prove_shielding(rho, pk, value, proof_s);
    //std::cout << "shielding proof:";
    //print_char_array(proof_str, 32);
    
    
    computeSendNullifier(rho, send_nf);
    std::cout << "send_nf:";
    print_char_array(send_nf, 32);

    computeCommitment(rho, pk, value, cm);
    std::cout << "cm:";
    print_char_array(cm, 32);

    bool ret = zsl_verify_shielding(proof_s, send_nf, cm, value);
    return ret;
}

void mt_debug()
{
    /*std::cout << "empty roots is:" << std::endl;
    for(int i = 0; i < mt.empty_roots.size(); i++) {
        std::cout << mt.empty_roots[i] << std::endl;
    }*/
    /*std::cout << "commitments map:" << std::endl;
    std::map<uint32_t, std::string>::iterator it = mt.map_commitments.begin();
    for ( ; it != mt.map_commitments.end(); it++) {
        std::cout << it->first << ":" << it->second << std::endl;
    }*/
    
    std::cout << "root is:" << mt.root() << std::endl;
}

bool unshielding()
{
    std::string cm_str = array_to_hex_str(cm, 32);
    mt.add_commitment(cm_str);
    std::cout << "commitment is:" << cm_str << std::endl;

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

    computeSpendNullifier(rho, sk, spend_nf);
    std::cout << "spend_nf:";
    print_char_array(spend_nf, 32);
    
    unsigned char root[32];
    //std::string rt_str = "8610652739ac0c6bb6b5353649bb822b26543f0ebe88f32a489a56843cd04f03";
    hex_str_to_array(mt.root(), root);
    mt_debug();
    
    unsigned char proof_u[584] = {0x00};
    //zsl_prove_unshielding(rho, sk, value, 0, auth_path, proof_u);
    zsl_prove_unshielding(rho, sk, value, index, auth_path, proof_u);
    
    bool ret = zsl_verify_unshielding(proof_u, spend_nf, root, value);
    return ret;
}

bool shield_transfer()
{
    std::string cm_str = array_to_hex_str(cm, 32);
    mt.add_commitment(cm_str);
    std::cout << "commitment is:" << cm_str << std::endl;

    uint32_t index = 0;
    std::vector<std::string> uncles;
    mt.get_witness(cm_str, index, uncles);
    
    std::cout << index << std::endl;
    unsigned char auth_path_1[29][32];
    for(int i = 0; i < uncles.size(); i++) {
        unsigned char item[32];
        hex_str_to_array(uncles[i], item);
        for(int j = 0; j < 32; j++) {
            auth_path_1[i][j] = item[j];
        }
        //std::cout << uncles[i] << std::endl;
    }
    unsigned char auth_path_2[29][32];
    for(int i = 0; i < uncles.size(); i++) {
        unsigned char item[32];
        hex_str_to_array(uncles[i], item);
        for(int j = 0; j < 32; j++) {
            auth_path_2[i][j] = item[j];
        }
        //std::cout << uncles[i] << std::endl;
    }

    unsigned char output_proof_ptr[584] = {0x0};
    unsigned char input_rho_ptr_1[] = {0xde, 0xde, 0xff, 0xdd, 0xde, 0xde, 0xff, 0xdd, 0xde, 0xde, 0xff, 0xdd, 0xde, 0xde, 0xff, 0xdd, 0xde, 0xde, 0xff, 0xdd, 0xde, 0xde, 0xff, 0xdd, 0xde, 0xde, 0xff, 0xdd, 0xde, 0xde, 0xff, 0xdd};
    unsigned char input_pk_ptr_1[] = {0xf0, 0xf0, 0xf0, 0xf0, 0x0f, 0x0f, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x00, 0xf0, 0x00, 0x0f, 0x0f, 0x00, 0xf0, 0x0f, 0x0f, 0x0f, 0x0f, 0x00, 0xff};
    uint64_t input_value_1 = 2378237;
    uint64_t input_tree_position_1 = 0;

    unsigned char input_authentication_path_ptr_1[29][32];
    unsigned char item[32] = {0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x01, 0x00};
    for(int i = 0; i < 29; i++) {
        //unsigned char item[32];
        //hex_str_to_array(uncles[i], item);
        for(int j = 0; j < 32; j++) {
            input_authentication_path_ptr_1[i][j] = item[j];
        }
        //std::cout << uncles[i] << std::endl;
    }

    unsigned char input_rho_ptr_2[] = {0xde, 0xde, 0xff, 0xdd, 0xde, 0xde, 0xff, 0xdd, 0xde, 0xde, 0xff, 0xdd, 0xde, 0xde, 0xff, 0xdd, 0xde, 0xde, 0xff, 0xdd, 0xde, 0xde, 0xff, 0xdd, 0xde, 0xde, 0xff, 0xdd, 0xde, 0xde, 0xff, 0xdd};
    unsigned char input_pk_ptr_2[] = {0xf0, 0xf0, 0xf0, 0xf0, 0x0f, 0x0f, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x00, 0xf0, 0x00, 0x0f, 0x0f, 0x00, 0xf0, 0x0f, 0x0f, 0x0f, 0x0f, 0x00, 0xff};
    uint64_t input_value_2 = 2378237;
    uint64_t input_tree_position_2 = 0;

    unsigned char input_authentication_path_ptr_2[29][32];
    for(int i = 0; i < 29; i++) {
        //unsigned char item[32];
        //hex_str_to_array(uncles[i], item);
        for(int j = 0; j < 32; j++) {
            input_authentication_path_ptr_2[i][j] = item[j];
        }
        //std::cout << uncles[i] << std::endl;
    }
    
    
    unsigned char output_rho_ptr_1[] = {0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac};
    unsigned char output_pk_ptr_1[] = {0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb};
    uint64_t output_value_1 = 2378237;
    unsigned char output_rho_ptr_2[] = {0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac};
    unsigned char output_pk_ptr_2[] = {0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb, 0xac, 0xfb};
    uint64_t output_value_2 = 2378237;
    
    unsigned char output_sk[32];
    unsigned char output_pk[32];
    /*std::string output_rho;
    get_randomness(output_rho, 32);
    get_keypair(sk, pk);
    
    std::cout << "a_pk:";
    print_char_array(pk, 32);
    std::cout << "a_sk:";
    print_char_array(sk, 32);
    std::cout << "rho:" << output_rho << std::endl;
    
    unsigned char output_rho_1[32];
    unsigned char output_rho_2[32];
    hex_str_to_array(output_rho, output_rho_1);
    hex_str_to_array(output_rho, output_rho_2);*/

    zsl_prove_transfer(output_proof_ptr, input_rho_ptr_1, input_pk_ptr_1, input_value_1, input_tree_position_1, input_authentication_path_ptr_1, input_rho_ptr_2, input_pk_ptr_2, input_value_2, input_tree_position_2, input_authentication_path_ptr_2, output_rho_ptr_1, output_pk_ptr_1, output_value_1, output_rho_ptr_2, output_pk_ptr_2, output_value_2);
    
    unsigned char anchor_ptr[] = {0x86, 0x10, 0x65, 0x27, 0x39, 0xac, 0x0c, 0x6b, 0xb6, 0xb5, 0x35, 0x36, 0x49, 0xbb, 0x82, 0x2b, 0x26, 0x54, 0x3f, 0x0e, 0xbe, 0x88, 0xf3, 0x2a, 0x48, 0x9a, 0x56, 0x84, 0x3c, 0xd0, 0x4f, 0x03};
    
    
    unsigned char spend_nf_ptr_1[] = {0x45, 0xcc, 0xb2, 0x10, 0x61, 0x33, 0x18, 0xd0, 0xd1, 0x27, 0xe9, 0x94, 0x7c, 0x2c, 0xe6, 0xb1, 0xb2, 0x46, 0xc5, 0xc2, 0xdd, 0x53, 0x48, 0x9c, 0x1f, 0x39, 0x39, 0x78, 0x43, 0x41, 0x0c, 0x1a};
    unsigned char spend_nf_ptr_2[] = {0x45, 0xcc, 0xb2, 0x10, 0x61, 0x33, 0x18, 0xd0, 0xd1, 0x27, 0xe9, 0x94, 0x7c, 0x2c, 0xe6, 0xb1, 0xb2, 0x46, 0xc5, 0xc2, 0xdd, 0x53, 0x48, 0x9c, 0x1f, 0x39, 0x39, 0x78, 0x43, 0x41, 0x0c, 0x1a};
    unsigned char send_nf_ptr_1[] = {0x35, 0xf7, 0xff, 0x84, 0x5e, 0x47, 0x86, 0xd6, 0x44, 0xc9, 0xc9, 0x05, 0xfe, 0x68, 0xfd, 0x0f, 0x60, 0x2f, 0xea, 0x0b, 0xeb, 0x2c, 0x34, 0x1b, 0xc5, 0xd2, 0xde, 0xfc, 0x17, 0xc3, 0x2e, 0x86};
    unsigned char send_nf_ptr_2[] = {0x35, 0xf7, 0xff, 0x84, 0x5e, 0x47, 0x86, 0xd6, 0x44, 0xc9, 0xc9, 0x05, 0xfe, 0x68, 0xfd, 0x0f, 0x60, 0x2f, 0xea, 0x0b, 0xeb, 0x2c, 0x34, 0x1b, 0xc5, 0xd2, 0xde, 0xfc, 0x17, 0xc3, 0x2e, 0x86};
    unsigned char cm_ptr_1[] = {0xd6, 0x21, 0x8a, 0x07, 0x71, 0x4d, 0xd9, 0xfa, 0xc9, 0x2b, 0xde, 0xef, 0xd3, 0xf4, 0xc0, 0xd7, 0x69, 0xf4, 0x0f, 0x4b, 0x04, 0x3a, 0xd7, 0xe6, 0x7e, 0x73, 0x75, 0xed, 0xc4, 0x98, 0xe4, 0x5c};
    unsigned char cm_ptr_2[] = {0xd6, 0x21, 0x8a, 0x07, 0x71, 0x4d, 0xd9, 0xfa, 0xc9, 0x2b, 0xde, 0xef, 0xd3, 0xf4, 0xc0, 0xd7, 0x69, 0xf4, 0x0f, 0x4b, 0x04, 0x3a, 0xd7, 0xe6, 0x7e, 0x73, 0x75, 0xed, 0xc4, 0x98, 0xe4, 0x5c};

    bool ret = zsl_verify_transfer(output_proof_ptr, anchor_ptr, spend_nf_ptr_1, spend_nf_ptr_2, send_nf_ptr_1, send_nf_ptr_2, cm_ptr_1, cm_ptr_2);

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
    ret = unshielding();
    if(!ret) {
        std::cout << "unshield verify failed" << std::endl;
    } else {
        std::cout << "unshield verify done" << std::endl;
    }
    
    // transfer zk-snark test
    //zsl_paramgen_transfer();
    /*ret = shield_transfer();
    if(!ret) {
        std::cout << "shield transfer failed" << std::endl;
    } else {
        std::cout << "shield transfer done" << std::endl;
    }*/
    
    // get keypair test
    /*std::string pub;
    std::string priv;
    get_keypair(pub, priv);
    std::cout << "a_pk:" << pub << std::endl;
    std::cout << "a_sk:" << priv << std::endl;
     */

    return 0;
}
