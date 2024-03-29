#include <Shards/ShardsFixedRate.h>
#include <cassert>

ShardsFixedRate::ShardsFixedRate(uint64_t T, uint64_t P, uint64_t bucketSize)
    : kT{T}, kP{P}, kR{static_cast<double>(T) / P}, kBucketSize{bucketSize}
{
    assert(bucketSize > 0);
}

ShardsFixedRate::ShardsFixedRate(double R, uint64_t bucketSize)
    : kR{R}, kP{1 << 24}, kT{static_cast<uint64_t>(kR * kP)}, kBucketSize{bucketSize}
{
    assert(bucketSize > 0);
}

uint64_t ShardsFixedRate::getDistance(std::string const &key)
{
    uint64_t distance = 0;
    auto const [pair, inserted] = m_timePerObject.emplace(key, m_objectCounter);
    if (!inserted)
    {
        auto &time = pair->second;
        distance = m_distanceTree.greater_or_equal_to(time);
        m_distanceTree.erase(time);
        time = m_objectCounter;
    }
    m_distanceTree.insert(m_objectCounter);
    return distance;
}

void ShardsFixedRate::updateHistogram(uint64_t bucket)
{
    auto const [pair, inserted] = m_distanceHistogram.emplace(bucket, 1);
    if (!inserted)
    {
        pair->second++;
    }
}

void ShardsFixedRate::feed(std::string const &key)
{
    if (Shards::hash(key) % kP < kT)
    {
        m_objectCounter++;
        uint64_t const distance = getDistance(key) / kR;
        updateHistogram(distance == 0 ? 0 : ((distance - 1) / kBucketSize) * kBucketSize + kBucketSize);
    }
}

std::map<uint64_t, double> ShardsFixedRate::mrc()
{
    std::map<uint64_t, double> mrc{};
    if (!m_distanceHistogram.empty())
    {
        uint64_t sum{0};
        for (auto const &[bucket, frequency] : m_distanceHistogram)
        {
            mrc.emplace(bucket, frequency);
            sum += frequency;
        }
        mrc.erase(mrc.begin());
        uint64_t accum{0};
        for (auto &[_, missRate] : mrc)
        {
            accum += static_cast<uint64_t>(missRate);
            missRate = 1.0 - static_cast<double>(accum) / sum;
        }
    }
    return mrc;
}

void ShardsFixedRate::clear()
{
    m_distanceHistogram.clear();
    m_distanceTree = {};
    m_objectCounter = 0;
    m_timePerObject.clear();
}