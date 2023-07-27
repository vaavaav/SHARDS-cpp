#pragma once
#include "splay_tree.h"
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <queue>
#include <shards/shards.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Shards
{
public:
    virtual void feedKey(std::string key, int size) = 0;
    virtual std::unordered_map<uint32_t, double> mrc() = 0;
};

class FixedRateShards : public Shards
{
    std::unordered_map<std::string, uint32_t> time_per_object;
    std::unordered_map<uint32_t, uint32_t> distance_histogram;
    SplayTree<uint32_t> distance_tree;
    uint32_t const T;
    uint32_t const P;
    uint32_t bucket_size{1};
    uint32_t total_objects{0};
    uint32_t num_obj{0};

    inline void updateDistTable(uint32_t bucket);
    inline uint32_t calcReuseDist(std::string key);

public:
    FixedRateShards(uint32_t const T, uint32_t const P, uint32_t bucket_size) : T(T), P(P), bucket_size(bucket_size){};
    void feedKey(std::string key, int size) override final;
    std::unordered_map<uint32_t, double> mrc() override final;
};

class FixedSizeShards : public Shards
{
    struct FrequencyType
    {
        uint32_t frequency;
        uint32_t T;
    };
    std::unordered_map<std::string, uint32_t> time_per_object;
    std::unordered_map<uint32_t, FrequencyType> distance_histogram;
    SplayTree<uint32_t> distance_tree;
    uint32_t T;
    uint32_t const P;
    uint32_t bucket_size{1};
    uint32_t total_objects{0};
    uint32_t num_obj{0};
    // Additional structures
    std::unordered_map<uint32_t, std::unordered_set<std::string>> set_table;
    SplayTree<uint32_t> set_tree;
    uint32_t set_size{0};
    uint32_t const S_max;

    inline void updateDistTable(uint32_t bucket);
    inline uint32_t calcReuseDist(std::string key);

public:
    FixedSizeShards(uint32_t const initialT, uint32_t const P, uint32_t const S_max, uint32_t bucket_size) : T(initialT), P(P), S_max(S_max), bucket_size(bucket_size){};
    void feedKey(std::string key, int size) override final;
    std::unordered_map<uint32_t, double> mrc() override final;
};