#ifndef _NOTE_HPP_
#define _NOTE_HPP_

#include <string>
#include <iostream>


/* Note class */
class Note {
public:
    std::string pk;
    std::string rho;

    uint64_t value = 0;

    Note(std::string pk, uint64_t value, std::string rho)
        : value(value), pk(pk), rho(rho) {}
    Note();
    ~Note() {};
    
    std::string cm() const; // hash(rho, pk, value)
    std::string send_nf() const; // hash(rho)
    std::string spend_nf(const std::string& sk) const; // hash(rho, sk)

    static std::string computeSendNullifier(const std::string& rho);
    static std::string computeSpendNullifier(const std::string& rho, const std::string& sk);
    static std::string computeCommitment(const std::string& rho, const std::string& pk, uint64_t value);

    void debug_string() {
        std::cout << "pk:" << pk << std::endl;
        std::cout << "rho:" << rho << std::endl;
        std::cout << "value:" << value << std::endl;
        std::cout << "cm:" << cm() << std::endl;
        std::cout << "send_nf:" << send_nf() << std::endl;
        //std::cout << "spend_nf:" << spend_nf() << std::endl;
    }

private:
    static void computeSendNullifier(unsigned char *rho, unsigned char *send_nf);
    static void computeSpendNullifier(unsigned char *rho, unsigned char *sk, unsigned char *spend_nf);
    static void computeCommitment(unsigned char *rho, unsigned char *pk, uint64_t value, unsigned char *commitment);
};

#endif
