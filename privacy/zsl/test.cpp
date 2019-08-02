#include <iostream>
#include <string>

#include "note.hpp"
#include "api.hpp"
#include "utils/util.h"
#include "merkle_tree.hpp"
#include "utils/sha256.h"

std::string sk= "f0f0f0f00f0f0ffffffffff000000f0f0f0f0f00f0000f0f00f00f0f0f0f00ff";
std::string pk= "e8e55f617b4b693083f883f70926dd5673fa434cefa3660828759947e2276348";
std::string rho= "dedeffdddedeffdddedeffdddedeffdddedeffdddedeffdddedeffdddedeffdd";


bool shielding()
{
    uint64_t value = 2378237 ;
    std::cout << "rho:" << rho<< std::endl;
    std::cout << "pk:" << pk<< std::endl;
    std::cout << "sk:" << sk<< std::endl;
    

    std::string proof;
    zsl_prove_shielding(rho, pk, value, proof);
    //std::cout << "shielding proof:" << proof << std::endl;;
    
    std::string send_nf = Note::computeSendNullifier(rho);
    std::cout << "send_nf:" << send_nf << std::endl;

    std::string cm = Note::computeCommitment(rho, pk, value);
    std::cout << "cm:" << cm << std::endl;
    
    bool ret = zsl_verify_shielding(proof, send_nf, cm, value);
    return ret;
}

bool unshielding()
{
    MerkleTree mt(29);
    uint64_t index = 0;
    uint64_t value = 2378237 ;

    std::string cm = Note::computeCommitment(rho, pk, value);
    mt.add_commitment(cm);
    std::vector<std::string> uncles;
    mt.get_witness(cm, index, uncles);

    std::cout << "index:" << index << std::endl;

    std::string spend_nf = Note::computeSpendNullifier(rho, sk);
    std::cout << "spend_nf:" << spend_nf << std::endl;
    
    value = 2378237;
    std::string proof_u;
    zsl_prove_unshielding(rho, sk, value, index, uncles, proof_u);
    //std::cout << "unshielding proof:" << proof_u << std::endl;
     
    bool ret = zsl_verify_unshielding(proof_u, spend_nf, mt.root(), value);
    return ret;

}

bool shield_transfer()
{
    MerkleTree mt(29);
    std::string input_rho1 = rho;
    //get_randomness(input_rho1, 32);
    Note input_note1(pk, 100, input_rho1);

    std::string pk1;
    std::string sk1;
    get_keypair(sk1, pk1);

    std::string input_rho2;
    get_randomness(input_rho2, 32);
    Note input_note2(pk1, 100, input_rho2);
 
    std::string input_cm1 = input_note1.cm();
    mt.add_commitment(input_cm1);
    std::string input_cm2 = input_note2.cm();
    mt.add_commitment(input_cm2);

    uint64_t index1 = 0;
    std::vector<std::string> auth_path1;
    mt.get_witness(input_cm1, index1, auth_path1);

    uint64_t index2 = 0;
    std::vector<std::string> auth_path2;
    mt.get_witness(input_cm2, index2, auth_path2);
 
    std::string output_pk;
    std::string output_sk;
    get_keypair(output_sk, output_pk);
    
    std::string output_pk1;
    std::string output_sk1;
    get_keypair(output_sk1, output_pk1);

    std::string output_rho1;
    get_randomness(output_rho1, 32);
    std::string output_rho2;
    get_randomness(output_rho2, 32);

    Note output_note1(output_pk, 150, output_rho1);
    Note output_note2(output_pk1, 50, output_rho2);
 
    std::string anchor = mt.root();
    
    /*std::cout << "input_note1:" << std::endl;
    input_note1.debug_string();
    std::cout << "input_note2:" << std::endl;
    input_note2.debug_string();
    std::cout << "output_note1:" << std::endl;
    output_note1.debug_string();
    std::cout << "output_note1:" << std::endl;
    output_note2.debug_string();*/


    std::string proof_t;
    zsl_prove_transfer(proof_t, input_note1.rho, sk, input_note1.value, index1, auth_path1, input_note2.rho, sk1, input_note2.value, index2, auth_path2, output_note1.rho, output_note1.pk, output_note1.value, output_note2.rho, output_note2.pk, output_note2.value);
    
    bool ret = zsl_verify_transfer(proof_t, anchor, input_note1.spend_nf(sk), input_note2.spend_nf(sk1), output_note1.send_nf(), output_note2.send_nf(), output_note1.cm(), output_note2.cm());
    if (ret) {
        std::cout << "spend the new output note test" << std::endl;
        std::string new_input_cm1 = output_note1.cm();
        mt.add_commitment(new_input_cm1);
        std::string new_input_cm2 = output_note2.cm();
        mt.add_commitment(new_input_cm2);
        
        auth_path1.clear();
        auth_path2.clear();
        mt.get_witness(new_input_cm1, index1, auth_path1);
        mt.get_witness(new_input_cm2, index2, auth_path2);

        get_randomness(output_rho1, 32);
        get_randomness(output_rho2, 32);

        Note new_output_note1(pk, 190, output_rho1);
        Note new_output_note2(output_pk, 10, output_rho2);
     
        std::string anchor = mt.root();

        std::string new_proof;
        zsl_prove_transfer(new_proof, output_note1.rho, output_sk, output_note1.value, index1, auth_path1, output_note2.rho, output_sk1, output_note2.value, index2, auth_path2, new_output_note1.rho, new_output_note1.pk, new_output_note1.value, new_output_note2.rho, new_output_note2.pk, new_output_note2.value);
        
        bool ret = zsl_verify_transfer(new_proof, anchor, output_note1.spend_nf(output_sk), output_note2.spend_nf(output_sk1), new_output_note1.send_nf(), new_output_note2.send_nf(), new_output_note1.cm(), new_output_note2.cm());
    }
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
    /*ret = unshielding();
    if(!ret) {
        std::cout << "unshield verify failed" << std::endl;
    } else {
        std::cout << "unshield verify done" << std::endl;
    }*/

    // sheilding transfer zk-snark test
    //zsl_paramgen_transfer();
    ret = shield_transfer();
    if(!ret) {
        std::cout << "shield tranfer verify failed" << std::endl;
    } else {
        std::cout << "shield tranfer verify done" << std::endl;
    }
    return 0;
}
