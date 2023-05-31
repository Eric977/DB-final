#include <iostream>
#include <vector>
#include <lz4.h>
#include "../include/btree_map.hpp"
#include "../include/encode.hpp"
using namespace std;


int main() {
    // compress for integer
    // std::vector<int> original = {111, 111, 111, 111, 5, 50, 500, 8, 9, 10};
    // std::vector<char> compressed = compress<int>(LZ4, original);
    // std::vector<int> decompressed = decompress<int>(LZ4, compressed, original.size());
    // for (const auto& val : decompressed) {
    //     std::cout << val << " ";
    // }

    // compress for string (char array)
    vector<array<char, 20>> original = {
        array<char, 20> {"helloaaaa"},
        array<char, 20> {"worldbb"},
        array<char, 20> {"hellocc"},
        array<char, 20> {"worlddd"}
    };
    vector<char> compressed = compress<array<char, 20>>(LZ4, original);
    vector<array<char, 20>> decompressed = decompress<array<char, 20>>(LZ4, compressed, original.size());
    for (const auto& val : decompressed) {
        std::cout << val.data() << " ";
    }

    /*
    // Testing B tree
    typedef tlx::btree_map<std::string, std::mstring, std::less<std::string> > btree_type;
    btree_type *bt = new btree_type();
    for (int i = 0; i < 100000; i ++){
        bt->insert2(std::to_string(i), std::to_string(i));
    }

    std::cout << "size = " << bt->get_stats().size << std::endl;
    std::cout << "leaves = " << bt->get_stats().leaves << std::endl;
    std::cout << "inner_nodes = " << bt->get_stats().inner_nodes << std::endl;
    std::cout << "avgfill = " << bt->get_stats().avgfill_leaves() << std::endl;
    std::cout << "mem = " << (256 * bt->get_stats().nodes()) << std::endl;
    */
    return 0;
}
