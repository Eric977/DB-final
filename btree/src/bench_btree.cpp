#include <sys/time.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <vector>
#include <fstream>

#include "../include/btree_map.hpp"

using namespace std;

void loadKVpair(string file, vector<string>& keys, vector<string>& values){
    std::ifstream inputFile(file);  

    if (!inputFile) {
        std::cerr << "Failed to open file." << std::endl;
        return;
    }
    string line;
    while (getline(inputFile, line)){
        istringstream iss(line);
        string key, value;
        iss >> key >> value;
        keys.push_back(key);
        values.push_back(value);
    }
    inputFile.close();
}
int main(){
    // Load key value
    vector<string> keys, values;
    loadKVpair("../workloads/sample", keys, values);
    // Testing B tree
    typedef tlx::btree_map<string, string, less<string> > btree_type;
    btree_type *bt = new btree_type();
    for (int i = 0; i < keys.size(); i ++){
        bt->insert2(keys[i], values[i]);
    }

    cout << "size = " << bt->get_stats().size << std::endl;
    cout << "leaves = " << bt->get_stats().leaves << std::endl;
    cout << "inner_nodes = " << bt->get_stats().inner_nodes << std::endl;
    cout << "avgfill = " << bt->get_stats().avgfill_leaves() << std::endl;
    cout << "mem = " << (256 * bt->get_stats().nodes()) << std::endl;

}