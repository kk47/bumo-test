#ifndef _NOTE_HPP_
#define _NOTE_HPP_

#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <openssl/sha.h>

#include <stdio.h>

#include "utils/util.h"

template <typename T>
std::string to_string(T value) {
    std::ostringstream os;
    os << value;
    return os.str();
}

// hash function 
void sha256(const std::string input, std::string& output) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input.c_str(), input.size());
    SHA256_Final((unsigned char*)output.c_str(), &sha256);
}

void sha256(unsigned char *str, int len, unsigned char *buf) {
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str, len);
    SHA256_Final(buf, &sha256);
}

/*
void hex_str_to_array(std::string& str, unsigned char * array) {
    unsigned char array[str.size()/2]; 
    for(int i = 0; i < str.size(); i+=2) {
        std::string sub = str.substr(i, 2);
        array[i/2] = strtoul(sub.c_str(), 0, 16);
    }
}
*/
void hex_str_to_array(const std::string& hex_str, unsigned char *array) {
    unsigned int c;
    for (int i = 0; i < hex_str.size(); i+=2) {
        std::istringstream hex_stream(hex_str.substr(i, 2));
        hex_stream >> std::hex >> c;
        array[i/2] = c;
    }
}

void print_char_array(unsigned char *array, int len) {
    for(int i=0; i < len; i++)
        printf("%02x", array[i]);
    printf("\n");
}

/* hex string to hex array */
/*void hex_str_to_array(const std::string& hex_str, unsigned char * array) {
    for(int i = 0; i < hex_str.size(); i+=2) {
        char ch_h = hex_str[i];
        char ch_l = hex_str[i+1];
        unsigned int high = (ch_h >= 'A')? (ch_h - 'A' + 10): (ch_h - '0');
        unsigned int low = (ch_l >= 'A')? (ch_l - 'A' + 10): (ch_l - '0');
        array[i/2] = high * 16 + low;
    }
}*/

/* binary data to hex string */
static std::string BinToHexString(const std::string &value) {
    std::string result;
    result.resize(value.size() * 2);
    for (size_t i = 0; i < value.size(); i++) {
        uint8_t item = value[i];
        uint8_t high = (item >> 4);
        uint8_t low = (item & 0x0F);
        result[2 * i] = (high >= 0 && high <= 9) ? (high + '0') : (high - 10 + 'a');
        result[2 * i + 1] = (low >= 0 && low <= 9) ? (low + '0') : (low - 10 + 'a');
    }
    return result;
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

static void BinToHexArray(const char *value, int len, unsigned char *output) {
    for (size_t i = 0; i < len; i++) {
        uint8_t item = value[i];
        uint8_t high = (item >> 4);
        uint8_t low = (item & 0x0F);
        int valuex = (high << 4) + low;
        output[i] = (char)valuex;
    }
    return;
}

std::string HexStringToBin(const std::string &hex_string, bool force_little = false){
    if (hex_string.size() % 2 != 0 || hex_string.empty() ){
        return "";
    }
    std::string result;
    result.resize(hex_string.size()/2);
    for (size_t i = 0; i < hex_string.size() - 1; i = i + 2){
        uint8_t high = 0;
        if (hex_string[i] >= '0' && hex_string[i] <= '9')
            high = (hex_string[i] - '0');
        else if (hex_string[i] >= 'a' && hex_string[i] <= 'f')
            high = (hex_string[i] - 'a' + 10);
        else if (hex_string[i] >= 'A' && hex_string[i] <= 'F'  && !force_little) {
            high = (hex_string[i] - 'A' + 10);
        }
        else {
            return "";
        }

        uint8_t low = 0;
        if (hex_string[i + 1] >= '0' && hex_string[i + 1] <= '9')
            low = (hex_string[i + 1] - '0');
        else if (hex_string[i + 1] >= 'a' && hex_string[i + 1] <= 'f')
            low = (hex_string[i + 1] - 'a' + 10);
        else  if (hex_string[i + 1] >= 'A' && hex_string[i + 1] <= 'F' && !force_little) {
            low = (hex_string[i + 1] - 'A' + 10);
        }
        else {
            return "";
        }

        int valuex = (high << 4) + low;
        //sscanf(hex_string.substr(i, 2).c_str(), "%x", &valuex);
        result.at(i/2) = (char)valuex;
    }

    return result;
}

static bool IsHexString(const std::string &hex_string) {
    if (hex_string.size() % 2 != 0) {
        return false;
    }
    for (size_t i = 0; i < hex_string.size(); i++) {
        char c = hex_string.at(i);
        if (('0' <= c &&c <= '9') || ('a' <= c&& c <= 'f') || ('A' <= c&& c <= 'F')) {
            continue;
        }
        else
            return false;
    }
    return true;
}
bool HexStringToBin(const std::string &hex_string, std::string &out_put) {
    if (!IsHexString(hex_string)) {
        return false;
    }
    out_put = HexStringToBin(hex_string);
    return true;
}

void HexStringToBin(const std::string &hex_string, unsigned char* output){
    if (hex_string.size() % 2 != 0 || hex_string.empty() ){
        return;
    }
    std::string result;
    result.resize(hex_string.size()/2);
    for (size_t i = 0; i < hex_string.size() - 1; i = i + 2){
        uint8_t high = 0;
        if (hex_string[i] >= '0' && hex_string[i] <= '9')
            high = (hex_string[i] - '0');
        else if (hex_string[i] >= 'a' && hex_string[i] <= 'f')
            high = (hex_string[i] - 'a' + 10);
        else if (hex_string[i] >= 'A' && hex_string[i] <= 'F') {
            high = (hex_string[i] - 'A' + 10);
        }
        else {
            return;
        }

        uint8_t low = 0;
        if (hex_string[i + 1] >= '0' && hex_string[i + 1] <= '9')
            low = (hex_string[i + 1] - '0');
        else if (hex_string[i + 1] >= 'a' && hex_string[i + 1] <= 'f')
            low = (hex_string[i + 1] - 'a' + 10);
        else  if (hex_string[i + 1] >= 'A' && hex_string[i + 1] <= 'F') {
            low = (hex_string[i + 1] - 'A' + 10);
        }
        else {
            return;
        }

        int valuex = (high << 4) + low;
        //sscanf(hex_string.substr(i, 2).c_str(), "%x", &valuex);
        output[i/2] = (char)valuex;
    }
    
    return;
}

/* get random number */
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

/* get random string hash(random_number)
void get_randomness(std::string& r) {
    unsigned long long int random_value = 0;
    size_t size = sizeof(random_value);
    std::ifstream urandom("/dev/urandom", std::ios::in|std::ios::binary);
    if(urandom) {
        urandom.read(reinterpret_cast<char*>(&random_value), size);
        //urandom.read(random_value, sizeof(random_value));
        if(urandom) {
            sha256(to_string(random_value), r);
        }
        urandom.close();
    }
    return;
}*/

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

/* Note class */
class Note {
public:
    uint256 a_pk;
    uint256 rho;

    uint64_t value = 0;

    Note(uint256 a_pk, uint64_t value, uint256 rho)
        : value(value), a_pk(a_pk), rho(rho) {}
    Note();
    ~Note() {};
    
    uint256 cm() const; // hash(rho, a_pk, value)
    uint256 nullifier(const uint256& a_sk) const; // hash(rho, a_sk)
};

#endif
