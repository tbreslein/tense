/**
 * @file tense.hpp
 * @brief main entry point for the tense lib
 * @author Tommy Breslein (https://github.com/tbreslein)
 *
 * @copyright Copyright (c) 2023 Tommy Breslein <https://github.com/tbreslein>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <array>
#include <type_traits>

/// @brief Namespace for this lib
namespace tense {

template <class T, std::size_t... Dims>
class Tensor : std::integral_constant<bool, std::is_arithmetic<T>::value> {
    constexpr const static std::array<std::size_t, sizeof...(Dims)> dims{
        Dims...};
    std::array<T, (... * Dims)> data;
    using This = Tensor<T, Dims...>;

  public:
    Tensor()
        : data{} {}

    explicit Tensor(const T x)
        : data{} {
        this->data.fill(x);
    }

    explicit Tensor(const std::array<T, (... * Dims)> &input)
        : data{} {
        for (std::size_t i = 0; i < input.size(); i++) {
            this->data[i] = input[i];
        }
    }

    explicit Tensor(const T *input)
        : data{} {
        const auto foo = std::to_array(input);
        for (std::size_t i = 0; i < foo.size(); i++) {
            this->data[i] = foo[i];
        }
    }

    constexpr std::size_t dim_len(const std::size_t i) const noexcept {
        static_assert(i < sizeof...(Dims));
        return dims[i];
    }

    constexpr T &operator()(const std::size_t i) const noexcept
        __attribute__((always_inline)) {
        static_assert(sizeof...(Dims) == 1);
        return this->data[i];
    }
    constexpr T &operator()(const std::size_t j,
                            const std::size_t i) const noexcept
        __attribute__((always_inline)) {
        static_assert(sizeof...(Dims) == 2);
        return this->data[j * this->dims[0] + i];
    }
    constexpr T &operator()(const std::size_t k, const std::size_t j,
                            const std::size_t i) const noexcept
        __attribute__((always_inline)) {
        static_assert(sizeof...(Dims) == 3);
        return this->data[k * this->dims[0] + j * this->dims[1] + i];
    }
    constexpr T &operator()(const std::size_t l, const std::size_t k,
                            const std::size_t j,
                            const std::size_t i) const noexcept
        __attribute__((always_inline)) {
        static_assert(sizeof...(Dims) == 4);
        return this->data[l * this->dims[0] + k * this->dims[1] +
                          j * this->dims[2] + i];
    }

    constexpr This operator+(const This &other) const noexcept
        __attribute__((always_inline)) {
        auto foo = Tensor(other);
        for (std::size_t i = 0; i < this->data.size(); i++)
            foo[i] = this->data[i] + other->data[i];
        return foo;
    }

    constexpr void operator+=(const This &other) noexcept
        __attribute__((always_inline)) {
        for (std::size_t i = 0; i < this->data.size(); i++)
            this->data[i] += other->data[i];
    }

    constexpr This operator-(const This &other) const noexcept
        __attribute__((always_inline)) {
        auto foo = Tensor(other);
        for (std::size_t i = 0; i < this->data.size(); i++)
            foo[i] = this->data[i] - other->data[i];
        return foo;
    }

    constexpr void operator-=(const This &other) noexcept
        __attribute__((always_inline)) {
        for (std::size_t i = 0; i < this->data.size(); i++)
            this->data[i] -= other->data[i];
    }

    constexpr This operator*(const This &other) const noexcept
        __attribute__((always_inline)) {
        auto foo = Tensor(other);
        for (std::size_t i = 0; i < this->data.size(); i++)
            foo[i] = this->data[i] * other->data[i];
        return foo;
    }

    constexpr void operator*=(const This &other) noexcept
        __attribute__((always_inline)) {
        for (std::size_t i = 0; i < this->data.size(); i++)
            this->data[i] *= other->data[i];
    }

    constexpr This operator/(const This &other) const noexcept
        __attribute__((always_inline)) {
        auto foo = Tensor(other);
        for (std::size_t i = 0; i < this->data.size(); i++)
            foo[i] = this->data[i] / other->data[i];
        return foo;
    }

    constexpr void operator/=(const This &other) noexcept
        __attribute__((always_inline)) {
        for (std::size_t i = 0; i < this->data.size(); i++)
            this->data[i] /= other->data[i];
    }

    constexpr void operator-() noexcept __attribute__((always_inline)) {
        // TODO assert that T can be cast to -1
        for (std::size_t i = 0; i < this->data.size(); i++)
            this->data[i] *= static_cast<T>(-1);
        return *this;
    }

    constexpr This &operator=(const This &other) noexcept
        __attribute__((always_inline)) {
        for (std::size_t i = 0; i < this->data.size(); i++)
            this->data[i] = other->data[i];
        return *this;
    }
};
} // namespace tense
