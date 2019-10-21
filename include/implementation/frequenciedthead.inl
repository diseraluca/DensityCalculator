// Copyright 2019 Luca Di Sera
//		Contact: disera.luca@gmail.com
//				 https://github.com/diseraluca
//				 https://www.linkedin.com/in/luca-di-sera-200023167
//
// This code is licensed under the MIT License. 
// More informations can be found in the LICENSE file in the root folder of this repository

#pragma once

#include "../frequenciedthread.hpp"

#include <functional>
#include <ratio>
#include <random>

namespace densitycalculator {
    namespace utils {
        namespace details {
            inline constexpr std::chrono::duration<double> frequencyToDuration(std::size_t&& frequency) {
                return std::chrono::duration<double>{1.0/static_cast<double>(frequency)};
            }
        }
    }
}

template<typename Clock, typename Clock::duration duration, int frequency, int variation> 
template<typename Callable, typename... Args>
densitycalculator::utils::FrequenciedThread<Clock, duration, frequency, variation>::FrequenciedThread(Callable&& f, Args&&... args) 
    : thread{&FrequenciedThread<Clock, duration, frequency, variation>::execute<Callable, Args...>, this, std::forward<Callable>(f), std::forward<Args>(args)...} {}

template<typename Clock, typename Clock::duration duration, int frequency, int variation> 
densitycalculator::utils::FrequenciedThread<Clock, duration, frequency, variation>::~FrequenciedThread() {
    thread.join();
}

template<typename Clock, typename Clock::duration duration, int frequency, int variation> 
template<typename Callable, typename... Args>
void densitycalculator::utils::FrequenciedThread<Clock, duration, frequency, variation>::execute(Callable&& f, Args&&... args) {
    std::random_device rd{};
    std::mt19937_64 mt{rd()};
    std::uniform_int_distribution dist(-variation, variation);

    auto remaining_duration{duration};

    while (remaining_duration > decltype(remaining_duration)::zero()) {
        auto start = Clock::now();
        std::this_thread::sleep_for(details::frequencyToDuration(frequency + dist(mt)));
        std::invoke(std::forward<Callable>(f), std::forward<Args>(args)...);
        remaining_duration -= (Clock::now() - start);
    }
}