#include <sys/time.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <vector>
#include <thread>
#include <fstream>

#include "../include/encode.hpp"
#include "../include/btree_map.hpp"

using namespace std;

void loadKVpair(string file, vector<string>& keys, vector<string>& values){
    std::ifstream inputFile(file);  
}



int main(){
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Load key value
    vector<string> keys, values;
    loadKVpair("../workloads/sample_big", keys, values);
    // Testing B tree
    typedef tlx::btree_map<string, string, less<string> > btree_type;
    btree_type *bt = new btree_type();
    for (int i = 0; i < keys.size(); i ++){
        bt->insert2(keys[i], values[i]);
    }
    bt->erase_one(keys[0]);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    cout << "size = " << bt->get_stats().size << std::endl;
    // cout << "leaves = " << bt->get_stats().leaves << std::endl;
    // cout << "inner_nodes = " << bt->get_stats().inner_nodes << std::endl;
    // cout << "avgfill = " << bt->get_stats().avgfill_leaves() << std::endl;
    // cout << "mem = " << (256 * bt->get_stats().nodes()) << std::endl;



    ////////////////////////////////////////////////////////////////////
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

    // lz4 compression
    int lz4_srcSize = 0;
    vector<char> lz4_compressed = lz4_compress<pair<string, string>>(original, lz4_srcSize);
    vector<pair<string, string>> lz4_decompressed = lz4_decompress<pair<string, string>>(lz4_compressed, lz4_srcSize, original.size());
    for (const auto& val : lz4_decompressed) {
        std::cout << val.first << " " << val.second << " " << std::endl;
    }
    cout << endl;

    // snappy compression
    int snappy_srcSize = 0;
    vector<char> snappy_compressed = snappy_compress<pair<string, string>>(original);
    vector<pair<string, string>> snappy_decompressed = snappy_decompress<pair<string, string>>(snappy_compressed, snappy_srcSize, original.size());
    for (const auto& val : snappy_decompressed) {
        std::cout << val.first << " " << val.second << " " << std::endl;
    }
    cout << endl;

    // fastpfor compression
    int fastpfor_srcSize = 24;  // should be multiple of 4
    vector<pair<uint32_t, uint32_t>> fastpfor_compressed = fastpfor_compress<pair<string, string>>(original, fastpfor_srcSize);
    vector<pair<string, string>> fastpfor_decompressed = fastpfor_decompress<pair<string, string>>(fastpfor_compressed, fastpfor_srcSize);
    for (const auto& val : fastpfor_decompressed) {
        std::cout << val.first << " " << val.second << " " << std::endl;
    }
    cout << endl;

    ////////////////////////////////////////////////////////////////////

    return 0;
}