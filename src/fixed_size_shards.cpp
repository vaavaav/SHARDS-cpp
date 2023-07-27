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
            x.first->second.frequency = static_cast<uint32_t>(x.first->second.frequency * static_cast<double>(T) / x.first->second.T + 1);
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
        if (reuse_dist > 0)
        {
            updateDistTable(static_cast<uint32_t>(1 + reuse_dist * static_cast<double>(P) / T / bucket_size) * bucket_size);
        }
        if (auto x = set_table.emplace(T_i, std::unordered_set{key}); x.second || (!x.second && x.first->second.insert(key).second))
        {
            set_tree.insert(T_i);
            if (++set_size > S_max)
            {
                // Eviction
                auto T_max = set_tree.unsafe_max();

                for (auto const &k : set_table[T_max])
                {
                    distance_tree.remove(time_per_object[k]);
                    time_per_object.erase(k);
                    set_size--;
                }
                set_table.erase(T_max);
                set_tree.remove(T_max);
                T = T_max;
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
    for (uint32_t i = 0; i < buckets.size(); i++)
    {
        auto f = distance_histogram[buckets[i]];
        sum += (f.T == T) ? f.frequency : static_cast<uint32_t>(f.frequency * (static_cast<double>(T) / f.T));
        mrc[buckets[i]] = sum;
    }
    for (auto const &bucket : buckets)
    {
        mrc[bucket] = 1.0 - (mrc[bucket] / sum);
    }

    return mrc;
}