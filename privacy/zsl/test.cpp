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
unsigned char spend_nf[32];
unsigned char send_nf[32];
unsigned char cm[32];
unsigned char proof_s[584];
uint64_t value = 0;

MerkleTree mt(29);

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
    std::cout << "rho:" << rho_str << std::endl;
    std::cout << "pk:" << pk_str << std::endl;
    std::cout << "sk:" << sk_str << std::endl;
    
    hex_str_to_array(sk_str, sk);
    hex_str_to_array(pk_str, pk);
    hex_str_to_array(rho_str, rho);

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
    if(ret) {
        std::string cm_str = array_to_hex_str(cm, 32);
        mt.add_commitment(cm_str);
        std::cout << "add commitment to merkle tree:" << cm_str << std::endl;
        std::cout << "root is:" << mt.root() << std::endl;
    }
    return ret;
}

bool unshielding()
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

    uint64_t index = 0;
    std::vector<std::string> uncles;
    std::string cm_str = array_to_hex_str(cm, 32);

    mt.get_witness(cm_str, index, uncles);

    std::cout << index << std::endl;
    unsigned char auth_path[29][32];
    for(int i = 0; i < uncles.size(); i++) {
        unsigned char item[32];
        hex_str_to_array(uncles[i], item);
        for(int j = 0; j < 32; j++) {
            auth_path[i][j] = item[j];
        }
        //std::cout << uncles[i] << std::endl;
    }

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

void copy_uchar(unsigned char *src, unsigned char *dst, int len)
{
    for (int i = 0; i < len; i++) {
        dst[i] = src[i];
    }
}

bool shield_transfer()
{
    unsigned char rho_1[32];
    unsigned char pk_1[32];
    
    copy_uchar(rho, rho_1, 32); 
    copy_uchar(pk, pk_1, 32); 

    unsigned char output_sk[32];
    unsigned char output_pk[32];
    unsigned char output_rho[32];

    /*get_keypair(output_sk, output_pk);
    get_randomness(output_rho, 32);
 
    std::cout << "output a_pk:";
    print_char_array(output_pk, 32);
    std::cout << "output a_sk:";
    print_char_array(output_sk, 32);
    std::cout << "output rho:";
    print_char_array(output_rho, 32);
    */

    //std::string sk_str = "f0f0f0f00f0f0ffffffffff000000f0f0f0f0f00f0000f0f00f00f0f0f0f00ff";
    std::string output_pk_str = "acfbacfbacfbacfbacfbacfbacfbacfbacfbacfbacfbacfbacfbacfbacfbacfb";
    std::string output_rho_str = "fbacfbacfbacfbacfbacfbacfbacfbacfbacfbacfbacfbacfbacfbacfbacfbac";
    std::cout << "output rho:" << output_rho_str << std::endl;
    std::cout << "output pk:" << output_pk_str << std::endl;
    //std::cout << "sk:" << sk_str << std::endl;
    
    //hex_str_to_array(sk_str, sk);
    hex_str_to_array(output_pk_str, output_pk);
    hex_str_to_array(output_rho_str, output_rho);
    //unsigned char output_sk_1[32];
    unsigned char output_pk_1[32];
    unsigned char output_rho_1[32];
    
    //copy_uchar(output_sk, output_sk_1, 32); 
    copy_uchar(output_pk, output_pk_1, 32); 
    copy_uchar(output_rho, output_rho_1, 32); 
    
    uint64_t value_1 = 2378237;
    uint64_t output_value = 2378237;
    uint64_t output_value_1 = 2378237;
    uint64_t input_tree_postion = 0;

    
    std::vector<std::string> uncles;
    std::string cm_str = array_to_hex_str(cm, 32);
    mt.get_witness(cm_str, input_tree_postion, uncles);
    uint64_t input_tree_postion_1 = input_tree_postion;
    
    std::cout << "auth_path:" << std::endl;
    unsigned char auth_path[29][32];
    for(int i = 0; i < uncles.size(); i++) {
        unsigned char item[32];
        hex_str_to_array(uncles[i], item);
        for(int j = 0; j < 32; j++) {
            auth_path[i][j] = item[j];
        }
        std::cout << uncles[i] << std::endl;
    }

    std::cout << "auth_path_1:" << std::endl;
    unsigned char auth_path_1[29][32];
    for(int i = 0; i < uncles.size(); i++) {
        unsigned char item[32];
        hex_str_to_array(uncles[i], item);
        for(int j = 0; j < 32; j++) {
            auth_path_1[i][j] = item[j];
        }
        std::cout << uncles[i] << std::endl;
    }

    unsigned char proof_t[584];
    
    std::cout << "rho:";
    print_char_array(rho, 32);
    std::cout << "pk:";
    print_char_array(pk, 32);
    std::cout << "value:" << value << std::endl;
    std::cout << "rho_1:";
    print_char_array(rho_1, 32);
    std::cout << "pk_1:";
    print_char_array(pk_1, 32);
    std::cout << "value_1:" << value_1 << std::endl;
    std::cout << "output_rho:";
    print_char_array(output_rho, 32);
    std::cout << "output_pk:";
    print_char_array(output_pk, 32);
    std::cout << "output_value:" << output_value << std::endl;
    std::cout << "output_rho_1:";
    print_char_array(output_rho_1, 32);
    std::cout << "output_pk_1:";
    print_char_array(output_pk_1, 32);
    std::cout << "output_value_1:" << output_value_1 << std::endl;
    zsl_prove_transfer(proof_t, rho, pk, value, input_tree_postion, auth_path, rho_1, pk_1, value_1, input_tree_postion_1, auth_path_1, output_rho, output_pk, output_value, output_rho_1, output_pk_1, output_value_1);
    
    unsigned char anchor[32];
    hex_str_to_array(mt.root(), anchor);
   
    unsigned char output_send_nf[32];
    computeSendNullifier(output_rho, output_send_nf);

    unsigned char spend_nf_1[32];
    unsigned char output_send_nf_1[32];
    copy_uchar(spend_nf, spend_nf_1, 32);
    copy_uchar(output_send_nf, output_send_nf_1, 32);
    std::cout << "output_send_nf:";
    print_char_array(output_send_nf, 32);
    
    unsigned char output_cm[32];
    unsigned char output_cm_1[32];
    computeCommitment(output_rho, output_pk, output_value, output_cm);
    std::cout << "output_cm:";
    print_char_array(output_cm, 32);
    copy_uchar(output_cm, output_cm_1, 32);
    
    std::cout << "anchor:";
    print_char_array(anchor, 32);
    
    /*std::string proof_str = "";
    hex_str_to_array(proof_str, proof_t);
    std::cout << "proof_t:";
    print_char_array(proof_t, 584);
    print_char_array(spend_nf, 32);
    print_char_array(spend_nf_1, 32);
    print_char_array(output_send_nf, 32);
    print_char_array(output_send_nf_1, 32);
    print_char_array(output_cm, 32);
    print_char_array(output_cm_1, 32);*/
    bool ret = zsl_verify_transfer(proof_t, anchor, spend_nf, spend_nf_1, output_send_nf, output_send_nf_1, output_cm, output_cm_1);
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

    // Transfer shield asset
    
    ret = shield_transfer();
    if(!ret) {
        std::cout << "shield tranfer verify failed" << std::endl;
    } else {
        std::cout << "shield tranfer verify done" << std::endl;
    }
    return 0;
}
