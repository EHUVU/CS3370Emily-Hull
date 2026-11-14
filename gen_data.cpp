
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
        // Edge-case test file generation mode.
    if (argc == 3 && std::string(argv[1]) == "--edge") {
        fs::path out_dir = argv[2];
        fs::create_directories(out_dir);

        // 1. Empty file.
        std::ofstream(out_dir / "empty.dat");

        // 2. Exactly 5 values.
        {
            std::ofstream out(out_dir / "five.dat");
            out << "10\n-20\n30\n-40\n50\n";
        }

        // 3. Exactly 7 values.
        {
            std::ofstream out(out_dir / "seven.dat");
            out << "10\n-20\n30\n-40\n50\n-60\n70\n";
        }

        // 4. Invalid non-integer token.
        {
            std::ofstream out(out_dir / "invalid.dat");
            out << "10\n20\nabc\n30\n";
        }

        // 5. Noise-only file, no pulses.
        {
            std::ofstream out(out_dir / "noise_only.dat");
            std::mt19937 edge_rng(12345);
            std::normal_distribution<double> noise(0.0, 20.0);

            for (int i = 0; i < 100; ++i) {
                out << static_cast<int>(std::llround(noise(edge_rng)))
                    << '\n';
            }
        }

        std::cout << "Generated edge-case files in "
                  << out_dir << '\n';
        return 0;
    }
    
    if (argc != 5) {
        std::cerr << "Usage: gen_data <out_dir> <num_files> "
                     "<samples_per_file> <seed>\n";
        return 1;
    }

    try {
        fs::path out_dir = argv[1];
        int num_files = std::stoi(argv[2]);
        int samples_per_file = std::stoi(argv[3]);
        unsigned int seed = static_cast<unsigned int>(
            std::stoul(argv[4])
        );

        if (num_files <= 0 || samples_per_file <= 0) {
            throw std::invalid_argument(
                "Number of files and samples must be positive."
            );
        }

        fs::create_directories(out_dir);

        std::mt19937 rng(seed);

        std::normal_distribution<double> noise(0.0, 20.0);
        std::uniform_int_distribution<int> amplitude(500, 5000);
        std::uniform_int_distribution<int> rise_time(5, 10);
        std::uniform_int_distribution<int> decay_time(30, 80);
        std::uniform_int_distribution<int> pulse_count(3, 8);
        std::uniform_int_distribution<int> gap(5, 15);
        std::uniform_int_distribution<int> spacing(100, 250);

        std::ofstream truth(out_dir / "truth.txt");

        if (!truth) {
            throw std::runtime_error("Could not create truth.txt");
        }

        for (int file_num = 0; file_num < num_files; ++file_num) {
            std::vector<double> samples(samples_per_file);

            // Generate baseline Gaussian noise.
            for (double& sample : samples) {
                sample = noise(rng);
            }

            std::vector<int> starts;

            int position = 20;
            int number_of_pulses = pulse_count(rng);

            for (int p = 0; p < number_of_pulses; ++p) {
                int amp = amplitude(rng);
                int rise = rise_time(rng);
                int decay = decay_time(rng);

                if (position + rise + decay >= samples_per_file) {
                    break;
                }

                starts.push_back(position);

                // Negative amplitude because hardware stores
                // the inverted signal.
                double peak = -static_cast<double>(amp);

                // Rising edge: gradually move downward.
                for (int i = 0; i < rise; ++i) {
                    double fraction =
                        static_cast<double>(i + 1) / rise;

                    samples[position + i] += peak * fraction;
                }

                // Slower exponential decay back toward baseline.
                for (int i = 0; i < decay; ++i) {
                    double fraction =
                        std::exp(-4.0 * i / decay);

                    samples[position + rise + i] +=
                        peak * fraction;
                }

                // Sometimes place another pulse close by.
                if (p + 1 < number_of_pulses &&
                    std::uniform_int_distribution<int>(0, 1)(rng) == 0) {
                    position += rise + gap(rng);
                } else {
                    position += rise + decay + spacing(rng);
                }
            }

            fs::path filename =
                out_dir / ("pulse_" + std::to_string(file_num + 1) + ".dat");

            std::ofstream output(filename);

            if (!output) {
                throw std::runtime_error(
                    "Could not create " + filename.string()
                );
            }

            // Write integer samples separated by whitespace.
            for (double sample : samples) {
                output << static_cast<long long>(std::llround(sample))
                       << '\n';
            }

            // Record the ground-truth pulse starting positions.
            truth << filename.filename().string() << ":";

            for (int start : starts) {
                truth << " " << start;
            }

            truth << '\n';

            std::cout << "Generated " << filename << '\n';
        }

        std::cout << "Truth file written to "
                  << (out_dir / "truth.txt") << '\n';

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}

//g++ -std=c++23 -Wall -Wextra -pedantic gen_data.cpp -o gen_data