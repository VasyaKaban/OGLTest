/**
 * @file
 *
 * Represents the vector class
 */
#pragma once

#include "vector_common.hpp"
#include <cmath>

namespace hrs
{
	namespace math
	{
		/**
		 * @brief The vector class
		 * @tparam T must satisfy the arithmetic concept
		 * @tparam _DIMENSION represents the dimension of vector and must be greater than zero
		 * @tparam _ALIGNMENT represents the alignment of vector structure
		 */
		template<arithmetic T, std::size_t _DIMENSION, std::size_t _ALIGNMENT>
			requires (_DIMENSION > 0)
		struct alignas(_ALIGNMENT) vector
		{
			constexpr static std::size_t DIMENSION = _DIMENSION;///<Dimension of vector
			constexpr static std::size_t ALIGNMENT = _ALIGNMENT;///<Alignment of vector

			using iterator = T *;///<iterator type
			using const_iterator = const T *;///constant iterator type

			T data[DIMENSION];///<inner array data

			/**
			 * @brief vector
			 *
			 * Fills the inner data with zeroes
			 */
			constexpr vector() noexcept : data{}{}

			~vector() = default;

			/**
			 * @brief vector
			 * @tparam A must satisfy the arithmetic concept
			 * @tparam N specifies the size of array
			 * @param arr array of elements to assign
			 *
			 * Assigns values from passed array to inner data array.
			 * If dimension of vector is greater than count of passed arguments
			 * the remain values of data array will be filled with zeroes
			 */
			template<arithmetic A, std::size_t N>
			constexpr vector(const A(&arr)[N]) noexcept
			{
				if constexpr(N < DIMENSION)
				{
					for(std::size_t i = 0; i < N; i++)
						data[i] = arr[i];

					for(std::size_t i = N; i < DIMENSION; i++)
						data[i] = 0;
				}
				else
				{
					for(std::size_t i = 0; i < DIMENSION; i++)
						data[i] = arr[i];

				}
			}

			/**
			 * @brief vector
			 * @tparam A must satisfy the arithmetic concept
			 * @tparam N specifies the size of array
			 * @param arr array of elements to assign
			 *
			 * Assigns values from passed array to inner data array.
			 * If dimension of vector is greater than count of passed arguments
			 * the remain values of data array will be filled with zeroes
			 */
			template<arithmetic A, std::size_t N>
			constexpr vector(A(&&arr)[N]) noexcept
			{
				if constexpr(N < DIMENSION)
				{
					for(std::size_t i = 0; i < N; i++)
						data[i] = arr[i];

					for(std::size_t i = N; i < DIMENSION; i++)
						data[i] = 0;
				}
				else
				{
					for(std::size_t i = 0; i < DIMENSION; i++)
						data[i] = arr[i];
				}
			}

			/**
			 * @brief vector
			 * @tparam A must satisfy the arithmetic concept
			 * @param ptr pointer to data
			 * @param size count of elements to assign
			 *
			 * Assigns values from passed pointer to inner data array.
			 * If dimension of vector is greater than count of passed arguments
			 * the remain values of data array will be filled with zeroes
			 */
			template<arithmetic A>
			constexpr vector(const A *ptr, std::size_t size) noexcept
			{
				if(size < DIMENSION)
				{
					for(std::size_t i = 0; i < size; i++)
						data[i] = ptr[i];

					for(std::size_t i = size; i < DIMENSION; i++)
						data[i] = 0;
				}
				else
				{
					for(std::size_t i = 0; i < DIMENSION; i++)
						data[i] = ptr[i];
				}
			}

			/**
			 * @brief vector
			 * @tparam Args must satisfy the arithmetic concept
			 * @param args arguments to assign
			 *
			 * Assigns passed arguments to inner data array.
			 * If dimension of vector is greater than count of passed arguments
			 * the remain values of data array will be filled with zeroes
			 */
			template<arithmetic ...Args>
			constexpr vector(Args ...args) noexcept : data{static_cast<T>(args)...}
			{
				if constexpr(sizeof...(Args) < DIMENSION)
					for(std::size_t i = sizeof...(Args); i < DIMENSION; i++)
						data[i] = 0;
			}

			/**
			 * @brief vector
			 * @tparam V must satisfy the vector_concept concept
			 * @param v vector object for assignment
			 *
			 * Assigns data from addable vector to target vector if index of value within
			 * vector is less than dimension of target vector.
			 * If dimension of target vector greater than dimension of addable vector
			 * than remain values will be filled with zeroes
			 */
			template<vector_concept V>
			constexpr vector(V &&v) noexcept
			{
				constexpr std::size_t V_DIMENSION = vector_dimension<V>;
				if constexpr(DIMENSION >= V_DIMENSION)
				{
					for(std::size_t i = 0; i < V_DIMENSION; i++)
						data[i] = std::forward<V>(v).data[i];

					for(std::size_t i = V_DIMENSION; i < DIMENSION; i++)
						data[i] = 0;
				}
				else
				{
					for(std::size_t i = 0; i < DIMENSION; i++)
						data[i] = std::forward<V>(v).data[i];
				}
			}

			/**
			 * @brief operator =
			 * @tparam V must satisfy the vector_concept concept
			 * @param v vector object for assignment
			 * @return reference to current object after assignment
			 *
			 * Assigns data from addable vector to target vector if index of value within
			 * vector is less than dimension of target vector.
			 * If dimension of target vector greater than dimension of addable vector
			 * than remain values will be filled with zeroes
			 */
			template<vector_concept V>
			constexpr vector & operator=(V &&v) noexcept
			{
				constexpr std::size_t V_DIMENSION = vector_dimension<V>;
				if constexpr(DIMENSION >= V_DIMENSION)
				{
					for(std::size_t i = 0; i < V_DIMENSION; i++)
						data[i] = std::forward<V>(v).data[i];

					for(std::size_t i = V_DIMENSION; i < DIMENSION; i++)
						data[i] = 0;
				}
				else
				{
					for(std::size_t i = 0; i < DIMENSION; i++)
						data[i] = std::forward<V>(v).data[i];
				}

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
			constexpr std::common_type_t<F, T> length(F eps = default_eps) const noexcept
			{
				T self_dot_product = 0;
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
			 * @return iterator to begin of inner data array
			 *
			 * Overloading for non-constant objects
			 */
			iterator begin() noexcept
			{
				return data;
			}

			/**
			 * @brief begin
			 * @return iterator to begin of inner data array
			 *
			 * Overloading for constant objects
			 */
			const_iterator begin() const noexcept
			{
				return data;
			}

			/**
			 * @brief end
			 * @return iterator to the end of inner data array
			 *
			 * Overloading for non-constant objects
			 */
			iterator end() noexcept
			{
				return data + DIMENSION;
			}

			/**
			 * @brief end
			 * @return iterator to the end of inner data array
			 *
			 * Overloading for constant objects
			 */
			const_iterator end() const noexcept
			{
				return data + DIMENSION;
			}
		};

		/**
		 * @brief vector
		 * @tparam Args must satisfy the arithmetic concept
		 *
		 * The vector deduction guide for variadic arguments.
		 * Value type will be deduced as common type of arguments.
		 * Alignment will be calculated as maximum from arguments' alignment.
		 */
		template<arithmetic ...Args>
		vector(Args ...) -> vector<std::common_type_t<Args...>, sizeof...(Args), max(alignof(Args)...)>;
	};
};
