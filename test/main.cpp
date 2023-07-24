#include <shards/shards.hpp>
#include <cxxopts.hpp>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

int main(int argc, char **argv)
{

    cxxopts::Options options("shards with twitter traces", "");
    auto opts = options.add_options();
    opts("m", "Type of shards", cxxopts::value<std::string>());
    opts("t,threshold", "Threshold", cxxopts::value<uint32_t>());
    opts("p,modulus", "Modulus", cxxopts::value<uint32_t>());
    opts("b,bucket_size", "Bucket Size", cxxopts::value<uint32_t>());
    opts("f,tracefile", "Trace File", cxxopts::value<std::string>());
    opts("s", "S_max", cxxopts::value<uint32_t>());
    opts("h,help", "Print usage");

    auto result = options.parse(argc, argv);
    if (result.count("help"))
    {
        std::cout << options.help() << std::endl;
        exit(0);
    }

    std::string type = result["m"].as<std::string>();
    uint32_t T = result["t"].as<uint32_t>();
    uint32_t P = result["p"].as<uint32_t>();
    uint32_t bucket_size = result["b"].as<uint32_t>();
    volatile uint32_t S_max; 
    std::string tracefile = result["f"].as<std::string>();

    std::shared_ptr<Shards> shards;
    if (type == "fixed_size") {
        S_max = result["s"].as<uint32_t>();
        shards = std::make_shared<FixedSizeShards>(T, P, S_max, bucket_size);
    } else if(type == "fixed_rate") {
        shards = std::make_shared<FixedRateShards>(T, P, bucket_size);
    } else {
        std::cout << "Invalid type" << std::endl;
        exit(1);
    }

    std::ifstream infile(tracefile);
    std::string line, word;

    while (getline(infile, line))
    {
        std::stringstream str(line);
        std::vector<std::string> row;

        while (getline(str, word, ','))
        {
            row.push_back(word);
        }

        if (row[5] == "get")
        {
            shards->feedKey(row[1], strtol(row[2].c_str(), NULL, 10));
        }
    }

    for (auto const &[key, val] : shards->mrc())
    {
        std::cout << key << "," << val << std::endl;
    }


    return 0;
}
