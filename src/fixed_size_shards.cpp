#include <shards/shards.hpp>
#include <MurmurHash3.h>

uint32_t FixedSizeShards::calcReuseDist(std::string key)
{
    uint32_t reuse_dist = 0;
    auto x = time_per_object.emplace(key, num_obj);
    if (!x.second)
    {
        reuse_dist = distance_tree.calc_distance(x.first->second);
        distance_tree.remove(x.first->second);
        x.first->second = num_obj;
    }
    distance_tree.insert(num_obj);
    return reuse_dist;
}

void FixedSizeShards::updateDistTable(uint32_t bucket)
{
    auto x = distance_histogram.emplace(bucket, FrequencyType{.frequency = 1, .T = T});
    if (!x.second)
    {
        if (x.first->second.T != T)
        {
            x.first->second.frequency *= static_cast<double>(T) / x.first->second.T;
            x.first->second.T = T;
        }
        x.first->second.frequency++;
    }
}

void FixedSizeShards::feedKey(std::string key, int size)
{
    total_objects++;

    uint64_t hash[2];
    MurmurHash3_x86_128(key.data(), size, 0, hash);
    uint64_t T_i{hash[1] % P};

    if (T_i < T)
    {
        num_obj++;
        auto reuse_dist = calcReuseDist(key);
        auto bucket = reuse_dist == 0 ? 0 : static_cast<uint32_t>(reuse_dist * static_cast<double>(P) / T / bucket_size) * bucket_size + bucket_size;
        updateDistTable(bucket);

        // Insert <key, T_i> into Set s
        set_tree.insert(T_i);
        // Lookup the list associated with the value T_i
        if (auto x = set_table.emplace(set_tree.root, std::unordered_set{key}); x.second || (!x.second && x.first->second.insert(key).second))
        {
            set_size++;
            if (set_size > Smax)
            {
                // Eviction
                // auto evict_tree = set_tree.find_rank(set_tree.root->size - 1);
                // evict_tree = SplayTree<uint32_t>::splay(evict_tree->key, evict_tree);
                auto evict_tree = set_tree.find_max();
                auto eviction_key = evict_tree->key;

                for (auto const &key : set_table[evict_tree])
                {
                    distance_tree.remove(time_per_object[key]);
                    time_per_object.erase(key);
                    set_size--;
                }
                set_table.erase(evict_tree);
                set_tree.remove(eviction_key);
                T = eviction_key;
            }
        }
    }
}

std::unordered_map<uint32_t, double> FixedSizeShards::mrc()
{
    if (distance_histogram.size() == 0)
    {
        return {};
    }
    std::unordered_map<uint32_t, double> mrc{};
    std::vector<uint32_t> buckets;
    buckets.reserve(distance_histogram.size());
    for (auto const &bucket : distance_histogram)
    {
        buckets.push_back(bucket.first);
    }
    std::sort(buckets.begin(), buckets.end());

    uint32_t sum{0};
    for (uint32_t i = 1; i < buckets.size(); i++)
    {
        auto f = distance_histogram[buckets[i]];
        sum += (f.T == T) ? f.frequency : static_cast<uint32_t>(f.frequency * static_cast<double>(T) / f.T);
        mrc[buckets[i]] = sum;
    }
    {
        auto f = distance_histogram[buckets[0]];
        sum += (f.T == T) ? f.frequency : static_cast<uint32_t>(f.frequency * static_cast<double>(T) / f.T);
        mrc[buckets[0]] = sum;
    }
    for (auto const &bucket : buckets)
    {
        mrc[bucket] = 1.0 - (mrc[bucket] / sum);
    }

    return mrc;
}