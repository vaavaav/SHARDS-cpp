#pragma once
#include <cstdint>
#include <string>
#include <map>

class Shards
{
protected:
    static uint64_t hash(std::string const &key);

public:
    virtual void feed(std::string const &key) = 0;
    virtual std::map<uint32_t, double> mrc() = 0;
    virtual void clear() = 0;

    static Shards *fixedRate(uint64_t T, uint64_t P, uint32_t bucket_size = 1);
    static Shards *fixedRate(double R, uint32_t bucket_size = 1);
    static Shards *fixedSize(uint64_t T0, uint64_t P, uint32_t SMax, uint32_t bucketSize = 1);
    static Shards *fixedSize(double R0, uint32_t SMax, uint32_t bucketSize = 1);
};
