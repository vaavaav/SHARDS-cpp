#include <shards/shards.hpp>
#include <murmurhash3/MurmurHash3.h>
#include <vector>
#include <algorithm>

class FixedRateShards : public Shards<uint32_t>
{

    inline void updateDistTable(uint32_t bucket)
    {
        if (distance_histogram.find(bucket) == distance_histogram.end())
        {
            distance_histogram[bucket] = 1;
        }
        else
        {
            distance_histogram[bucket]++;
        }
    }

public:
    FixedRateShards(uint32_t const T, uint32_t const P, uint32_t bucket_size) : Shards(T, P, bucket_size){};
    void feedKey(std::string key, int size) override final
    {
        total_objects++;

        uint64_t hash[2];
        MurmurHash3_x86_128(key.data(), size, 0, hash);
        uint64_t T_i{hash[1] & (P - 1)};

        if (T_i < T)
        {
            num_obj++;
            auto r = calcReuseDist(key);
            auto reuse_dist = static_cast<uint32_t>(r * (T / static_cast<double>(P)));
            std::cout << "reuse_dist=" << r << std::endl;
            auto bucket = reuse_dist == 0 ? 0 : ((reuse_dist - 1) / bucket_size) * bucket_size + bucket_size;
            updateDistTable(bucket);
        }
    }

    std::unordered_map<uint32_t, double> mrc() override final
    {
        std::unordered_map<uint32_t, double> mrc{};
        if (distance_histogram.size() == 0)
        {
            return mrc;
        }
        std::vector<uint32_t> buckets;
        for (auto const &bucket : distance_histogram)
        {
            buckets.push_back(bucket.first);
        }
        std::sort(buckets.begin(), buckets.end());

        uint32_t sum{0};
        for (uint32_t i = 1; i < buckets.size(); i++)
        {
            sum += distance_histogram[buckets[i]];
            mrc[buckets[i]] = sum;
        }
        sum += distance_histogram[buckets[0]];
        for (auto const &bucket : buckets)
        {
            mrc[bucket] = 1.0 - (mrc[bucket] / sum);
        }

        return mrc;
    }
};