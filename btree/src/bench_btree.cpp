#include <sys/time.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <vector>
#include <thread>
#include <fstream>
#include <unistd.h>
#include <sys/sysctl.h>
#include <sys/resource.h>
#include <mach/mach.h>



#include "../include/btree_hybrid_map.hpp"


using namespace std;


typedef tlx::btree_map<string, string, less<string> > btree_type;
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

void loadAndInsertKVpair(string file, btree_type* bt){
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
        // keys.push_back(key);
        // values.push_back(value);
        bt->insert2(key, value);
    }

}

void ReadKey(string file, btree_type* bt){
    std::ifstream inputFile(file); 
    if (!inputFile) {
        std::cerr << "Failed to open file." << std::endl;
        return;
    }
    string line;
    int i = 0;
    while (getline(inputFile, line)){
        istringstream iss(line);
        string key, value;
        iss >> key >> value;
        auto k = bt->find(key);

        if (k->first != key || k->second != value){
            cout << "key not match\n";
            cout << k->first << " " << key << "\n";
        }

        // cout << k->first << " " << k->second << "\n";
        // bt->erase_one(key);
    }
}

uint64_t mem_snapshot(){
    task_t task = mach_task_self();
    struct task_basic_info taskInfo;
    mach_msg_type_number_t infoCount = TASK_BASIC_INFO_COUNT;
    kern_return_t result;
    result = task_info(task, TASK_BASIC_INFO, (task_info_t)&taskInfo, &infoCount);
    return taskInfo.resident_size;
}

void print_stats(btree_type* bt){
    bt->get_size();
    cout << "size = " << bt->get_stats().size << std::endl;
    cout << "gapped = " << bt->get_stats().leaves << std::endl;
    cout << "packed = " << bt->get_stats().packeds << std::endl;
    cout << "succinct = " << bt->get_stats().succincts << std::endl;
    cout << "leaves = " << bt->get_stats().packeds + bt->get_stats().leaves + bt->get_stats().succincts << std::endl;

    return;
}

int main(int argc, char** argv){


    std::this_thread::sleep_for(std::chrono::seconds(2));
    // Load key value
    vector<string> keys, values;
    // Testing B tree
    btree_type *bt = new btree_type();

    cout << "Load key value pair\n";
    int mode = stoi(argv[2]);
    bt->setMode(mode);
    loadAndInsertKVpair(argv[1], bt);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    uint64_t first, migrate, last;

    first = mem_snapshot();
    // cout << "start migrate\n";
    cout << "Start Read Workload\n";
    auto start = std::chrono::high_resolution_clock::now();
    ReadKey(argv[1], bt);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Read complete in " << duration << " milliseconds." << std::endl;

    print_stats(bt);
    // cout << "Read key again\n";

    migrate = mem_snapshot();

    // bt->clear();
    btree_type *bt2 = new btree_type();
    cout << "Construct another tree\n";
    loadAndInsertKVpair(argv[1], bt2);
    // ReadKey(argv[1], bt2);



    // bt->clear();

    last = mem_snapshot();

    task_t task = mach_task_self();
    struct task_basic_info taskInfo;
    mach_msg_type_number_t infoCount = TASK_BASIC_INFO_COUNT;
    cout << "Original Memory Usage: " << first << "\n";
    cout << "Memory Usage: " << last - first << "\n";
    cout << "Saving: " << (double)(first * 2 - last) /  (first) << "\n";
    ;
    

    return 0;
}