/* Simple aes encrypt and decrypt tool by kuangkai@bubi.cn */

#include <iostream>
#include <string>
#include <openssl/aes.h>

#include <string.h>

// compile: g++ -o aes aes-decrypt.cpp --std=c++11 -lssl -lcrypto

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

bool HexStringToBin(const std::string &hex_string, std::string &out_put) {
    if (!IsHexString(hex_string)) {
        return false;
    }
    out_put = HexStringToBin(hex_string);
    return true;
}

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
class Aes {
    public:
    Aes() {};
    ~Aes() {};
    static std::string Crypto(const std::string &input, const std::string &key);
    static std::string Decrypto(const std::string &input, const std::string &key);
    static std::string CryptoHex(const std::string &input, const std::string &key);
    static std::string HexDecrypto(const std::string &input, const std::string &key);
};

std::string Aes::Decrypto(const std::string &input, const std::string &key) {
    if (key.size() != 16 &&
        key.size() != 24 &&
        key.size() != 32
        ) {
        return "";
    }

    /* Input data to encrypt */
    unsigned char aes_input[] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5 };

    /* Init vector */
    unsigned char iv[AES_BLOCK_SIZE];
    memset(iv, 0x00, AES_BLOCK_SIZE);

    /* Buffers for Encryption and Decryption */
    std::string enc_out;
    enc_out.resize(input.size());

    AES_KEY dec_key;
    /* AES-128 bit CBC Decryption */
    memset(iv, 0x00, AES_BLOCK_SIZE); // Don't forget to set iv vector again, or you can't decrypt data properly
    AES_set_decrypt_key((const unsigned char *)key.c_str(), key.size() * 8, &dec_key); // Size of key is in bits
    AES_cbc_encrypt((const unsigned char *)input.c_str(), (unsigned char *)enc_out.c_str(), enc_out.size(), &dec_key, iv, AES_DECRYPT);
    enc_out.resize(strlen(enc_out.c_str()));
    return enc_out;
}

std::string Aes::Crypto(const std::string &input, const std::string &key) {
    if (key.size() != 16 &&
        key.size() != 24 &&
        key.size() != 32
        ) {
        return "key error";
    }

    // Set the encryption length
    size_t len = 0;
    if ((input.size() + 1) % AES_BLOCK_SIZE == 0) {
        len = input.size() + 1;
    } else {
        len = ((input.size() + 1) / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
    }

    // Set the input string
    unsigned char* input_string = (unsigned char*)calloc(len, sizeof(unsigned char));
    if (input_string == NULL) {
        fprintf(stderr, "Unable to allocate memory for input_string\n");
        return "";
    }
    strncpy((char*)input_string, input.c_str(), input.size());

    /* Init vector */
    unsigned char iv[AES_BLOCK_SIZE];
    memset(iv, 0x00, AES_BLOCK_SIZE);

    /* Buffers for Encryption and Decryption */
    std::string enc_out;
    enc_out.resize(len);

    /* AES-128 bit CBC Encryption */
    AES_KEY enc_key;
    AES_set_encrypt_key((const unsigned char *)key.c_str(), key.size() * 8, &enc_key);
    AES_cbc_encrypt((const unsigned char *)input.c_str(), (unsigned char *)enc_out.c_str(), input.size(), &enc_key, iv, AES_ENCRYPT);

    delete input_string;
    return enc_out;
}

std::string Aes::CryptoHex(const std::string &input, const std::string &key) {
    return BinToHexString(Crypto(input, key));
}

std::string Aes::HexDecrypto(const std::string &input, const std::string &key) {
    return Decrypto(HexStringToBin(input), key);
}

int main(int argc, char* argv[])
{
    if(argc != 3 && argc != 4) {
        std::cout << "Usage: ./aes de|en aes_priv_key_string|raw_priv_key_string [aes_key]" << std::endl;
        return 1;
    }

    std::string cmd = argv[1];
    std::string str = argv[2];
    char key[] = { 'H', 'C', 'P', 'w', 'z', '!', 'H', '1', 'Y', '3', 'j', 'a', 'J', '*', '|', 'q', 'w', '8', 'K', '<', 'e', 'o', '7', '>', 'Q', 'i', 'h', ')', 'r', 'P', 'q', '1', 0 };
    
    if(argc == 4) {
        strncpy(key, argv[3], 32);
    }
    key[32] = '\0';
    
    if(cmd == "de") {
        std::cout << "AES decrypt " << str << " is:" << Aes::HexDecrypto(str, key) << std::endl; 
    } else if(cmd == "en") {
        std::cout << "AES encrypt " << str << " is:" << Aes::CryptoHex(str, key) << std::endl; 
    } else {
        std::cout << "Unknown command " << cmd << std::endl;
        return 1;
    }
    
    return 0;
}
