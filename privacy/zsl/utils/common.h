// Copyright (c) 2014 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CRYPTO_COMMON_H
#define BITCOIN_CRYPTO_COMMON_H

#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "endian.h"
//#include "byteswap.h"

#if defined(NDEBUG)
# error "Zcash cannot be compiled without assertions."
#endif

/*#if HAVE_DECL_HTOBE32 == 0
inline uint32_t htobe32(uint32_t host_32bits)
{
    return bswap_32(host_32bits);
}
#endif

#if HAVE_DECL_BE32TOH == 0
inline uint32_t be32toh(uint32_t big_endian_32bits)
{
    return return bswap_32(big_endian_32bits);
}
#endif


#if HAVE_DECL_HTOBE64 == 0
inline uint64_t htobe64(uint64_t host_64bits)
{
    return bswap_64(host_64bits);
}
#endif*/

uint32_t static inline ReadBE32(const unsigned char* ptr)
{
    uint32_t x;
    memcpy((char*)&x, ptr, 4);
    return be32toh(x);
}

void static inline WriteBE32(unsigned char* ptr, uint32_t x)
{
    uint32_t v = htobe32(x);
    memcpy(ptr, (char*)&v, 4);
}

void static inline WriteBE64(unsigned char* ptr, uint64_t x)
{
    uint64_t v = htobe64(x);
    memcpy(ptr, (char*)&v, 8);
}
#endif // BITCOIN_CRYPTO_COMMON_H
