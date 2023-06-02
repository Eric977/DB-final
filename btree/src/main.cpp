#include <iostream>
#include <vector>
#include <lz4.h>
#include "../include/btree_map.hpp"
#include "../include/encode.hpp"
using namespace std;


int main() {
    // compress for pair<string, string> array
    pair<string, string> data[20] = {
        {"identifier1", "alpha@gamil.com"},
        {"identifier2", "bravo@gamil.com"},
        {"identifier3", "charlie@gmail.com"},
        {"identifier4", "delta@gmail.com"},
        {"identifier5", "echo@gmail.com"},
        {"identifier6", "foxtrot@gmail.com"},
        {"identifier7", "golf@ntu.edu.tw"},
        {"identifier8", "hotel@ntu.edu.tw"},
        {"identifier9", "india@ntu.edu.tw"},
        {"identifier10", "juliett@microsoft.com"},
        {"identifier11", "kilo@microsoft.com"},
        {"identifier12", "lima@microsoft.com"},
        {"identifier13", "mike@microsoft.com"},
        {"identifier14", "november@gmail.com"},
        {"identifier15", "oscar@gmail.com"},
        {"identifier16", "papa@fatcat.com"},
        {"identifier17", "quebec@fatcat.com"},
        {"identifier18", "romeo@fatcat.com"},
        {"identifier19", "sierra@fatcat.com"},
        {"identifier20", "tango@fatcat.com"}
    };
    vector<pair<string, string>> original;
    for (int i = 0; i < 20; i ++) {
        original.push_back(data[i]);
    }

    int srcSize = 0;
    vector<char> compressed = compress<pair<string, string>>(LZ4, original, srcSize);
    vector<pair<string, string>> decompressed = decompress<pair<string, string>>(LZ4, compressed, srcSize, original.size());
    for (const auto& val : decompressed) {
        std::cout << val.first << " " << val.second << " " << std::endl;
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
