#ifndef _UTIL_H_
#define _UITL_H_

#include <string>
#include <sstream>

template <typename T>
std::string to_string(T value) {
    std::ostringstream os;
    os << value;
    return os.str();
}

static std::string BinToHexString(const char *value, int len);
void print_char_array(unsigned char *array, int len);

void sha256(unsigned char *str, int len, unsigned char *buf);
void sha256_compress(unsigned char *a, unsigned char *b, unsigned char *output);
std::string sha256_compress(const std::string& a, const std::string& b);
std::string sha256(const std::string& hex_str);

void hex_str_to_array(const std::string& hex_str, unsigned char *array);
std::string array_to_hex_str(unsigned char *array, int len);

template<class T>
T get_randomness();
void get_randomness(std::string &output, int len);
void get_randomness(unsigned char *output, int len);

void get_keypair(unsigned char *priv, unsigned char *pub);
#endif
