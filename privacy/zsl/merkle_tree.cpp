#include "merkle_tree.hpp"
#include "utils/util.h"

void MerkleTree::create_empty_roots(uint32_t depth) {
    std::string root = "0000000000000000000000000000000000000000000000000000000000000000";
    empty_roots.push_back(root);
    for (int i = 0; i < depth-1; i++) {
        root = combine(root, root);
        empty_roots.push_back(root);
    }
}

std::string MerkleTree::get_empty_root(uint32_t depth) {
    if (depth >= empty_roots.size())
        return "";
    return empty_roots[0];
}

std::vector<std::string>& MerkleTree::get_empty_roots() {
    return empty_roots;
}

std::string MerkleTree::combine(const std::string& left, const std::string& right) {
    std::string tmpstr = left + right;
    return sha256(tmpstr);
}

/*uint32_t MerkleTree::get_leaf_index(std::string& cm) {
    uint32_t index = map_commitments_indices[cm];
    return index+1;
}

std::string MerkleTree::get_commitment_at_leaf_index(uint32_t index) {
    if (index < num_commitments) {
        uint32_t map_index = index + 1;
        return map_commitments[map_index];
    }
    return "";
}*/

void MerkleTree::add_commitment(std::string& cm) {
    if (num_commitments >= max_num_elements) return;
    //if (map_commitment_indices.find(cm) != map_commitment_indices.end()) return;
    if (commitment_exists(cm)) return;
    
    uint32_t map_index = ++ num_commitments;
    //map_commitment_indices[cm] = map_index;
    map_commitments[map_index] = cm;
}

bool MerkleTree::commitment_exists(const std::string& cm) {
    //return map_commitment_indices.find(cm) != map_commitment_indices.end();
    std::map<uint32_t, std::string>::iterator it = map_commitments.begin();
    for( ; it != map_commitments.end(); it++) {
        if(it->second == cm) return true;
    }
    return false;
}

std::string MerkleTree::root() {
    return calc_subtree(0, tree_depth);
}

uint32_t MerkleTree::size() {
    return num_commitments;
}

uint32_t MerkleTree::capacity() {
    return max_num_elements;
}

uint32_t MerkleTree::available() {
    return max_num_elements - num_commitments;
}

uint32_t MerkleTree::depth() {
    return tree_depth;
}

std::string MerkleTree::calc_subtree(uint32_t index, uint32_t item_depth) {
    if (num_commitments <= left_shift(index, item_depth)) {
        return empty_roots[item_depth]; 
    }
    
    if (item_depth == 0) {
        uint32_t map_index = index + 1;
        return map_commitments[map_index];
    } else {
        std::string left = calc_subtree(left_shift(index, 1), item_depth - 1);
        std::string right = calc_subtree(left_shift(index, 1) + 1, item_depth - 1);
        return combine(left, right);
    }

}

void MerkleTree::get_witness(const std::string& cm, uint32_t& index, std::vector<std::string>& uncles) {
    std::map<uint32_t, std::string>::iterator it = map_commitments.begin();
    for ( ; it != map_commitments.end(); it++) {
        if (it->second == cm) {
            index = it->first - 1;
            break;
        }
    }
    if (it == map_commitments.end() && index == 0) return;

    uint32_t cur_depth = 0;
    uint32_t cur_index = index;
    uint32_t i = 0;
    while (cur_depth < tree_depth) {
        uncles.push_back(calc_subtree(cur_index^1, cur_depth++));
        cur_index = right_shift(cur_index, 1);
    }
}

uint64_t MerkleTree::left_shift(uint64_t v, uint64_t n) {
    return v * (1 << n);
}

uint64_t MerkleTree::right_shift(uint64_t v, uint64_t n) {
    return v / (1 << n);
}


