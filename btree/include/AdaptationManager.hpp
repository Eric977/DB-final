#include <atomic>
#include <unordered_map>
#include <utility>
#include <cstddef>
#include <thread>
#include <bitset>
#include <queue>
#include <vector>

using namespace std;
#define HISTORYSIZE 1
template <class Index, typename Identifier, typename Context>
class AdaptationManager {
public:
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
        bitset<HISTORYSIZE> last_classifications;
        int last_epoch;
    };

    explicit AdaptationManager(Index *index)
        : index_(index), current_epoch_(0) {
//             template <class Index, typename Identifier, typename Context>
// std::atomic<size_t> AdaptationManager<Index, Identifier, Context>::global_skip_length_{10};
            global_skip_length_ = {2};

        }

    bool IsSample() {
        // std::cout << skip_length << "\n";
        if (--skip_length == 0)
        {
            skip_length = global_skip_length_.load(); // synchronized
            return true;
        }
        return false;
    }

    void Track(const Identifier &id, AccessType type);

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

private:
    void Classify(); // Implement your classification logic here...
    void Adapt() {
        // Implement your adaptation logic here...
    }
    atomic<size_t> global_skip_length_; // Adaptive parameter
    static thread_local size_t skip_length;
    atomic<size_t> global_sample_size_; // Adaptive parameter
    unordered_map<Identifier, pair<AccessStats, Context>> samples_; // Todo: change to hashmap
    // BloomFilter<Identifier> filter_;
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

    size_t k = 20; // Set the value for k
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
        samples_[pq.top().first].last_classifications = true;
        pq.pop();
    }
}
template <class Index, typename Identifier, typename Context>
void AdaptationManager<Index, Identifier, Context>::Track(const Identifier &id, AccessType type) {
    switch (type) {
    case READ:
        samples_[id].first.reads++;
        break;
    case WRITE:
        samples_[id].first.writes++;
        break;
    default:
        break;
    }
    samples_[id].first.last_epoch = current_epoch_;
};

// 74, change the operator's direction ('<' => '>)
// 94, fist => first
// 100, sample.second.last_classifications => sample.second.first.last_classifications

// 17, WRITE, unnecessary? (WRITE or INSERT)


// std::atomic<size_t> AdaptationManager<Index, Identifier, Context>::global_skip_length_{10};