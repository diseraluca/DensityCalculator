// Copyright 2019 Luca Di Sera
//		Contact: disera.luca@gmail.com
//				 https://github.com/diseraluca
//				 https://www.linkedin.com/in/luca-di-sera-200023167
//
// This code is licensed under the MIT License. 
// More informations can be found in the LICENSE file in the root folder of this repository

#pragma once

#include "executionclock.hpp"
#include "circularbuffer.hpp"

#include <functional>
#include <mutex>

namespace densitycalculator {
    namespace details {
        template<typename T, typename timepoint = utils::execution_clock::time_point>
        struct Measurement {
            public:
                using value_type = T;

            public:
                value_type value;
                timepoint time;

            public:
                operator value_type() const {
                    return value;
                }
        };

        using position_measurement = Measurement<int>;
        using density_measurement  = Measurement<int>;
    }

    template<std::size_t positionbuffer, std::size_t densitybuffer>
    class DensityMachine {
        public:
            using position_type = details::position_measurement;
            using density_type  = details::density_measurement;

            template<typename T, std::size_t size>
            using container_type = CircularBuffer<T, size>;

            struct position {};
            struct density  {};

        public:
            constexpr DensityMachine() = default;

        public:
            constexpr void registerMeasurement(position_type&& measurement, position);
            constexpr void registerMeasurement(density_type&& measurement, density);

            template<typename Examiner, typename... Args>
            constexpr void examine(Examiner&& f, Args... args);

        private:
            container_type<position_type, positionbuffer> positionSamples;
            container_type<density_type, densitybuffer> densitySamples;

            std::mutex positionLock;
            std::mutex densityLock;
    };
}

#include "implementation/densitymachine.inl"