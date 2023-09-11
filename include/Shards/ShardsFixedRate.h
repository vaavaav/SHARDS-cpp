#pragma once
#include <Shards/Shards.h>
#include <Shards/SplayTree.h>
#include <unordered_map>

class ShardsFixedRate : public Shards
{
    std::unordered_map<std::string, uint32_t> m_timePerObject;
    std::unordered_map<uint32_t, uint32_t> m_distanceHistogram;
    SplayTree<uint32_t> m_distanceTree;

    uint32_t m_objectCounter{0};

    inline void updateHistogram(uint32_t const bucket);
    inline uint32_t getDistance(std::string const &key);

public:
    uint64_t const kP{1 << 24};
    double const kR{0.001};
    uint64_t const kT{static_cast<uint64_t>(kR * kP)};
    uint32_t const kBucketSize{1};

    ShardsFixedRate(uint64_t T, uint64_t P, uint32_t bucketSize = 1);
    ShardsFixedRate(double R, uint32_t bucketSize = 1);
    void feed(std::string const &key) override final;
    std::map<uint32_t, double> mrc() override final;
    void clear() override final;
};