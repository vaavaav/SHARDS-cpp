#include <shards/shards.hpp>
#include <MurmurHash3.h>

uint32_t FixedRateShards::calcReuseDist(std::string key)
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

void FixedRateShards::updateDistTable(uint32_t bucket)
{
    auto x = distance_histogram.emplace(bucket, 1);
    if (!x.second)
    {
        x.first->second++;
    }
}

void FixedRateShards::feedKey(std::string key, int size)
{
    total_objects++;

    uint64_t hash[2];
    MurmurHash3_x86_128(key.data(), size, 0, hash);
    uint64_t T_i{hash[1] % P};

    if (T_i < T)
    {
        num_obj++;
        auto bucket = static_cast<uint32_t>(calcReuseDist(key) * static_cast<double>(P) / T / bucket_size) * bucket_size;
        updateDistTable(bucket);
    }
}

std::unordered_map<uint32_t, double> FixedRateShards::mrc()
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
        mrc[buckets[i]] = sum;
        sum += distance_histogram[buckets[i]];
    }
    for (auto const &bucket : buckets)
    {
        mrc[bucket] = 1.0 - (mrc[bucket] / sum);
    }

    return mrc;
}