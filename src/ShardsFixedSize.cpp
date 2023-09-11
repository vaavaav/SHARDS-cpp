#include <Shards/ShardsFixedSize.h>
#include <cassert>

ShardsFixedSize::ShardsFixedSize(uint64_t T0, uint64_t P, uint64_t SMax, uint64_t bucketSize)
    : kP{P}, kSMax{SMax}, kBucketSize{bucketSize}, m_R{static_cast<double>(T0) / P}, m_T{T0}
{
    assert(bucketSize > 0 && SMax > 0);
}

ShardsFixedSize::ShardsFixedSize(double R0, uint64_t SMax, uint64_t bucketSize)
    : kSMax{SMax}, kBucketSize{bucketSize}, kP{1 << 24}, m_R{R0}, m_T{static_cast<uint64_t>(R0 * (1 << 24))}
{
    assert(bucketSize > 0 && SMax > 0);
}

void ShardsFixedSize::setT(uint64_t newT)
{
    m_T = newT;
    m_R = static_cast<double>(m_T) / kP;
}

uint64_t ShardsFixedSize::getDistance(std::string const &key)
{
    uint64_t distance = 0;
    auto const [pair, inserted] = m_timePerObject.emplace(key, m_objectCounter);
    auto &time = pair->second;
    if (!inserted)
    {
        // if the key already existed
        distance = m_distanceTree.greater_or_equal_to(time);
        m_distanceTree.erase(time);
        time = m_objectCounter;
    } // else, the key did not exist (therefore, previous emplace successful)
    m_distanceTree.insert(m_objectCounter);
    return distance;
}

void ShardsFixedSize::updateHistogram(uint64_t const bucket)
{
    auto const [pair, inserted] = m_distanceHistogram.emplace(bucket, FrequencyType{m_T, 1});
    if (!inserted)
    {
        // if bucket already existed (therefore, emplace successful)
        auto &[T, f] = pair->second;
        if (T != m_T)
        {
            f = 2 + f * static_cast<double>(m_T) / T;
            T = m_T;
        }
        else
        {
            f++;
        }
    }
}

void ShardsFixedSize::feed(std::string const &key)
{
    uint64_t const Ti{Shards::hash(key) % kP};

    if (Ti < m_T)
    {
        m_objectCounter++;
        uint64_t const distance = getDistance(key) / m_R;
        updateHistogram(distance == 0 ? 0 : ((distance - 1) / kBucketSize) * kBucketSize + kBucketSize);

        if (distance == 0)
        { // then key is new
            m_keysPerT.emplace(Ti, key);
            if (m_keysPerT.size() > kSMax)
            {
                auto const TMax = m_keysPerT.rbegin()->first;
                auto const [start, end] = m_keysPerT.equal_range(TMax);
                for (auto keyToRemove = start; keyToRemove != end; keyToRemove++)
                {
                    m_distanceTree.erase(m_timePerObject.extract(keyToRemove->second).mapped());
                }
                m_keysPerT.erase(start, end);
                setT(TMax);
            }
        }
    }
}

std::map<uint64_t, double> ShardsFixedSize::mrc()
{
    std::map<uint64_t, double> mrc{};
    if (!m_distanceHistogram.empty())
    {
        uint64_t sum{0};
        for (auto &[bucket, ft] : m_distanceHistogram)
        {
            auto &[T, f] = ft;
            if (T != m_T)
            {
                // if T is old, update bucket
                f = 1 + f * static_cast<double>(m_T) / T;
                T = m_T;
            }
            mrc.emplace(bucket, f);
            sum += f;
        }
        mrc.erase(mrc.begin());
        uint64_t accum{0};
        for (auto &[_, missRate] : mrc)
        {
            accum += static_cast<uint64_t>(missRate);
            missRate = 1.0 - (static_cast<double>(accum) / sum);
        }
    }
    return mrc;
}

void ShardsFixedSize::clear()
{
    m_distanceHistogram.clear();
    m_distanceTree = {};
    m_objectCounter = 0;
    m_timePerObject.clear();
    m_keysPerT.clear();
}