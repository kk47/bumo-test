#include <iostream>
#include <string>
#include <chrono>

#include <ctime>
#include <unistd.h>

#include "note.hpp"
#include "api.hpp"
#include "utils/util.h"
#include "merkle_tree.hpp"
#include "utils/sha256.h"


bool test_shielding()
{
    auto t1 = std::chrono::high_resolution_clock::now();
    std::string pk;
    std::string sk;
    std::string rho = get_randomness();

    get_keypair(sk, pk);
    Note note(pk, 100, rho);

    std::cout << "rho:" << rho << std::endl;
    std::cout << "pk:" << pk << std::endl;
    std::cout << "sk:" << sk << std::endl;
    std::cout << "value:" << 100 << std::endl;
    
    std::string proof;
    zsl_prove_shielding(note.rho, note.pk, note.value, proof);
    //std::cout << "shielding proof:" << proof << std::endl;;
    auto t2 = std::chrono::high_resolution_clock::now();
    int64_t duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count();
    std::cout << "Shielding take " << duration/1000000 << " ms" << std::endl;
    
    bool ret = zsl_verify_shielding(proof, note.send_nf(), note.cm(), note.value);
    auto t3 = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t3-t2).count();
    std::cout << "Verify Shielding take " << duration/1000000 << " ms" << std::endl;
    return ret;
}

bool test_unshielding()
{
    auto t1 = std::chrono::high_resolution_clock::now();
    std::string pk;
    std::string sk;
    std::string rho = get_randomness();

    get_keypair(sk, pk);
    Note note(pk, 100, rho);

    std::cout << "" << std::endl; 
    std::cout << "rho:" << rho << std::endl;
    std::cout << "pk:" << pk << std::endl;
    std::cout << "sk:" << sk << std::endl;
    std::cout << "value:" << 100 << std::endl;

    MerkleTree mt(29);
    uint64_t index = 0;
    
    mt.add_commitment(note.cm());

    std::vector<std::string> uncles;
    mt.get_witness(note.cm(), index, uncles);

    std::string proof_u;
    zsl_prove_unshielding(note.rho, sk, note.value, index, uncles, proof_u);
    //std::cout << "unshielding proof:" << proof_u << std::endl;
    auto t2 = std::chrono::high_resolution_clock::now();
    int64_t duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count();
    std::cout << "Unshielding take " << duration/1000000 << " ms" << std::endl;
     
    bool ret = zsl_verify_unshielding(proof_u, note.spend_nf(sk), mt.root(), note.value);
    auto t3 = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t3-t2).count();
    std::cout << "Verify unshielding take " << duration/1000000 << " ms" << std::endl;
    return ret;
}

