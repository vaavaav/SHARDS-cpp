#pragma once
#include <Shards/Shards.h>
#include <Shards/SplayTree.h>
#include <unordered_map>

class ShardsFixedRate : public Shards
{
    std::unordered_map<std::string, uint64_t> m_timePerObject;
    std::unordered_map<uint64_t, uint64_t> m_distanceHistogram;
    SplayTree<uint64_t> m_distanceTree;

    uint64_t m_objectCounter{0};

    inline void updateHistogram(uint64_t const bucket);
    inline uint64_t getDistance(std::string const &key);

public:
    uint64_t const kP{1 << 24};
    double const kR{0.001};
    uint64_t const kT{static_cast<uint64_t>(kR * kP)};
    uint64_t const kBucketSize{1};

    ShardsFixedRate(uint64_t T, uint64_t P, uint64_t bucketSize = 1);
    ShardsFixedRate(double R, uint64_t bucketSize = 1);
    void feed(std::string const &key) override final;
    std::map<uint64_t, double> mrc() override final;
    void clear() override final;
};