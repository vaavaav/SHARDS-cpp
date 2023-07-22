#include <shards/shards.hpp>
#include <shards/fixed_rate_shards.hpp>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
// write the main function
int main(int argc, char **argv)
{

    /*
        argv[1] = T
        argv[2] = P
        argv[3] = bucket size
        argv[4] = tracefile
        argv[5] = mrc file
    */

    uint32_t T = strtol(argv[1], NULL, 10);
    uint32_t P = strtol(argv[2], NULL, 10);
    uint32_t bucket_size = strtol(argv[3], NULL, 10);
    
    std::cout << "T: " << T << std::endl;
    std::cout << "P: " << P << std::endl;
    std::cout << "bucket size: " << bucket_size << std::endl;

    auto shards = std::make_shared<FixedRateShards>(T, P, bucket_size);

    std::ifstream infile(argv[4]);
    std::string line, word;

    clock_t start_time = clock();
    while (getline(infile, line))
    {
        std::stringstream str(line);
        std::vector<std::string> row;

        while (getline(str, word, ','))
        {
            row.push_back(word);
        }
        //std::cout << row[1] << "," << row[2] << "," << row[5] << std::endl;
        if (row[5] == "get")
        {
            shards->feedKey(row[1], strtol(row[2].c_str(), NULL, 10));
        }
    }
    std::cout << "end of file" << std::endl;

    auto mrc = shards->mrc();
    std::ofstream mrc_file(argv[5]);
    for (auto const &[key, val] : mrc)
    {
        mrc_file << key << "," << val << std::endl;
    }
    mrc_file.close();

    clock_t end_time = clock();

    return 0;
}
