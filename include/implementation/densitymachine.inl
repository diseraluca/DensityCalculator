// Copyright 2019 Luca Di Sera
//		Contact: disera.luca@gmail.com
//				 https://github.com/diseraluca
//				 https://www.linkedin.com/in/luca-di-sera-200023167
//
// This code is licensed under the MIT License. 
// More informations can be found in the LICENSE file in the root folder of this repository

#pragma once

#include "../densitymachine.hpp"

#include <future>

template<std::size_t positionbuffer, std::size_t densitybuffer>
constexpr void densitycalculator::DensityMachine<positionbuffer, densitybuffer>::registerMeasurement(position_type&& measurement, position) {
    std::scoped_lock lock{positionLock};
    positionSamples.push_back(std::move(measurement));
}

template<std::size_t positionbuffer, std::size_t densitybuffer>
constexpr void densitycalculator::DensityMachine<positionbuffer, densitybuffer>::registerMeasurement(density_type&& measurement, density) {
    std::scoped_lock lock{densityLock};
    densitySamples.push_back(std::move(measurement));
}

template<std::size_t positionbuffer, std::size_t densitybuffer>
template<typename Examiner, typename... Args>
constexpr void densitycalculator::DensityMachine<positionbuffer, densitybuffer>::examine(Examiner&& f, Args... args) {
    std::scoped_lock lock{positionLock, densityLock};
    return std::invoke(std::forward<Examiner>(f), positionSamples.begin(), positionSamples.end(), densitySamples.begin(), densitySamples.end(), std::forward<Args>(args)...);
}