bool test_shield_transfer()
{
    auto t1 = std::chrono::high_resolution_clock::now();
    MerkleTree mt(29);

    std::string pk1;
    std::string sk1;
    std::string pk2;
    std::string sk2;
    std::string input_rho1 = get_randomness();
    std::string input_rho2 = get_randomness();

    get_keypair(sk1, pk1);
    get_keypair(sk2, pk2);

    Note input_note1(pk1, 100, input_rho1);
    Note input_note2(pk2, 100, input_rho2);
 
    std::string input_cm1 = input_note1.cm();
    std::string input_cm2 = input_note2.cm();
    
    mt.add_commitment(input_cm1);
    mt.add_commitment(input_cm2);
    /*std::cout << "" << std::endl; 
    std::cout << "Add input note 1:" << std::endl;
    input_note1.debug_string();
    std::cout << "" << std::endl; 
    std::cout << "Add input note 2:" << std::endl;
    input_note1.debug_string();*/

    uint64_t index1 = 0;
    uint64_t index2 = 0;
    std::vector<std::string> auth_path1;
    std::vector<std::string> auth_path2;

    mt.get_witness(input_cm1, index1, auth_path1);
    mt.get_witness(input_cm2, index2, auth_path2);
 
    std::string output_pk1;
    std::string output_sk1;
    std::string output_pk2;
    std::string output_sk2;
    std::string output_rho1;
    std::string output_rho2;

    get_keypair(output_sk1, output_pk1);
    get_keypair(output_sk2, output_pk2);

    get_randomness(output_rho1, 32);
    get_randomness(output_rho2, 32);

    Note output_note1(output_pk1, 150, output_rho1);
    Note output_note2(output_pk2, 50, output_rho2);
 
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
    zsl_prove_transfer(proof_t, input_note1.rho, sk1, input_note1.value, index1, auth_path1, input_note2.rho, sk2, input_note2.value, index2, auth_path2, output_note1.rho, output_note1.pk, output_note1.value, output_note2.rho, output_note2.pk, output_note2.value);
    auto t2 = std::chrono::high_resolution_clock::now();
    int64_t duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count();
    std::cout << "Prove transfer take " << duration/1000000  << " ms" << std::endl;
    
    bool ret = zsl_verify_transfer(proof_t, anchor, input_note1.spend_nf(sk1), input_note2.spend_nf(sk2), output_note1.send_nf(), output_note2.send_nf(), output_note1.cm(), output_note2.cm());
    auto t3 = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t3-t2).count();
    std::cout << "Verify transfer take " << duration/1000000 << " ms" << std::endl;
    if (!ret) {
        return false;
    }
    std::cout << "" << std::endl; 
    std::cout << "Shield transfer for input note to output note done" << std::endl;
    std::string new_input_cm1 = output_note1.cm();
    std::string new_input_cm2 = output_note2.cm();

    mt.add_commitment(new_input_cm1);
    mt.add_commitment(new_input_cm2);

    /*std::cout << "Add Onput note 1:" << std::endl;
    output_note1.debug_string();
    std::cout << "" << std::endl; 
    std::cout << "Add Onput note 2:" << std::endl;
    output_note2.debug_string();*/

    auth_path1.clear();
    auth_path2.clear();
    mt.get_witness(new_input_cm1, index1, auth_path1);
    mt.get_witness(new_input_cm2, index2, auth_path2);
    
    output_rho1 = get_randomness();
    output_rho2 = get_randomness();

    Note new_output_note1(pk1, 190, output_rho1);
    Note new_output_note2(output_pk1, 10, output_rho2);
 
    anchor = mt.root();
    
    std::cout << "" << std::endl; 
    std::cout << "Spend the output note and generate new output note" << std::endl;
    std::cout << "New output note 1:" << std::endl;
    new_output_note1.debug_string();
    std::cout << "" << std::endl; 
    std::cout << "New output note 2:" << std::endl;
    new_output_note2.debug_string();

    std::string new_proof;
    zsl_prove_transfer(new_proof, output_note1.rho, output_sk1, output_note1.value, index1, auth_path1, output_note2.rho, output_sk2, output_note2.value, index2, auth_path2, new_output_note1.rho, new_output_note1.pk, new_output_note1.value, new_output_note2.rho, new_output_note2.pk, new_output_note2.value);
    auto t4 = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t4-t3).count();
    std::cout << "Prove transfer take " << duration/1000000 << " ms" << std::endl;
    
    ret = zsl_verify_transfer(new_proof, anchor, output_note1.spend_nf(output_sk1), output_note2.spend_nf(output_sk2), new_output_note1.send_nf(), new_output_note2.send_nf(), new_output_note1.cm(), new_output_note2.cm());
    auto t5 = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t5-t4).count();
    std::cout << "Verify transfer take " << duration/1000000 << " ms" << std::endl;

    return ret;
}

void init_param()
{
    auto t1 = std::chrono::high_resolution_clock::now();
    zsl_initialize();
    
    zsl_paramgen_shielding();

    zsl_paramgen_unshielding();

    zsl_paramgen_transfer();
    auto t2 = std::chrono::high_resolution_clock::now();
    int64_t duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count();
    std::cout << "Init setup params take: " << duration / 1000000 << " ms" << std::endl;
}

int main(int argc, char *argv[])
{
    
    //init_param();
    
    // zsl initialization, should be call before shielding and unshileding
    zsl_initialize();

    // shielding zk-snark test
    bool ret = test_shielding();
    if(!ret) {
        std::cout << "Shielding verify failed" << std::endl;
    } else {
        std::cout << "Shielding verify done" << std::endl;
    }

    // unsheilding zk-snark test
    ret = test_unshielding();
    if(!ret) {
        std::cout << "Unshielding verify failed" << std::endl;
    } else {
        std::cout << "Unshielding verify done" << std::endl;
    }

    // sheilding transfer zk-snark test
    ret = test_shield_transfer();
    if(!ret) {
        std::cout << "Shield tranfer verify failed" << std::endl;
    } else {
        std::cout << "Shield tranfer verify done" << std::endl;
    }
    return 0;
}
