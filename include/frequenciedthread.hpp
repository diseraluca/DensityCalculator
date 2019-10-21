// Copyright 2019 Luca Di Sera
//		Contact: disera.luca@gmail.com
//				 https://github.com/diseraluca
//				 https://www.linkedin.com/in/luca-di-sera-200023167
//
// This code is licensed under the MIT License. 
// More informations can be found in the LICENSE file in the root folder of this repository

#pragma once

#include <cstddef>
#include <chrono>
#include <thread>

namespace densitycalculator {
    namespace utils {
        template<typename Clock, typename Clock::duration duration, int frequency, int variation> 
        class FrequenciedThread {
            public:
                template<typename Callable, typename... Args>
                FrequenciedThread(Callable&& f, Args&&... args);

                ~FrequenciedThread();

            private:
                template<typename Callable, typename... Args>
                void execute(Callable&& f, Args&&... args);

            private:
                std::thread thread;
        };
    }
}

#include "implementation/frequenciedthead.inl"