#pragma once
#include <unordered_map>
#include <cstdint>
#include "splay_tree.h"
#include <string>
#include <iostream>
#include <queue>
#include <algorithm>

template<typename FrequencyType>
class Shards
{
protected:
    std::unordered_map<std::string, uint32_t> time_per_object;
    std::unordered_map<uint32_t, FrequencyType> distance_histogram;
    SplayTree<uint32_t> distance_tree;
    uint32_t const T;
    uint32_t const P;
    uint32_t bucket_size{1};
    uint32_t total_objects{0};
    uint32_t num_obj{0};

protected:
    inline uint32_t calcReuseDist(std::string key)
    {
        uint32_t reuse_dist = 0;
        if (time_per_object.find(key) == time_per_object.end())
        {
            reuse_dist = 0;
        }
        else
        {
            uint32_t timestamp = time_per_object[key];
            reuse_dist = distance_tree.calc_distance(timestamp);
            distance_tree.remove(timestamp);
        }
        distance_tree.insert(num_obj);
        //distance_tree.print();
        //std::cout << "---" << std::endl;
        time_per_object[key] = num_obj;
        return reuse_dist;
    }

public:
    Shards(uint32_t const T, uint32_t const P, uint32_t bucket_size) : T(T), P(P), bucket_size(bucket_size){};
    virtual void feedKey(std::string key, int size) = 0;
    virtual std::unordered_map<uint32_t, double> mrc() = 0;
};