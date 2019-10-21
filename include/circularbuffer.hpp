// Copyright 2019 Luca Di Sera
//		Contact: disera.luca@gmail.com
//				 https://github.com/diseraluca
//				 https://www.linkedin.com/in/luca-di-sera-200023167
//
// This code is licensed under the MIT License. 
// More informations can be found in the LICENSE file in the root folder of this repository

#pragma once

#include <array>

namespace densitycalculator {
    template<typename T, std::size_t size>
    class CircularBuffer;

    namespace details {
        template<typename T, std::size_t size>
        class CircularIterator;
        
        template<typename T_, std::size_t size_>
        constexpr bool operator==(const CircularIterator<T_, size_>& left, const CircularIterator<T_, size_>& right) noexcept;

        template<typename T_, std::size_t size_>
        constexpr bool operator!=(const CircularIterator<T_, size_>& left, const CircularIterator<T_, size_>& right) noexcept;

        template<typename T, std::size_t size>
        class CircularIterator {
            public:
                using iterator_type = typename CircularBuffer<T, size>::container_type::const_iterator;

                using value_type = typename std::iterator_traits<iterator_type>::value_type;
                using difference_type = typename std::iterator_traits<iterator_type>::difference_type;
                using reference = typename std::iterator_traits<iterator_type>::reference;
                using pointer   = typename std::iterator_traits<iterator_type>::pointer;
                using iterator_category = std::forward_iterator_tag;
            
            public:
                constexpr CircularIterator(iterator_type&& data_, typename CircularBuffer<T, size>::size_type offset_, bool cylcled_=false) noexcept;

                constexpr CircularIterator(const CircularIterator<T, size>&) = default;
                
                constexpr CircularIterator<T, size>& operator=(const CircularIterator<T, size>&) = default;

            public:
                constexpr reference operator*() const;
                constexpr pointer operator->() const;

            public:
                constexpr CircularIterator<T, size>& operator++();

                friend constexpr bool operator==<>(const CircularIterator<T, size>& left, const CircularIterator<T, size>& right) noexcept;
                friend constexpr bool operator!=<>(const CircularIterator<T, size>& left, const CircularIterator<T, size>& right) noexcept;

            private:
                iterator_type data;
                iterator_type head;
                typename CircularBuffer<T, size>::size_type offset;

                bool cycled;
        };
    }

    template<typename T, std::size_t size>
    class CircularBuffer {
        public:
            using container_type = std::array<T, size>;

            using value_type = typename container_type::value_type;
            using size_type = typename container_type::size_type;

            using const_iterator = details::CircularIterator<T, size>;
        
        public:
            constexpr CircularBuffer() = default;
        
        public:
            [[nodiscard]] constexpr bool full() const noexcept;

        public:
            constexpr void push_back(value_type&& value);
        
        public:
            [[nodiscard]] constexpr const_iterator begin() const noexcept;
            [[nodiscard]] constexpr const_iterator end() const noexcept;

        private:
            constexpr void increment_head();

        private:
            container_type data;
            size_type head, item_count;
    };
}

#include "implementation/circularbuffer.inl"