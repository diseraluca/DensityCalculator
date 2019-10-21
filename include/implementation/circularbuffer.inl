// Copyright 2019 Luca Di Sera
//		Contact: disera.luca@gmail.com
//				 https://github.com/diseraluca
//				 https://www.linkedin.com/in/luca-di-sera-200023167
//
// This code is licensed under the MIT License. 
// More informations can be found in the LICENSE file in the root folder of this repository

#pragma once

#include "../circularbuffer.hpp"

#include <memory>

template<typename T, std::size_t size>
[[nodiscard]] constexpr bool densitycalculator::CircularBuffer<T, size>::full() const noexcept {
    return item_count == size;
}

template<typename T, std::size_t size>
constexpr void densitycalculator::CircularBuffer<T, size>::push_back(value_type&& value) {
    if (full()) {
        data[head] = std::move(value);
        increment_head();
    } else {
       data[item_count] = std::move(value);
       ++item_count;
    }
}

template<typename T, std::size_t size>
[[nodiscard]] constexpr typename densitycalculator::CircularBuffer<T, size>::const_iterator densitycalculator::CircularBuffer<T, size>::begin() const noexcept {
    return details::CircularIterator<T, size>(data.begin(), head);
}

template<typename T, std::size_t size>
[[nodiscard]] constexpr typename densitycalculator::CircularBuffer<T, size>::const_iterator densitycalculator::CircularBuffer<T, size>::end() const noexcept {
    return details::CircularIterator<T, size>(data.begin(), head, true);
}

template<typename T, std::size_t size>
constexpr void densitycalculator::CircularBuffer<T, size>::increment_head() {
    if (++head == size) { head = 0; }
}

template<typename T, std::size_t size>
constexpr densitycalculator::details::CircularIterator<T, size>::CircularIterator(iterator_type&& data_, typename CircularBuffer<T, size>::size_type offset_, bool cycled_) noexcept : data{std::move(data_)}, head{std::next(data, offset_)}, offset{offset_}, cycled{cycled_} {}

template<typename T, std::size_t size>
constexpr typename densitycalculator::details::CircularIterator<T, size>::reference densitycalculator::details::CircularIterator<T, size>::operator*() const {
    return *head;
}

template<typename T, std::size_t size>
constexpr typename densitycalculator::details::CircularIterator<T, size>::pointer densitycalculator::details::CircularIterator<T, size>::operator->() const {
    return head;
}

template<typename T, std::size_t size>
constexpr densitycalculator::details::CircularIterator<T, size>& densitycalculator::details::CircularIterator<T, size>::operator++() {
    head = std::next(data, (std::distance(data, head) + 1) % size);
    cycled = true;

    return *this;
}


template<typename T_, std::size_t size_>
constexpr bool densitycalculator::details::operator==(const CircularIterator<T_, size_>& left, const CircularIterator<T_, size_>& right) noexcept {
    return left.data == right.data && left.head == right.head && left.offset == right.offset && left.cycled == right.cycled;
}

template<typename T_, std::size_t size_>
constexpr bool densitycalculator::details::operator!=(const CircularIterator<T_, size_>& left, const CircularIterator<T_, size_>& right) noexcept {
    return !(left == right);
}