
#pragma once

#include <chrono>
#include <iostream>
#include <map>
#include <string>

class StageTimer {
    using Clock = std::chrono::steady_clock;

    struct Result {
        double milliseconds = 0.0;
        std::size_t calls = 0;
    };

    inline static std::map<std::string, Result> totals{};

    std::string name;
    Clock::time_point start;

public:
    explicit StageTimer(std::string stage_name)
        : name(std::move(stage_name)), start(Clock::now()) {}

    ~StageTimer() {
        auto elapsed = std::chrono::duration<double, std::milli>(
            Clock::now() - start
        ).count();

        auto& result = totals[name];
        result.milliseconds += elapsed;
        ++result.calls;
    }

    StageTimer(const StageTimer&) = delete;
    StageTimer& operator=(const StageTimer&) = delete;

    static void print_stage_report() {
        std::cerr << "\nStage timing report:\n";

        for (const auto& [name, result] : totals) {
            std::cerr << name
                      << ": " << result.milliseconds << " ms, "
                      << result.calls << " calls\n";
        }
    }
};
