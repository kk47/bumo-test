#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <sstream>

#include <stdio.h>

#include "note.hpp"
#include "utils/util.h"
#include "api.hpp"


int main(int argc, char *argv[])
{
    /*srand((unsigned)time(0));
    for(int i=0; i<100000000; ++i) {
        std::cout << rand() << std::endl; 
    }*/
    
    /*unsigned char a_pk[32];
    unsigned char a_sk[32];
    get_keypair(a_sk, a_pk);
    print_char_array(a_pk, 32);
    print_char_array(a_sk, 32);
    std::cout << sizeof(a_sk) << ":" << sizeof(a_pk) << std::endl;
    */

    /*std::string rho_str = "dedeffdddedeffdddedeffdddedeffdddedeffdddedeffdddedeffdddedeffdd";
    std::string pk_str = "e8e55f617b4b693083f883f70926dd5673fa434cefa3660828759947e2276348";
    std::string sk_str = "f0f0f0f00f0f0ffffffffff000000f0f0f0f0f00f0000f0f00f00f0f0f0f00ff";
    
    unsigned char rho[rho_str.size()/2];
    unsigned char pk[32];
    unsigned char sk[32];
    unsigned char send_nf[32];
    unsigned char send_nf1[32];
    unsigned char cm[32];
    unsigned char spend_nf[32];

    hex_str_to_array(rho_str, rho);
    hex_str_to_array(pk_str, pk);
    hex_str_to_array(sk_str, sk);

    //computeSendNullifier(rho, send_nf);
    computeCommitment(rho, pk, 2378237, cm);
    computeSpendNullifier(rho, sk, spend_nf);

    //print_char_array(send_nf, 32);
    print_char_array(cm, 32);
    print_char_array(spend_nf, 32);*/
    
    std::string rho_str = "0000000000000000000000000000000000000000000000000000000000000000";
    unsigned char rho[64];
    unsigned char rho_hash[32];
    hex_str_to_array(rho_str+rho_str, rho);
    sha256(rho, 32, rho_hash);
    print_char_array(rho_hash, 32);

    std::cout << sha256(rho_str + rho_str) << std::endl;

    return 0;
}
