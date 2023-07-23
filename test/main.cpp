#include <shards/shards.hpp>
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
        argv[6] = s_max
    */

    uint32_t T = strtol(argv[1], NULL, 10);
    uint32_t P = strtol(argv[2], NULL, 10);
    uint32_t bucket_size = strtol(argv[3], NULL, 10);
    uint32_t Smax = strtol(argv[6], NULL, 10);
    
    std::cout << "T: " << T << std::endl;
    std::cout << "P: " << P << std::endl;
    std::cout << "bucket size: " << bucket_size << std::endl;

    auto frshards = std::make_shared<FixedRateShards>((1<<24)/1000, 1<<24, bucket_size);
    auto fsshards = std::make_shared<FixedSizeShards>((1<<24)/1000, 1<<24, Smax, bucket_size);

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
        //if (row[5] == "get")
        {
            fsshards->feedKey(row[1], strtol(row[2].c_str(), NULL, 10));
            frshards->feedKey(row[1], strtol(row[2].c_str(), NULL, 10));
        }
    }
    std::cout << "end of file" << std::endl;

    auto fsmrc = fsshards->mrc();
    auto frmrc = frshards->mrc();
    std::ofstream mrc_file(argv[5]);
    for (auto const &[key, val] : fsmrc)
    {
        mrc_file << "Fixed Size" << "," << key << "," << val << std::endl;
    }
    for (auto const &[key, val] : frmrc)
    {
        mrc_file << "Fixed Rate" << "," << key << "," << val << std::endl;
    }
    mrc_file.close();

    clock_t end_time = clock();

    return 0;
}
