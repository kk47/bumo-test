#ifndef _MERKLE_TREE_HPP_
#define _MERKLE_TREE_HPP_

#include <string>
#include <vector>
#include <map>

class MerkleTree
{
public:
    uint32_t tree_depth;
    uint32_t max_num_elements;
    uint32_t num_commitments;

    std::vector<std::string> empty_roots;
    std::map<uint32_t, std::string> map_commitments;
    //std::map<std::string, uint32_t> map_commitments_indices;
    
    MerkleTree(uint32_t depth) : tree_depth(depth), num_commitments(0) {
        max_num_elements = 1 << depth;
        create_empty_roots(depth);
    };
    virtual ~MerkleTree() {};
    
    void create_empty_roots(uint32_t depth);
    std::string get_empty_root(uint32_t depth);
    std::vector<std::string>& get_empty_roots();
    
    std::string combine(const std::string& left, const std::string& right);
    //uint32_t get_leaf_index(std::string& cm);
    //std::string get_commitment_at_leaf_index(uint32_t index);
    void add_commitment(const std::string& cm);
    bool commitment_exists(const std::string& cm);
    void get_witness(const std::string& cm, uint64_t& index, std::vector<std::string>& uncles);
   
    std::string root();
    uint32_t size();
    uint32_t capacity();
    uint32_t available();
    uint32_t depth();
    void debug_string();
private:
    std::string calc_subtree(uint32_t index, uint32_t item_depth);
    uint64_t left_shift(uint64_t v, uint64_t n);
    uint64_t right_shift(uint64_t v, uint64_t n);
};

#endif
