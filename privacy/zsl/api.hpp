#include <string>
#include <iostream>
#include <vector>

#include "note.hpp"
#include "utils/util.h"
#include "zsl/zsl.h"

void zsl_prove_shielding(
        const std::string& rho,
        const std::string& pk,
        uint64_t value,
        std::string& output_proof
        )
{
    unsigned char rho_ptr[rho.size()/2];
    unsigned char pk_ptr[pk.size()/2];
    unsigned char proof_ptr[584];
    hex_str_to_array(rho, rho_ptr);
    hex_str_to_array(pk, pk_ptr);

    zsl_prove_shielding(rho_ptr, pk_ptr, value, proof_ptr);

    output_proof = array_to_hex_str(proof_ptr, 584);
}

bool zsl_verify_shielding(
        const std::string& proof,
        const std::string& send_nf,
        const std::string& cm,
        uint64_t value
        )
{
    
    unsigned char proof_ptr[proof.size()/2];
    unsigned char send_nf_ptr[send_nf.size()/2];
    unsigned char cm_ptr[cm.size()/2];

    hex_str_to_array(proof, proof_ptr);
    hex_str_to_array(send_nf, send_nf_ptr);
    hex_str_to_array(cm, cm_ptr);

    return zsl_verify_shielding(proof_ptr, send_nf_ptr, cm_ptr, value);
}

void zsl_prove_unshielding(
        const std::string& rho,
        const std::string& sk,
        uint64_t value,
        uint64_t tree_position,
        std::vector<std::string>& auth_path,
        std::string& output_proof
        )
{
    unsigned char rho_a[rho.size()/2];
    unsigned char sk_a[sk.size()/2];

    hex_str_to_array(rho, rho_a);
    hex_str_to_array(sk, sk_a);
    
    unsigned char auth_path_a[29][32];
    for(int i = 0; i < auth_path.size(); i++) {
        unsigned char item[32];
        hex_str_to_array(auth_path[i], item);
        for(int j = 0; j < 32; j++) {
            auth_path_a[i][j] = item[j];
        }
    }

    unsigned char proof[584];
    zsl_prove_unshielding(rho_a, sk_a, value, tree_position, auth_path_a, proof);

    output_proof = array_to_hex_str(proof, 584);
}

bool zsl_verify_unshielding(
        const std::string& proof,
        const std::string& spend_nf,
        const std::string& root,
        uint64_t value
        )
{
    
    unsigned char proof_ptr[proof.size()/2];
    unsigned char spend_nf_ptr[spend_nf.size()/2];
    unsigned char root_ptr[root.size()/2];

    hex_str_to_array(proof, proof_ptr);
    hex_str_to_array(spend_nf, spend_nf_ptr);
    hex_str_to_array(root, root_ptr);

    return zsl_verify_unshielding(proof_ptr, spend_nf_ptr, root_ptr, value);
}

