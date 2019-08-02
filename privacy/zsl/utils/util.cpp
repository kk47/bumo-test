#include <openssl/sha.h>
#include <stdio.h>

#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "sha256.h"


void print_char_array(unsigned char *array, int len) {
    for(int i=0; i < len; i++)
        printf("%02x", array[i]);
    printf("\n");
}

void sha256(unsigned char *str, int len, unsigned char *buf) {
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str, len);
    SHA256_Final(buf, &sha256);
}

void sha256_compress(unsigned char *a, unsigned char *b, unsigned char *output) {
    CSHA256 hasher;
    hasher.Write(a, 32);
    hasher.Write(b, 32);
    hasher.FinalizeNoPadding(output);
}

void hex_str_to_array(const std::string& hex_str, unsigned char *array) {
    unsigned int c;
    for (int i = 0; i < hex_str.size(); i+=2) {
        std::istringstream hex_stream(hex_str.substr(i, 2));
        hex_stream >> std::hex >> c;
        array[i/2] = c;
    }
}

std::string array_to_hex_str(unsigned char *array, int len) {
    std::string result;
    result.resize(len * 2);
    for (size_t i = 0; i < len; i++) {
        uint8_t item = array[i];
        uint8_t high = (item >> 4);
        uint8_t low = (item & 0x0f);
        result[2 * i] = (high >= 0 && high <= 9) ? (high + '0') : (high - 10 + 'a');
        result[2 * i + 1] = (low >= 0 && low <= 9) ? (low + '0') : (low - 10 + 'a');
    }
    return result;
}


std::string sha256(const std::string& hex_str) {
    int len = hex_str.size()/2;
    unsigned char array[len];
    hex_str_to_array(hex_str, array);
    unsigned char buf_arr[32];
    
    sha256(array, len, buf_arr);

    return array_to_hex_str(buf_arr, 32);
}

std::string sha256_compress(const std::string& a, const std::string& b) {
    CSHA256 hasher;
    
    unsigned char char_a[a.size()/2];
    unsigned char char_b[b.size()/2];
    unsigned char char_o[32];
    hex_str_to_array(a, char_a);
    hex_str_to_array(b, char_b);
    hasher.Write(char_a, a.size()/2);
    hasher.Write(char_b, b.size()/2);
    hasher.FinalizeNoPadding(char_o);
    return array_to_hex_str(char_o, 32);
}

static std::string BinToHexString(const char *value, int len) {
    std::string result;
    result.resize(len * 2);
    for (size_t i = 0; i < len; i++) {
        uint8_t item = value[i];
        uint8_t high = (item >> 4);
        uint8_t low = (item & 0x0F);
        result[2 * i] = (high >= 0 && high <= 9) ? (high + '0') : (high - 10 + 'a');
        result[2 * i + 1] = (low >= 0 && low <= 9) ? (low + '0') : (low - 10 + 'a');
    }
    return result;
}

template<class T>
T get_randomness()
{
    union {
        T value;
        char cs[sizeof(T)];
    } u;

    std::ifstream rfin("/dev/urandom");
    rfin.read(u.cs, sizeof(u.cs));
    rfin.close();

    return u.value;
}

void get_randomness(std::string &output, int len) {
    char *buf = new char[len];
    std::ifstream urandom("/dev/urandom", std::ios::in|std::ios::binary);
    if(urandom) {
        urandom.read(buf, len);
        if(urandom) {
            output = BinToHexString(buf, len);
        }
        urandom.close();
    }
    return;
}
void get_randomness(unsigned char *output, int len) {
    std::string hex_str;
    get_randomness(hex_str, len);
    hex_str_to_array(hex_str, output);
    return;
}

/* get new address */
void get_keypair(unsigned char *priv, unsigned char *pub) {
    get_randomness(priv, 32);
    sha256(priv, 32, pub);
}

void get_keypair(std::string& priv, std::string& pub) {
    get_randomness(priv, 32);
    pub = sha256(priv);
}
