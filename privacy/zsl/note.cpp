#include <string>
#include <iostream>
#include <fstream>

#include "note.hpp"
#include "utils/util.h"


Note::Note() {

    get_randomness(pk, 32);
    get_randomness(rho, 32);
}

std::string Note::cm() const {
    return computeCommitment(rho, pk, value);
}

std::string Note::spend_nf(const std::string& sk) const {
    return computeSpendNullifier(rho, sk);
}

std::string Note::send_nf() const {
    return computeSendNullifier(rho);
}

std::string Note::computeSendNullifier(const std::string& rho) {
    unsigned char rho_array[rho.size()/2];
    unsigned char send_nf[32];
    hex_str_to_array(rho, rho_array);
    computeSendNullifier(rho_array, send_nf);

    return array_to_hex_str(send_nf, 32);
}

std::string Note::computeSpendNullifier(const std::string& rho, const std::string& sk) {
    unsigned char rho_array[rho.size()/2];
    unsigned char sk_array[sk.size()/2];
    unsigned char spend_nf[32];
    hex_str_to_array(rho, rho_array);
    hex_str_to_array(sk, sk_array);
    computeSpendNullifier(rho_array, sk_array, spend_nf);

    return array_to_hex_str(spend_nf, 32);
}

std::string Note::computeCommitment(const std::string& rho, const std::string& pk, uint64_t value) {
    unsigned char rho_array[rho.size()/2];
    unsigned char pk_array[pk.size()/2];
    unsigned char cm[32];
    hex_str_to_array(rho, rho_array);
    hex_str_to_array(pk, pk_array);
    computeCommitment(rho_array, pk_array, value, cm);

    return array_to_hex_str(cm, 32);
}

void Note::set_rho(const std::string& new_rho) {
    rho = new_rho;
}

void Note::set_pk(const std::string& new_pk) {
    rho = new_pk;
}

void Note::set_sk(const std::string& new_sk) {
    rho = new_sk;
}

void Note::set_value(uint64_t new_value) {
    rho = new_value;
}

void Note::computeSendNullifier(unsigned char *rho, unsigned char *send_nf) {
    unsigned char data[33];
    data[0] = 0x00;

    for(int i = 0; i < 32; i++) {
        data[i+1] = rho[i]; 
    }

    sha256(data, 33, send_nf);

    return;
}

void Note::computeSpendNullifier(unsigned char *rho, unsigned char *sk, unsigned char *spend_nf) {
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

void Note::computeCommitment(unsigned char *rho, unsigned char *pk, uint64_t value, unsigned char *commitment) {
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
