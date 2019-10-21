// Copyright 2019 Luca Di Sera
//		Contact: disera.luca@gmail.com
//				 https://github.com/diseraluca
//				 https://www.linkedin.com/in/luca-di-sera-200023167
//
// This code is licensed under the MIT License. 
// More informations can be found in the LICENSE file in the root folder of this repository

#pragma once

#include <chrono>
#include <type_traits>

namespace densitycalculator {
    namespace utils {
        class execution_clock {
            public:
                using clock_type = std::conditional_t<std::chrono::high_resolution_clock::is_steady, std::chrono::high_resolution_clock, std::chrono::steady_clock>;

                using rep        = clock_type::rep;
                using period     = clock_type::period;
                using duration   = clock_type::duration;
                using time_point = clock_type::time_point;
                
                inline static constexpr bool is_steady = clock_type::is_steady;

                static time_point now() noexcept(noexcept(clock_type::now()));
        };

        static execution_clock::clock_type::time_point initial_time = execution_clock::clock_type::now();
    }
}

densitycalculator::utils::execution_clock::time_point densitycalculator::utils::execution_clock::now() noexcept(noexcept(clock_type::now())) {
    return time_point{clock_type::now() - initial_time};
}