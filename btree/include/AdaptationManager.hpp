#include <atomic>
#include <unordered_map>
#include <utility>
#include <cstddef>
#include <thread>
#include <bitset>
#include <queue>
#include <vector>
#include <chrono>
#include "btree_hybrid.hpp"

#define SAMPLESIZE 500

#define SKIPLENGTH 10
#define K 200
#define HotRatio 0.3




using namespace std;
#define HISTORYSIZE 1
template <class Index, typename Identifier, typename Context>
class AdaptationManager {
public:
    friend class Btree;
    enum AccessType {
        READ,
        WRITE,
        INSERT,
        UPDATE,
        DELETE
    };
    struct AccessStats {
        size_t reads;
        size_t writes;
        // bitset<HISTORYSIZE> last_classifications;
        bool last_classifications;
        int last_epoch;
    };

    explicit AdaptationManager(Index *index)
        : index_(index), current_epoch_(0) {
//             template <class Index, typename Identifier, typename Context>
// std::atomic<size_t> AdaptationManager<Index, Identifier, Context>::global_skip_length_{10};
            global_skip_length_ = {SKIPLENGTH};
            global_sample_size_ = {SAMPLESIZE};
        }

    bool IsSample() {
        if (--skip_length == 0)
        {
            skip_length = global_skip_length_.load(); // synchronized
            return true;
        }
        return false;
    }

    void Track(const Identifier &id, AccessType type, Context& context, unsigned int slot);
    void UpdateContext(const Identifier &id, Context &context) {
        // Implement your context updating logic here...
    }
    void print(){
        cout << "print sample\n";
        for (auto &sample : samples_){
            if (sample.second.first.last_epoch != current_epoch_)
                continue;
            cout << sample.first << "\n";
            cout << sample.second.first.reads << " " << sample.second.first.writes << "\n";
        }
    }

    void ClearSample(const Identifier &id){
        samples_.erase(id);
    }
    

private:
    void Classify(); 
    void Adapt();    
    atomic<size_t> global_skip_length_; // Adaptive parameter
    static thread_local size_t skip_length;
    static thread_local size_t sample_size;
    atomic<size_t> global_sample_size_; // Adaptive parameter
    unordered_map<Identifier, pair<AccessStats, pair<Context,unsigned int>>> samples_; // Todo: change to hashmap
    // BloomFilter<Identifier> filter_; //Todo: add thie
    Index *index_;
    int current_epoch_;
};

// In source file
// template <class Index, typename Identifier, typename Context>
// thread_local size_t AdaptationManager<Index, Identifier, Context>::skip_length = 1;

template <class Index, typename Identifier, typename Context>
void AdaptationManager<Index, Identifier, Context>::Classify() {
    // Custom comparator for the priority queue
    auto comp = [](const pair<Identifier, AccessStats> &a, const pair<Identifier, AccessStats> &b) {
        return a.second.reads + a.second.writes > b.second.reads + b.second.writes;
    };

    // size_t k = K; // Set the value for k
    size_t k = samples_.size() * HotRatio;
    priority_queue<pair<Identifier, AccessStats>, vector<pair<Identifier, AccessStats>>, decltype(comp)> pq(comp);

    for (auto &sample : samples_) {
        if (sample.second.first.last_epoch != current_epoch_) {
            // The sample was not accessed in the last epoch, classify as cold
            sample.second.first.last_classifications = false;
            continue;
        }

        if (pq.size() < k) {
            pq.push(make_pair(sample.first, sample.second.first));
        }
        else {
            if (sample.second.first.reads + sample.second.first.writes > pq.top().second.reads + pq.top().second.writes) {
                // The sample was accessed more frequently than the least frequently accessed sample in the queue,
                // classify the least frequently accessed sample as cold and replace it with the current sample
                samples_[pq.top().first].first.last_classifications = false;
                pq.pop();
                pq.push(make_pair(sample.first, sample.second.first));
            }
            else {
                // The sample was accessed less frequently than all samples in the queue, classify as cold
                sample.second.first.last_classifications = false;
            }
        }
    }

    // All samples remaining in the queue are classified as hot
    while (!pq.empty()) {
        samples_[pq.top().first].first.last_classifications = true;
        pq.pop();
    }
}
template <class Index, typename Identifier, typename Context>
void AdaptationManager<Index, Identifier, Context>::Track(const Identifier &id, AccessType type, Context& context, unsigned int slot) {
    auto& sample = samples_[id];
    
    sample.second = make_pair(context, slot);
    switch (type) {
    case READ:
        sample.first.reads++;
        break;
    case WRITE:
        sample.first.writes++;
        break;
    default:
        break;
    }

    sample.first.last_epoch = current_epoch_;
    sample_size++;

    if (sample_size >= global_sample_size_) {
        sample_size = 0;
        Classify();
        sample.first.last_classifications = true;
        Adapt();
    }
}

template <class Index, typename Identifier, typename Context>
void AdaptationManager<Index, Identifier, Context>::Adapt() {
    for (auto &sample : samples_){
        // Cold Node
        if (!sample.second.first.last_classifications){
            index_->Encode(sample.first, Index::EncodingSchema::packed, sample.second.second.first, sample.second.second.second);
        }
        else{
            index_->Encode(sample.first, Index::EncodingSchema::expand, sample.second.second.first, sample.second.second.second);
        }
    }

    samples_.clear();

    return;
}

// 74, change the operator's direction ('<' => '>)
// 94, fist => first
// 100, sample.second.last_classifications => sample.second.first.last_classifications

// 17, WRITE, unnecessary? (WRITE or INSERT)


// std::atomic<size_t> AdaptationManager<Index, Identifier, Context>::global_skip_length_{10};
