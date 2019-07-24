#include "note.hpp"

Note::Note() {

    std::string a_pk_str;
    get_randomness(a_pk_str);
    if(!a_pk_str.empty()) rho = uint256S(a_pk_str);

    std::string rho_str;
    get_randomness(rho_str);
    if(!rho_str.empty()) rho = uint256S(rho_str);
}

uint256 Note::cm() {
    std::string data;
    data += a_pk.ToString();
    data += to_string(value);
    data += rho.ToString();
    std::string output;
    sha256(data, output);
    return uint256S(output);
}

uint256 Note::nullifier(const uint256& a_sk) const {
    std::string output;
    sha256(rho.ToString(), output);
    return uint256S(output); 
}
