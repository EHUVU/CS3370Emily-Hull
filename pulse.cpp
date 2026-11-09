// pulse.cpp -- CS 3370 Assignment: Validated Config Loader
// (Given to students as-is; they write pulse_config.cpp.)
//Emily Hull
#include "pulse_config.h"
#include <exception>
#include <iostream>
#include <filesystem>
#include <vector>
#include <fstream>
#include <numeric>
#include <iterator>
#include <algorithm>


/*
    I added some extra test files here to test the invalid file check.

    Reflection:

    I think this assignment was pretty interesting and I think this was a good exercise with using vectors, file reading and algorithms.
    I learned a lot from this and read about the std::filesystem for a bit. I think I did good with using const and references on the 3 vectors that
    are used in main.
*/

namespace fs = std::filesystem; //I really don't want to write this over and over again.

/*
Computes the area of the unsmooth, negated values. This is intended to be used in the readAndApplyFilters function to calculate the
area after the pulses are filtered.
*/
long long computeArea(const std::vector<int> &negatedValues, std::size_t start, std::size_t next, int width)
{
    std::size_t end = std::min({start + static_cast<std::size_t>(width), next, negatedValues.size()});
    return std::accumulate(
        negatedValues.begin() + start,
        negatedValues.begin() + end,
        0LL);
}

/*
    Returns a vector containing the pulses that are found through the smoothedValues vector.
*/
std::vector<int> detectPulses(const std::vector<int> &smoothedValues, const int vt)
{
    std::vector<int> pulses;

    for (std::size_t i = 0; i + 2 < smoothedValues.size(); ++i)
    {
        if (smoothedValues[i + 2] - smoothedValues[i] > vt)
        {
            pulses.push_back(static_cast<int>(i));

            i += 2;

            while (i + 1 < smoothedValues.size() && smoothedValues[i + 1] >= smoothedValues[i])
            {
                ++i;
            }
        }
    }
    return pulses;
}

/*
    Smoothed using the 7 point weighted average with the weights 1, 2, 3, 3, 3, 2, 1 and divided by 15.
*/
void smoothData(const std::vector<int> &negateData, std::vector<int> &smoothData)
{
    smoothData.clear();
    if (negateData.size() < 7)
    {
        smoothData = negateData;
        return;
    }

    std::copy(
        negateData.begin(),
        negateData.begin() + 3,
        std::back_inserter(smoothData));

    for (std::size_t i = 3; i < negateData.size() - 3; ++i)
    {
        int average = (negateData[i - 3] +
                       2 * negateData[i - 2] +
                       3 * negateData[i - 1] +
                       3 * negateData[i] +
                       3 * negateData[i + 1] +
                       2 * negateData[i + 2] +
                       negateData[i + 3]) /
                      15;

        smoothData.push_back(average);
    }

    std::copy(
        negateData.end() - 3,
        negateData.end(),
        std::back_inserter(smoothData));
}

/*
    This runs through the initial vector containing all of the .dat file values and flips the sign for each value. Negative to positive and vise versa.
*/
void negateData(const std::vector<int> &storage, std::vector<int> &negatedValues)
{
    negatedValues.clear();
    std::copy(storage.begin(),
              storage.end(),
              std::back_inserter(negatedValues));
    std::transform(
        negatedValues.begin(),
        negatedValues.end(),
        negatedValues.begin(),
        std::negate<int>());
}

/*
    Loops through pulse positions and checks if the next pulse is within the delta position. Threshold is calculated but multiplying the
    peak heigh by the drop ratio and excludes the piggyback when detected.
*/
std::vector<int> piggybackFilter(const std::vector<int> &smoothedValues, const std::vector<int> pulses, const PulseParams &p){
    std::vector<int> filteredPulses;

    for(std::size_t i = 0; i < pulses.size(); ++i){

        if(i + 1 >= pulses.size()){
            filteredPulses.push_back(pulses[i]);
            continue;
        }

        std::size_t start = pulses[i];
        std::size_t next = pulses[i + 1];
        if(next - start > static_cast<std::size_t>(p.pulse_delta)){
            filteredPulses.push_back(pulses[i]);
            continue;
        }

        std::size_t end = next;
        std::size_t peak = start;
        for(std::size_t j = start + 1; j < end; ++j){
            if(smoothedValues[j] > smoothedValues[peak]){
                peak = j;
            }
        }

        double threshold = smoothedValues[peak] * p.drop_ratio;

        int below = 0;
        for(std::size_t j = peak + 1; j < end; ++j){
            if(smoothedValues[j] < threshold){
                ++below;
            }
        }

        if(below > p.below_drop_ratio){
            std::cout << "Found piggyback at " << start << std::endl;
        }else{
            filteredPulses.push_back(pulses[i]);
        }
    }
    return filteredPulses;
}

/*
    This function checks the directory and checks for .dat files. Storage is filled with the values from the .dat files, the negation happens and then the values are
    smoothed out and filtered for piggybacks. The result is printed and then it moves onto the next entry in the directory.
*/
void readAndApplyFilters(const fs::path &folder, std::vector<int> &storage, std::vector<int> &negatedValues, std::vector<int> &smoothedValues, const PulseParams &p)
{
    if (!fs::exists(folder) || !fs::is_directory(folder))
    {
        return;
    }

    for (const auto &entry : fs::directory_iterator(folder))
    {
        if (!entry.is_regular_file() || entry.path().extension() != ".dat")
        {
            continue;
        }

        if (entry.file_size() == 0)
        {
            std::cout << "Invalid file: " << entry.path() << "\n\n";
            continue;
        }

        storage.clear();

        std::cout << entry.path() << "\n";
        std::ifstream file(entry.path());
        if (!file)
        {
            std::cout << "Could not open file.\n";
            continue;
        }

        storage.assign(
            std::istream_iterator<int>(file),
            std::istream_iterator<int>());

        negateData(storage, negatedValues);

        smoothData(negatedValues, smoothedValues);

        std::vector<int> pulses = detectPulses(smoothedValues, p.vt);

        std::vector<int> filteredPulses = piggybackFilter(smoothedValues, pulses, p);

        for (std::size_t i = 0; i < filteredPulses.size(); ++i)
        {
            std::size_t start = filteredPulses[i];
            std::size_t next;
            if (i + 1 < filteredPulses.size())
            {
                next = filteredPulses[i + 1];
            }
            else
            {
                next = negatedValues.size();
            }

            long long area = computeArea(
                negatedValues,
                start,
                next,
                p.width);

            std::cout << start << " (" << area << ") " << std::endl;
        }
    }
}

/*
    Main function with code from the first part of the assignment.
*/
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <file.ini>\n";
        return 1;
    }
    try
    {
        PulseConfig config{argv[1]};
        const PulseParams &p = config.params();
        std::cout << "Loaded " << argv[1] << '\n'
                  << "vt=" << p.vt << '\n'
                  << "width=" << p.width << '\n'
                  << "pulse_delta=" << p.pulse_delta << '\n'
                  << "drop_ratio=" << p.drop_ratio << '\n'
                  << "below_drop_ratio=" << p.below_drop_ratio
                  << '\n';

        fs::path folder = ".";
        std::vector<int> storage;
        std::vector<int> negatedValues;
        std::vector<int> smoothedValues;
        readAndApplyFilters(folder, storage, negatedValues, smoothedValues, p);
    }
    catch (const ConfigFileError &e)
    { // our errors first
        std::cerr << e.what() << '\n';
        return 1;
    }
    catch (const std::exception &e)
    { // anything else last
        std::cerr << "Unexpected error: " << e.what() << '\n';
        return 2;
    }
}
