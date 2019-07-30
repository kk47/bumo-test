#include <string>
#include <iostream>
#include <fstream>

#include "note.hpp"
#include "utils/util.h"


Note::Note() {

    std::string a_pk_str;
    get_randomness(a_pk_str, 32);
    if(!a_pk_str.empty()) rho = uint256S(a_pk_str);

    std::string rho_str;
    get_randomness(rho_str, 32);
    if(!rho_str.empty()) rho = uint256S(rho_str);
}

uint256 Note::cm() const {
    std::string data;
    data += a_pk.ToString();
    data += to_string(value);
    data += rho.ToString();
    std::string output = sha256(data);
    return uint256S(output);
}

uint256 Note::nullifier(const uint256& a_sk) const {
    std::string output = sha256(rho.ToString());
    return uint256S(output); 
}


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
