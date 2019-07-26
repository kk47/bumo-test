#ifndef _API_HPP_
#define _API_HPP_

#include <string>
#include <iostream>
#include <fstream>

#include "utils/util.h"


void computeSendNullifier(unsigned char *rho, unsigned char *send_nf) {
    unsigned char data[33];
    data[0] = 0x00;

    for(int i = 0; i < 32; i++) {
        data[i+1] = rho[i]; 
    }

    sha256(data, 33, send_nf);

    return;
}

void computeSpendNullifier(unsigned char *rho, unsigned char *sk, unsigned char *spend_nf) {
    unsigned char data[65];
    data[0] = 0x01;

    for(int i = 0; i < 32; i++) {
        data[i+1] = rho[i]; 
    }

    for(int i = 0; i < 32; i++) {
        data[i+33] = sk[i]; 
    }
    
    sha256(data, 65, spend_nf);
 
    return;
}

void computeCommitment(unsigned char *rho, unsigned char *pk, uint64_t value, unsigned char *commitment) {
    unsigned char data[64+sizeof(value)];

    for(int i = 0; i < 32; i++) {
        data[i] = rho[i];
    }

    for(int i = 0; i < 32; i++) {
        data[i+32] = pk[i];
    }

    for(int i = 0; i < sizeof(value); i++) {
        data[i+64] = (value >> (8 * i)) & 255; // little endian, big endian will use << operator
    }

    sha256(data, 64+sizeof(value), commitment);

    return;
}

/*void CreateShielding(unsigned char *rho, unsigned char *pk, uint64_t value, std::map<std::string, std::string>) {

    return;
}


void CreateShieldedTransfer(unsigned char *rho_1, unsigned char *sk_1, uint64_t value_1, uint64_t treeIndex_1, std::vector<std::string> authPath_1,
        unsigned char *rho_2, unsigned char *sk_2, uint64_t value_2, uint64_t treeIndex_1, std::vector<std::string> authPath_2,
        unsigned char *out_rho_1, unsigned char *out_pk_1, uint64_t out_value_1,
        unsigned char *out_rho_2, unsigned char *out_pk_2, uint64_t out_value_2) {
    return;
}*/

#endif