void zsl_prove_transfer(
	std::string& output_proof,
	const std::string& input_rho_1,
	const std::string& input_pk_1,
	uint64_t input_value_1,
	uint64_t input_tree_position_1,
	const std::vector<std::string>& auth_path_1,
	const std::string& input_rho_2,
	const std::string& input_pk_2,
	uint64_t input_value_2,
	uint64_t input_tree_position_2,
	const std::vector<std::string>& auth_path_2,
	const std::string& output_rho_1,
	const std::string& output_pk_1,
	uint64_t output_value_1,
	const std::string& output_rho_2,
	const std::string& output_pk_2,
	uint64_t output_value_2
    )
{

    unsigned char proof[584];
    unsigned char input_rho_ptr_1[32];
    unsigned char input_rho_ptr_2[32];
    unsigned char input_pk_ptr_1[32];
    unsigned char input_pk_ptr_2[32];
    unsigned char output_pk_ptr_1[32];
    unsigned char output_pk_ptr_2[32];
    unsigned char output_rho_ptr_1[32];
    unsigned char output_rho_ptr_2[32];
    
    hex_str_to_array(input_rho_1, input_rho_ptr_1);
    hex_str_to_array(input_rho_2, input_rho_ptr_2);
    hex_str_to_array(input_pk_1, input_pk_ptr_1);
    hex_str_to_array(input_pk_2, input_pk_ptr_2);
    hex_str_to_array(output_rho_1, output_rho_ptr_1);
    hex_str_to_array(output_rho_2, output_rho_ptr_2);
    hex_str_to_array(output_pk_1, output_pk_ptr_1);
    hex_str_to_array(output_pk_2, output_pk_ptr_2);
    
    unsigned char auth_path_ptr_1[29][32];
    for(int i = 0; i < 29; i++) {
        unsigned char item[32];
        hex_str_to_array(auth_path_1[i], item);
        for(int j = 0; j < 32; j++) {
            auth_path_ptr_1[i][j] = item[j];
        }
    }

    unsigned char auth_path_ptr_2[29][32];
    for(int i = 0; i < 29; i++) {
        unsigned char item[32];
        hex_str_to_array(auth_path_2[i], item);
        for(int j = 0; j < 32; j++) {
            auth_path_ptr_2[i][j] = item[j];
        }
    }
    
    /*std::cout << "input_rho_ptr_1:";
    print_char_array(input_rho_ptr_1, 32);
    std::cout << "input_pk_ptr_1:";
    print_char_array(input_pk_ptr_1, 32);
    std::cout << "input_rho_ptr_2:";
    print_char_array(input_rho_ptr_2, 32);
    std::cout << "input_pk_ptr_2:";
    print_char_array(input_pk_ptr_2, 32);
    std::cout << "output_rho_ptr_1:";
    print_char_array(output_rho_ptr_1, 32);
    std::cout << "output_pk_ptr_1:";
    print_char_array(output_pk_ptr_1, 32);
    std::cout << "output_rho_ptr_2:";
    print_char_array(output_rho_ptr_2, 32);
    std::cout << "output_pk_ptr_2:";
    print_char_array(output_pk_ptr_2, 32);
    std::cout << "input_value_1:" << input_value_1 << std::endl;
    std::cout << "input_value_2:" << input_value_2 << std::endl;
    std::cout << "output_value_1:" << output_value_1 << std::endl;
    std::cout << "output_value_2:" << output_value_2 << std::endl;
    std::cout << "input_tree_position_1:" << input_tree_position_1 << std::endl;
    std::cout << "input_tree_position_2:" << input_tree_position_2 << std::endl;*/

    zsl_prove_transfer(proof,
            input_rho_ptr_1, input_pk_ptr_1, input_value_1, input_tree_position_1, auth_path_ptr_1,
            input_rho_ptr_2, input_pk_ptr_2, input_value_2, input_tree_position_2, auth_path_ptr_2,
            output_rho_ptr_1, output_pk_ptr_1, output_value_1,
            output_rho_ptr_2, output_pk_ptr_2, output_value_2);

    output_proof = array_to_hex_str(proof, 584);
}

void zsl_prove_transfer(std::string& proof,
            Note& input_note1, Note& input_note2,
            uint64_t index1, const std::vector<std::string>& auth_path1,
            uint64_t index2, const std::vector<std::string>& auth_path2,
            Note& output_note1, Note& output_note2
    )
{
    zsl_prove_transfer(proof,
            input_note1.rho, input_note1.pk, input_note1.value, index1, auth_path1,
            input_note2.rho, input_note2.pk, input_note2.value, index2, auth_path2,
            output_note1.rho, output_note1.pk, output_note1.value,
            output_note2.rho, output_note2.pk, output_note2.value);

} 

bool zsl_verify_transfer(
        const std::string& proof_ptr,
        const std::string& anchor_ptr,
        const std::string& spend_nf_ptr_1,
        const std::string& spend_nf_ptr_2,
        const std::string& send_nf_ptr_1,
        const std::string& send_nf_ptr_2,
        const std::string& cm_ptr_1,
        const std::string& cm_ptr_2
    )
{

    unsigned char proof[584];
    unsigned char anchor[32];
    unsigned char spend_nf_1[32];
    unsigned char spend_nf_2[32];
    unsigned char send_nf_1[32];
    unsigned char send_nf_2[32];
    unsigned char cm_1[32];
    unsigned char cm_2[32];

    hex_str_to_array(proof_ptr, proof);
    hex_str_to_array(anchor_ptr, anchor);
    hex_str_to_array(spend_nf_ptr_1, spend_nf_1);
    hex_str_to_array(spend_nf_ptr_2, spend_nf_2);
    hex_str_to_array(send_nf_ptr_1, send_nf_1);
    hex_str_to_array(send_nf_ptr_2, send_nf_2);
    hex_str_to_array(cm_ptr_1, cm_1);
    hex_str_to_array(cm_ptr_2, cm_2);

    return zsl_verify_transfer(proof, anchor, spend_nf_1, spend_nf_2, send_nf_1, send_nf_2, cm_1, cm_2);
}
