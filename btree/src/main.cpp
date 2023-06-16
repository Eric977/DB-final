#include <sys/time.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <vector>
#include <thread>
#include <fstream>

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

    return 0;
}