#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <set>
#include <cmath>
#include <unordered_map>

#include <All.hh>

static std::vector<std::shared_ptr<Course::Algorithm>> algorithms = {
    std::make_shared<Course::NoneAlgorithm>(),
    std::make_shared<Course::RunLengthAlgorithm>(),
};

static std::string data = R"(aaaabbbbccccddddeeeeffff                                      hello world)";

static void dumpraw(const std::string &data)
{
    // Hexdump the data with ascii representation
    for (size_t i = 0; i < data.size(); i += 16)
    {
        std::cout << std::hex << std::setw(8) << std::setfill('0') << i << "  ";
        for (size_t j = 0; j < 16; j++)
        {
            if (i + j < data.size())
                std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)(unsigned char)data[i + j] << " ";
            else
                std::cout << "   ";
        }
        std::cout << " ";
        for (size_t j = 0; j < 16; j++)
        {
            if (i + j < data.size())
            {
                if (std::isprint(data[i + j]))
                    std::cout << data[i + j];
                else
                    std::cout << ".";
            }
        }
        std::cout << std::endl;
    }

    std::cout << std::dec;

    std::cout << "RawHex: ";
    for (const auto &c : data)
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)(unsigned char)c;

    std::cout << std::endl;
}

struct Stat
{
    const char *statname;
    double value;

    bool operator<(const Stat &other) const
    {
        return statname < other.statname;
    }
};

static double GetEntropy(const std::string &data)
{
    // calculate Shannon entropy
    std::unordered_map<uint8_t, int> freqs;

    for (const auto &c : data)
        freqs[c]++;
    
    double entropy = 0.0;
    for (const auto &f : freqs)
    {
        double p = (double)f.second / (double)data.size();
        entropy -= p * log2(p);
    }

    return entropy;
}

static std::set<Stat> getAlgorithmStats(const std::string &input, const std::string &comp)
{
    return {
        {"Compressed Size", (double)comp.size()},
        {"Original Size", (double)input.size()},
        {"Compression Ratio", (double)input.size() / (double)comp.size()},
        {"Space Savings", 1.0 - ((double)comp.size() / (double)input.size())},
        {"Message Entropy", GetEntropy(input)},
        {"Compressed Entropy", GetEntropy(comp)},
        {"Entropy Difference", GetEntropy(comp) - GetEntropy(input)},
    };
}

static std::set<Stat> do_algorithm(const Course::Algorithm &alg)
{
    std::stringstream input(data);
    std::stringstream compressed;
    std::stringstream decompressed;

    auto compressor = alg.getCompressor(input, compressed);
    compressor->compress();

    auto decompressor = alg.getDecompressor(compressed, decompressed);
    decompressor->decompress();

    // Verify that the decompressed data is the same as the original data
    if (data != decompressed.str())
    {
        std::cerr << "Decompressed data does not match original data" << std::endl;
        std::cerr << "Original data:" << std::endl;
        dumpraw(data);
        std::cerr << "Compressed data:" << std::endl;
        dumpraw(compressed.str());
        std::cerr << "Decompressed data:" << std::endl;
        dumpraw(decompressed.str());

        throw std::runtime_error("Decompressed data does not match original data");
    }

    std::cout << "Compressed data:" << std::endl;
    dumpraw(compressed.str());

    return getAlgorithmStats(data, compressed.str());
}

static void print_summary(const std::vector<std::pair<const char *, std::set<Stat>>> &stats)
{
    std::cout << "==========================================" << std::endl;
    std::cout << "Summary" << std::endl;
    std::cout << "==========================================" << std::endl;

    const char *best = stats[0].first;
    double best_savings = stats[0].second.find({"Space Savings", 0})->value;

    for (const auto &stat : stats)
    {
        std::cout << "Algorithm: " << stat.first << std::endl;
        for (const auto &s : stat.second)
            std::cout << "  " << s.statname << ": " << s.value << std::endl;

        if (stat.second.find({"Space Savings", 0})->value > best_savings)
        {
            best_savings = stat.second.find({"Space Savings", 0})->value;
            best = stat.first;
        }
    }

    std::cout << "==========================================" << std::endl;
    std::cout << "Best algorithm: " << best << std::endl;

    std::cout << "==========================================" << std::endl;
}

int main()
{
    std::vector<std::pair<const char *, std::set<Stat>>> stats;

    for (const auto &alg : algorithms)
        stats.push_back(std::make_pair(alg->name(), do_algorithm(*alg)));

    print_summary(stats);

    return 0;
}