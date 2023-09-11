#include <Shards/Shards.h>
#include <Shards/ShardsFixedSize.h>
#include <Shards/ShardsFixedRate.h>
#include <xxhash64.hpp>

uint64_t Shards::hash(std::string const &key)
{
    return XXHash64::hash(key.data(), key.size(), 0);
}

Shards *Shards::fixedRate(uint64_t T, uint64_t P, uint32_t bucketSize)
{
    return new ShardsFixedRate(T, P, bucketSize);
}

Shards *Shards::fixedRate(double R, uint32_t bucketSize)
{
    return new ShardsFixedRate(R, bucketSize);
}

Shards *Shards::fixedSize(uint64_t T0, uint64_t P, uint32_t SMax, uint32_t bucketSize)
{
    return new ShardsFixedSize(T0, P, SMax, bucketSize);
}

Shards *Shards::fixedSize(double R0, uint32_t SMax, uint32_t bucketSize)
{
    return new ShardsFixedSize(R0, SMax, bucketSize);
}