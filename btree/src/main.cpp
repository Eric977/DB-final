#include <iostream>
#include <vector>
#include <lz4.h>
#include "../include/btree_map.hpp"
using namespace std;


int main() {
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

    return 0;
}
