#include <shards/shards.hpp>
#include <murmurhash3/MurmurHash3.h>
#include <vector>
#include <algorithm>

struct fsfreq_t
{
    uint32_t frequency;
    uint32_t T;
};

class FixedSizeShards : public Shards<fsfreq_t>
{
    uint32_t const Smax;
    using set_tree_value_t = std::pair<std::string, uint32_t>;
    struct set_tree_cmp
    {
        bool operator()(const set_tree_value_t l, const set_tree_value_t r)
        {
            return l.second < r.second;
        }
    };
    std::priority_queue<set_tree_value_t, std::vector<set_tree_value_t>, set_tree_cmp> set_tree;

    inline void updateDistTable(uint32_t bucket)
    {
        auto x = distance_histogram.find(bucket);
        if (x == distance_histogram.end())
        {
            distance_histogram[bucket] = {.frequency = 1, .oldT = T};
        }
        else
        {
            if (x->second.T != T) 
            {
                x.second.frequency = static_cast<uint32_t>(x.second.frequency * (T / static_cast<double>(x.second.T)));
                x.second.T = T;
            }
            x.second.frequency++;
        }
    }

public:
    FixedSizeShards(uint32_t const initialT, uint32_t const P, uint32_t const Smax, uint32_t bucket_size) : Shards(initialT, P, bucket_size), Smax(Smax){};
    void feedKey(std::string key, int size) override final
    {
    }
    std::unordered_map<uint32_t, double> mrc() override final
    {
        return {};
    }
};