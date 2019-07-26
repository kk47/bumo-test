#ifndef UTIL_H_
#define UTIL_H_

#include <string>
#include <vector>

#include "sha256.h"
#include "uint256.h"

void printChar(const unsigned char c);

void printVector(const std::vector<bool>& v);

void printVector(const std::string str, const std::vector<bool>& v);

void printVectorAsHex(const std::vector<bool>& v);

void printVectorAsHex(const std::string str, const std::vector<bool>& v);

void printBytesVector(const std::vector<unsigned char>& v);

void printBytesVector(const std::string str, const std::vector<unsigned char>& v);

void printBytesVectorAsHex(const std::vector<unsigned char>& v);

void printBytesVectorAsHex(const std::string str, const std::vector<unsigned char>& v);

void getRandBytes(unsigned char* bytes, int num);

void convertBytesToVector(const unsigned char* bytes, std::vector<bool>& v);

void convertVectorToBytes(const std::vector<bool>& v, unsigned char* bytes);

void convertBytesToBytesVector(const unsigned char* bytes, std::vector<unsigned char>& v);

void convertBytesVectorToBytes(const std::vector<unsigned char>& v, unsigned char* bytes);

void convertBytesVectorToVector(const std::vector<unsigned char>& bytes, std::vector<bool>& v);

void convertVectorToBytesVector(const std::vector<bool>& v, std::vector<unsigned char>& bytes);

void convertIntToBytesVector(const uint64_t val_int, std::vector<unsigned char>& bytes);

uint64_t convertBytesVectorToInt(const std::vector<unsigned char>& bytes);

void concatenateVectors(const std::vector<bool>& A, const std::vector<bool>& B, std::vector<bool>& result);

void concatenateVectors(const std::vector<unsigned char>& A, const std::vector<unsigned char>& B, std::vector<unsigned char>& result);

void concatenateVectors(const std::vector<bool>& A, const std::vector<bool>& B, const std::vector<bool>& C, std::vector<bool>& result);

void concatenateVectors(const std::vector<unsigned char>& A, const std::vector<unsigned char>& B, const std::vector<unsigned char>& C, std::vector<unsigned char>& result);

void sha256(unsigned char* input, unsigned char* hash, int len);

void sha256(SHA256_CTX_mod* ctx256, unsigned char* input, unsigned char* hash, int len);

void hashVector(SHA256_CTX_mod* ctx256, const std::vector<bool> input, std::vector<bool>& output);

void hashVector(SHA256_CTX_mod* ctx256, const std::vector<unsigned char> input, std::vector<unsigned char>& output);

void hashVector(const std::vector<bool> input, std::vector<bool>& output);

void hashVector(const std::vector<unsigned char> input, std::vector<unsigned char>& output);

void hashVectors(SHA256_CTX_mod* ctx256, const std::vector<bool> left, const std::vector<bool> right, std::vector<bool>& output);

void hashVectors(SHA256_CTX_mod* ctx256, const std::vector<unsigned char> left, const std::vector<unsigned char> right, std::vector<unsigned char>& output);

void hashVectors(const std::vector<bool> left, const std::vector<bool> right, std::vector<bool>& output);

void hashVectors(const std::vector<unsigned char> left, const std::vector<unsigned char> right, std::vector<unsigned char>& output);

bool VectorIsZero(const std::vector<bool> test);

#endif /* UTIL_H_ */


