#ifndef _UTIL_H_
#define _UITL_H_
#include <string>


template <typename T>
std::string to_string(T value);
static std::string BinToHexString(const char *value, int len);
void print_char_array(unsigned char *array, int len);

void sha256(unsigned char *str, int len, unsigned char *buf);
std::string sha256(const std::string& hex_str);

void hex_str_to_array(const std::string& hex_str, unsigned char *array);
std::string array_to_hex_str(unsigned char *array, int len);

template<class T>
T get_randomness();
void get_randomness(std::string &output, int len);
void get_randomness(unsigned char *output, int len);

void get_keypair(unsigned char *priv, unsigned char *pub);
#endif
