// Copyright 2019 Luca Di Sera
//		Contact: disera.luca@gmail.com
//				 https://github.com/diseraluca
//				 https://www.linkedin.com/in/luca-di-sera-200023167
//
// This code is licensed under the MIT License. 
// More informations can be found in the LICENSE file in the root folder of this repository

#include "frequenciedthread.hpp"
#include "executionclock.hpp"
#include "densitymachine.hpp"

#include <iostream>
#include <random>
#include <execution>
#include <algorithm>
#include <functional>
#include <type_traits>

using namespace densitycalculator;
using namespace std::chrono;

static constexpr std::size_t maximumExaminedSeconds = 5;

static constexpr std::size_t positionFrequency = 1750;
static constexpr std::size_t densityFrequency = 500;

static constexpr std::size_t positionVariance = 150;
static constexpr std::size_t densityVariance  = 100;

static constexpr std::size_t maximumPositionFrequency = positionFrequency + positionVariance;
static constexpr std::size_t maximumDensityFrequency = densityFrequency + densityVariance;

static constexpr std::size_t positionBuffer = maximumPositionFrequency * maximumExaminedSeconds;
static constexpr std::size_t densityBuffer = maximumDensityFrequency * maximumExaminedSeconds;

static constexpr std::size_t examinerFrequency = 5;
static constexpr std::size_t examinerVariance  = 3;

static constexpr auto simulationTime = 20s;

template<typename ForwardIterator>
constexpr auto average(ForwardIterator start, ForwardIterator end) -> decltype(std::reduce(std::execution::par, start, end) / std::distance(start, end)) {
    return std::reduce(std::execution::par, start, end) / std::distance(start, end);
}

template<typename Iterator>
struct is_random_access_iterator {
    static inline constexpr bool value = std::is_convertible_v<typename std::iterator_traits<Iterator>::iterator_category, std::random_access_iterator_tag>; 
};

template<typename Iterator>
static constexpr bool is_random_access_iterator_v = is_random_access_iterator<Iterator>::value;

template<typename RandomAccessIterator, typename = std::enable_if_t<is_random_access_iterator_v<RandomAccessIterator>>>
constexpr auto median(RandomAccessIterator start, RandomAccessIterator end) {
    auto size = std::distance(start, end);
    auto middleOffset = size / 2 - (1 * (size % 2 == 0));

    auto medianStart = std::next(start, middleOffset);
    auto medianEnd = std::next(end, -middleOffset);

    return average(medianStart, medianEnd);
}

int main() {
    std::random_device rd{};
    std::mt19937_64 mt{rd()};
    std::uniform_int_distribution densityDist{2, 567};

    std::random_device examinerRd{};
    std::mt19937_64 examinerMt{examinerRd()};
    std::uniform_int_distribution offsetDist{1, 5};

    DensityMachine<positionBuffer, densityBuffer> simulationMachine{};

    utils::FrequenciedThread<std::chrono::high_resolution_clock, simulationTime, positionFrequency, positionVariance> positionSensor {
        [&simulationMachine](){
            static int current_position = 1;
            simulationMachine.registerMeasurement(decltype(simulationMachine)::position_type{current_position++, utils::execution_clock::now()}, decltype(simulationMachine)::position());
        }
    };

    utils::FrequenciedThread<std::chrono::high_resolution_clock, simulationTime, densityFrequency, densityVariance> densitySensor {
        [&simulationMachine, &mt, &densityDist](){
            simulationMachine.registerMeasurement(decltype(simulationMachine)::position_type{densityDist(mt), utils::execution_clock::now()}, decltype(simulationMachine)::density());
        }
    };

    utils::FrequenciedThread<std::chrono::high_resolution_clock, simulationTime, examinerFrequency, examinerVariance> samplesExaminer {
        [&simulationMachine, &examinerMt, &offsetDist]() {
            simulationMachine.examine([&examinerMt, &offsetDist](auto positionStart, auto positionEnd, auto densityStart, auto densityEnd) {
                int min_pos = positionStart->value + offsetDist(examinerMt);
                int max_pos = min_pos + 3 * offsetDist(examinerMt);

                auto min_time_it = std::find_if(positionStart, positionEnd, [min_pos](auto& elem){ return elem.value >= min_pos; });
                auto max_time_it = std::find_if(min_time_it, positionEnd, [max_pos](auto& elem){ return elem.value > max_pos; });

                auto min_density_it = std::find_if(densityStart, densityEnd, [min_time_it](auto& elem){ return elem.time >= min_time_it->time; });
                auto max_density_it = std::find_if(min_density_it, densityEnd, [max_time_it](auto& elem){ return elem.time > max_time_it->time; });

                std::vector<int> densitySamples{};
                std::copy(std::execution::par, min_density_it, max_density_it, std::back_inserter(densitySamples));

                std::sort(std::execution::par, densitySamples.begin(), densitySamples.end());

                std::cout << "Examining the measurements between " << min_pos << " and " << max_pos << " at second " << std::chrono::time_point_cast<std::chrono::seconds>(utils::execution_clock::now()).time_since_epoch().count() << '\n';
                if (densitySamples.empty()) {
                    std::cout << "Nothing to examine";
                } else {
                    std::cout << "Minimum density: " << densitySamples.front() << '\n';
                    std::cout << "Median density: " << median(densitySamples.begin(), densitySamples.end()) << "\n";
                    std::cout << "Mean density :" << average(densitySamples.begin(), densitySamples.end());
                }

                std::cout << "\n\n";
           });
        }
    };
}