#pragma once
#include <Shards/Shards.h>
#include <Shards/SplayTree.h>
#include <unordered_map>

class ShardsFixedSize : public Shards
{
    struct FrequencyType
    {
        uint64_t T;
        uint64_t frequency{1};
    };
    std::unordered_map<std::string, uint64_t> m_timePerObject;
    std::unordered_map<uint32_t, FrequencyType> m_distanceHistogram;
    SplayTree<uint64_t> m_distanceTree;

    double m_R{0.001};
    uint64_t m_T{static_cast<uint64_t>(m_R * kP)};
    uint32_t m_objectCounter{0};

    std::multimap<uint64_t, std::string> m_keysPerT;

    inline uint64_t getDistance(std::string const &key);
    inline void updateHistogram(uint64_t const bucket);
    inline void setT(uint64_t newT);

public:
    uint64_t const kBucketSize{1};
    uint64_t const kP{1 << 24};
    uint64_t const kSMax{1};
    ShardsFixedSize(uint64_t T0, uint64_t P, uint64_t SMax = 1, uint64_t bucketSize = 1);
    ShardsFixedSize(double R0, uint64_t SMax = 1, uint64_t bucketSize = 1);
    void feed(std::string const &key) override final;
    std::map<uint64_t, double> mrc() override final;
    void clear() override final;
};