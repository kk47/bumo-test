#include <iostream>
#include <string>

#include "utils/util.h"


int main()
{
    std::string a = "0000000000000000000000000000000000000000000000000000000000000000";
    std::string b = "58e38183982c6f7981e9f3ce0a735fdd4ca2f0cd88db6ee608c2fe1e84142d0d";
    std::cout << sha256_compress(a, a) << std::endl;
    std::cout << sha256_compress(b, a) << std::endl;
    return 0;
}
