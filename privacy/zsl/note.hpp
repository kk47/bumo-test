#ifndef _NOTE_HPP_
#define _NOTE_HPP_

#include "utils/uint256.h"


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
