/**
 * @file
 *
 * Represents the vector_view class
 */

#pragma once

#include "vector_common.hpp"
#include <cmath>

namespace hrs
{
	namespace math
	{
		/**
		 * @brief The vector_view class
		 * @tparam T must satisfy the arithmetic concept
		 * @tparam _DIMENSION represents the dimension of vector and must be greater than zero
		 */
		template<arithmetic T, std::size_t _DIMENSION>
			requires (_DIMENSION > 0)
		struct vector_view
		{
			constexpr static std::size_t DIMENSION = _DIMENSION;///<dimension of vector
			constexpr static std::size_t ALIGNMENT = alignof(T);///<alignment of vector

			using iterator = T *;///<iterator type

			T *data;///<pointer to viewable data

			/**
			 * @brief vector_view
			 *
			 * Sets the inner pointer to nullptr
			 */
			constexpr vector_view() noexcept : data(nullptr) {}
			~vector_view() = default;

			/**
			 * @brief vector_view
			 * @tparam N size of array
			 * @param arr an array to view
			 *
			 * Assigns the inner pointer to passed array
			 */
			template<std::size_t N>
				requires (DIMENSION <= N)
			constexpr vector_view(T(&arr)[N]) noexcept : data(&arr) {}

			/**
			 * @brief vector_view
			 * @tparam N size of array
			 * @param arr an array to view
			 *
			 * Assigns the inner pointer to passed array
			 */
			template<std::size_t N>
			constexpr vector_view(T(&&arr)[N]) noexcept : data(&arr) {}

			/**
			 * @brief vector_view
			 * @param ptr pointer to data
			 *
			 * Assigns the inner pointer to passed pointer
			 */
			constexpr vector_view(T *ptr) noexcept : data(ptr) {}

			/**
			 * @brief vector_view
			 * @tparam V must satisfy the vector_concept concept
			 * @param v vector object for viewing
			 * Sets the inner pointer to point to the first element of passed vector object
			 */
			template<vector_concept V>
				requires
					(DIMENSION <= vector_dimension<V>) &&
					std::same_as<T, vector_value_type<V>>
			constexpr vector_view(V &&v) noexcept : data(&std::forward<V>(v)[0]) {}

			/**
			 * @brief operator =
			 * @tparam V must satisfy the vector_concept concept
			 * @param v vector object for viewing
			 * @return target object
			 *
			 * Simply change the inner pointer to point to the first element of passed vector object
			 */
			template<vector_concept V>
				requires
					(DIMENSION <= vector_dimension<V>) &&
					std::same_as<T, vector_value_type<V>>
			constexpr vector_view & operator=(V &&v) noexcept
			{
				data = &std::forward<V>(v)[0];

				return *this;
			}

			/**
			 * @brief operator []
			 * @param i index within inner array(from 0 to DIMENSION - 1)
			 * @return reference to value stored at this index inside the inner array
			 *
			 * Overloading for non-constant objects
			 */
			constexpr T & operator[](std::size_t i) noexcept
			{
				return data[i];
			}

			/**
			 * @brief operator []
			 * @param i index within inner array(from 0 to DIMENSION - 1)
			 * @return reference to value stored at this index inside the inner array
			 *
			 * Overloading for constant objects
			 */
			constexpr const T & operator[](std::size_t i) const noexcept
			{
				return data[i];
			}

			/**
			 * @brief length
			 * @tparam F must satisfy the floating_point concept
			 * @param eps bias value for normalized length optimizations
			 * @return length of vector
			 */
			template<std::floating_point F = float>
			constexpr auto length(F eps = default_eps) const noexcept
			{
				std::remove_const_t<T> self_dot_product = 0;
				for(std::size_t i = 0; i < DIMENSION; i++)
					self_dot_product += data[i] * data[i];

				if(is_near(self_dot_product, 1.0f, eps))
					return self_dot_product;

				if constexpr(std::is_floating_point_v<T>)
					return std::sqrt(self_dot_product);
				else
					return std::sqrt(static_cast<F>(self_dot_product));
			}

			/**
			 * @brief inv_length
			 * @tparam F must satisfy the floating_point concept
			 * @param eps bias value for normalized length optimizations
			 * @return inverted length value
			 */
			template<std::floating_point F = float>
			constexpr auto inv_length(F eps = default_eps) const noexcept
			{
				auto len = length(eps);
				return static_cast<decltype(len)>(1.0) / len;
			}

			/**
			 * @brief normalize
			 * @tparam F must satisfy the floating_point concept
			 * @param eps bias value for normalized length optimizations
			 * @return normalized vector
			 */
			template<std::floating_point F = float>
			constexpr auto normalize(F eps = default_eps) const noexcept
			{
				vector out_vec(*this);
				return out_vec * inv_length(eps);
			}

			/**
			 * @brief begin
			 * @return iterator to the begin of inner data
			 *
			 * Used for non-constant objects
			 */
			iterator begin() noexcept
			{
				return data;
			}

			/**
			 * @brief begin
			 * @return iterator to the begin of inner data
			 *
			 * Used for constant objects
			 */
			const iterator begin() const noexcept
			{
				return data;
			}

			/**
			 * @brief end
			 * @return iterator to the end of inner data
			 *
			 * Used for non-constant objects
			 */
			iterator end() noexcept
			{
				return data + DIMENSION;
			}

			/**
			 * @brief end
			 * @return iterator to the end of inner data
			 *
			 * Used for constant objects
			 */
			const iterator end() const noexcept
			{
				return data + DIMENSION;
			}

			/**
			 * @brief operator bool
			 *
			 * Checks whether inner data is null pointer ot not
			 */
			constexpr explicit operator bool() const noexcept
			{
				return data;
			}
		};
	};
};
