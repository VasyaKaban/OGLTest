/**
 * @file
 *
 * Represents the vector concepts and its operators
 */

#pragma once

#include <concepts>
#include <cstdint>
#include <utility>
#include "math_common.hpp"

namespace hrs
{
	namespace math
	{
		/**
		 * @brief Forward declaration of vector structure
		 */
		template<arithmetic T, std::size_t _DIMENSION, std::size_t _ALIGNMENT = alignof(T)>
			requires (_DIMENSION > 0)
		struct alignas(_ALIGNMENT) vector;

		/**
		 * @brief Forward declaration of vector_view structure
		 */
		template<arithmetic T, std::size_t _DIMENSION>
			requires (_DIMENSION > 0)
		struct vector_view;

		/**
		 * @brief The vector_concept cincept
		 *
		 * Imposes restrictions for type which must have behaviour like a vector
		 */
		template<typename V>
		concept vector_concept =
			std::unsigned_integral<decltype(std::remove_reference_t<V>::DIMENSION)> &&
			std::unsigned_integral<decltype(std::remove_reference_t<V>::ALIGNMENT)> &&
			arithmetic<std::remove_reference_t<decltype(std::declval<V>()[std::size_t{}])>> &&
			requires(V &&vec)
		{
			requires arithmetic<decltype(vec.length())>;
			requires arithmetic<decltype(vec.inv_length())>;
		};

		/**
		 * @brief vector_dimension
		 * @tparam V must satisfy the vector_concept concept
		 *
		 * Help variable that stores vector type dimension
		 */
		template<vector_concept V>
		constexpr inline std::size_t vector_dimension = std::remove_reference_t<V>::DIMENSION;

		/**
		 * @brief vector_alignment
		 * @tparam V must satisfy the vector_concept concept
		 *
		 * Help variable that stores vector type alignment
		 */
		template<vector_concept V>
		constexpr inline std::size_t vector_alignment = std::remove_reference_t<V>::ALIGNMENT;

		/**
		 * @brief vector_value_type
		 * @tparam V must satisfy the vector_concept concept
		 *
		 * Help using that stores vector value type
		 */
		template<vector_concept V>
		using vector_value_type = std::remove_reference_t<decltype(std::declval<V>()[std::size_t{}])>;

		namespace vector_index_names
		{
			constexpr inline std::size_t x = 0;///<x coordinat of vector(same as 0 index)
			constexpr inline std::size_t y = 1;///<y coordinat of vector(same as 1 index)
			constexpr inline std::size_t z = 2;///<z coordinat of vector(same as 2 index)
		};

		/**
		 * @brief operator +=
		 * @tparam V0 must satisfy the vector_concept concept
		 * @tparam V1 must satisfy the vector_concept concept
		 * @param v0 first vector
		 * @param v1 second vector
		 * @return v0, after adding the v1
		 */
		template<vector_concept V0, vector_concept V1>
		constexpr V0 & operator+=(V0 &v0, V1 &&v1) noexcept
		{
			constexpr std::size_t min_dimension = min(vector_dimension<V0>, vector_dimension<V1>);
			for(std::size_t i = 0; i < min_dimension; i++)
				v0[i] += std::forward<V1>(v1)[i];

			return v0;
		}

		/**
		 * @brief operator -=
		 * @tparam V0 must satisfy the vector_concept concept
		 * @tparam V1 must satisfy the vector_concept concept
		 * @param v0 first vector
		 * @param v1 second vector
		 * @return v0, after subtracting the v1
		 */
		template<vector_concept V0, vector_concept V1>
		constexpr V0 & operator-=(V0 &v0, V1 &&v1) noexcept
		{
			constexpr std::size_t min_dimension = min(vector_dimension<V0>, vector_dimension<V1>);
			for(std::size_t i = 0; i < min_dimension; i++)
				v0[i] -= std::forward<V1>(v1)[i];

			return v0;
		}

		/**
		 * @brief operator ^
		 * @tparam V0 must satisfy the vector_concept concept
		 * @tparam V1 must satisfy the vector_concept concept
		 * @param v0 first vector
		 * @param v1 second vector
		 * @return v0, after cross product with v1
		 */
		template<vector_concept V0, vector_concept V1>
			requires
			(vector_dimension<V0> == vector_dimension<V1>) &&
			(vector_dimension<V0> == 3)
		constexpr V0 & operator^=(V0 &v0, V1 &&v1) noexcept
		{
			v0 = v0 ^ v1;
			return v0;
		}

		/**
		 * @brief operator +
		 * @tparam V0 must satisfy the vector_concept concept
		 * @tparam V1 must satisfy the vector_concept concept
		 * @param v0 first vector
		 * @param v1 second vector
		 * @return sum of first and second vectors
		 */
		template<vector_concept V0, vector_concept V1>
		constexpr auto operator+(V0 &&v0, V1 &&v1) noexcept
		{
			constexpr std::size_t max_dimension = max(vector_dimension<V0>, vector_dimension<V1>);
			constexpr std::size_t max_alignment = max(vector_alignment<V0>, vector_alignment<V1>);
			constexpr std::size_t min_dimension = min(vector_dimension<V0>, vector_dimension<V1>);

			vector<std::common_type_t<vector_value_type<V0>, vector_value_type<V1>>,
				   max_dimension, max_alignment> out_vec;

			for(std::size_t i = 0; i < min_dimension; i++)
				out_vec[i] = std::forward<V0>(v0)[i] + std::forward<V1>(v1)[i];

			if constexpr(vector_dimension<V0> > vector_dimension<V1>)
			{
				for(std::size_t i = min_dimension; i < max_dimension; i++)
					out_vec[i] = std::forward<V0>(v0)[i];
			}
			else
			{
				for(std::size_t i = min_dimension; i < max_dimension; i++)
					out_vec[i] = std::forward<V1>(v1)[i];
			}

			return out_vec;
		}

		/**
		 * @brief operator -
		 * @tparam V0 must satisfy the vector_concept concept
		 * @tparam V1 must satisfy the vector_concept concept
		 * @param v0 first vector
		 * @param v1 second vector
		 * @return difference vector of two vectors
		 */
		template<vector_concept V0, vector_concept V1>
		constexpr auto operator-(V0 &&v0, V1 &&v1) noexcept
		{
			constexpr std::size_t max_dimension = max(vector_dimension<V0>, vector_dimension<V1>);
			constexpr std::size_t max_alignment = max(vector_alignment<V0>, vector_alignment<V1>);
			constexpr std::size_t min_dimension = min(vector_dimension<V0>, vector_dimension<V1>);

			vector<std::common_type_t<vector_value_type<V0>, vector_value_type<V1>>,
				   max_dimension, max_alignment> out_vec;

			for(std::size_t i = 0; i < min_dimension; i++)
				out_vec[i] = std::forward<V0>(v0)[i] - std::forward<V1>(v1)[i];

			if constexpr(vector_dimension<V0> > vector_dimension<V1>)
			{
				for(std::size_t i = min_dimension; i < max_dimension; i++)
					out_vec[i] = -std::forward<V0>(v0)[i];
			}
			else
			{
				for(std::size_t i = min_dimension; i < max_dimension; i++)
					out_vec[i] = -std::forward<V1>(v1)[i];
			}

			return out_vec;
		}

		/**
		 * @brief operator *
		 * @tparam V0 must satisfy the vector_concept concept
		 * @tparam V1 must satisfy the vector_concept concept
		 * @param v0 first vector
		 * @param v1 second vector
		 * @return dot product value of two vectors
		 */
		template<vector_concept V0, vector_concept V1>
		constexpr auto operator*(V0 &&v0, V1 &&v1) noexcept
		{
			std::common_type_t<vector_value_type<V0>, vector_value_type<V1>> out_res = 0;
			constexpr std::size_t min_dimension = min(vector_dimension<V0>, vector_dimension<V1>);
			for(std::size_t i = 0; i < min_dimension; i++)
				out_res += std::forward<V0>(v0)[i] * std::forward<V1>(v1)[i];

			return out_res;
		}

		/**
		 * @brief operator ^
		 * @tparam V0 must satisfy the vector_concept concept
		 * @tparam V1 must satisfy the vector_concept concept
		 * @param v0 first vector
		 * @param v1 second vector
		 * @return cross product of two vectors
		 */
		template<vector_concept V0, vector_concept V1>
			requires
				(vector_dimension<V0> == vector_dimension<V1>) &&
				(vector_dimension<V0> == 3)
		constexpr auto operator^(V0 &&v0, V1 &&v1) noexcept
		{
			constexpr std::size_t max_dimension = max(vector_dimension<V0>, vector_dimension<V1>);
			constexpr std::size_t max_alignment = max(vector_alignment<V0>, vector_alignment<V1>);
			vector<std::common_type_t<vector_value_type<V0>, vector_value_type<V1>>,
				   max_dimension, max_alignment> out_vec;

			using namespace vector_index_names;

			out_vec[x] = std::forward<V0>(v0)[y] * std::forward<V1>(v1)[z] -
						 std::forward<V1>(v1)[y] * std::forward<V0>(v0)[z];

			out_vec[y] = std::forward<V1>(v1)[x] * std::forward<V0>(v0)[z] -
						 std::forward<V0>(v0)[x] * std::forward<V1>(v1)[z];

			out_vec[z] = std::forward<V0>(v0)[x] * std::forward<V1>(v1)[y] -
						 std::forward<V1>(v1)[x] * std::forward<V0>(v0)[y];

			return out_vec;
		}

		/**
		 * @brief operator |
		 * @tparam V0 must satisfy the vector_concept concept
		 * @tparam V1 must satisfy the vector_concept concept
		 * @param v0 first vector
		 * @param v1 second vector
		 * @return projection of first vector to second
		 */
		template<vector_concept V0, vector_concept V1>
		constexpr auto operator|(V0 &&v0, V1 &&v1) noexcept
		{
			auto dot = std::forward<V0>(v0) * std::forward<V1>(v1);
			auto inv_v1_len = std::forward<V1>(v1).inv_length();


			return static_cast<decltype(inv_v1_len)>(dot) * inv_v1_len;
		}

		/**
		 * @brief cos
		 * @tparam V0 must satisfy the vector_concept concept
		 * @tparam V1 must satisfy the vector_concept concept
		 * @param v0 first vector
		 * @param v1 second vector
		 * @return cosine between two vectors
		 */
		template<vector_concept V0, vector_concept V1>
		constexpr auto cos(V0 &&v0, V1 &&v1) noexcept
		{
			auto dot = std::forward<V0>(v0) * std::forward<V1>(v1);
			auto inv_v0_len = std::forward<V0>(v0).inv_length();
			auto inv_v1_len = std::forward<V1>(v1).inv_length();

			return std::common_type_t<decltype(dot), decltype(inv_v0_len), decltype(inv_v1_len)>(dot) *
				   inv_v0_len * inv_v1_len;
		}

		/**
		 * @brief operator+=
		 * @tparam V must satisfy the vector_concept concept
		 * @tparam S must satisfy the arithmetic concept
		 * @param v vector object
		 * @param s scalar object
		 * @return first vector after adding the scalar value to it
		 */
		template<vector_concept V, arithmetic S>
		constexpr V & operator+=(V &v, S s) noexcept
		{
			for(std::size_t i = 0; i < vector_dimension<V>; i++)
				v[i] += s;

			return v;
		}

		/**
		 * @brief operator-=
		 * @tparam V must satisfy the vector_concept concept
		 * @tparam S must satisfy the arithmetic concept
		 * @param v vector object
		 * @param s scalar object
		 * @return first vector after subtracting the scalar value from it
		 */
		template<vector_concept V, arithmetic S>
		constexpr V & operator-=(V &v, S s) noexcept
		{
			for(std::size_t i = 0; i < vector_dimension<V>; i++)
				v[i] -= s;

			return v;
		}

		/**
		 * @brief operator*=
		 * @tparam V must satisfy the vector_concept concept
		 * @tparam S must satisfy the arithmetic concept
		 * @param v vector object
		 * @param s scalar object
		 * @return first vector after multiplicating the vector value by a scalar
		 */
		template<vector_concept V, arithmetic S>
		constexpr V & operator*=(V &v, S s) noexcept
		{
			for(std::size_t i = 0; i < vector_dimension<V>; i++)
				v[i] *= s;

			return v;
		}

		/**
		 * @brief operator/=
		 * @tparam V must satisfy the vector_concept concept
		 * @tparam S must satisfy the arithmetic concept
		 * @param v vector object
		 * @param s scalar object
		 * @return first vector after dividing the vector value by a scalar
		 */
		template<vector_concept V, arithmetic S>
		constexpr V & operator/=(V &v, S s) noexcept
		{
			for(std::size_t i = 0; i < vector_dimension<V>; i++)
				v[i] /= s;

			return v;
		}

		/**
		 * @brief operator+
		 * @tparam V must satisfy the vector_concept concept
		 * @tparam S must satisfy the arithmetic concept
		 * @param v vector object
		 * @param s scalar object
		 * @return vector after adding the scalar value to vector
		 */
		template<vector_concept V, arithmetic S>
		constexpr auto operator+(V &&v, S &s) noexcept
		{
			std::remove_cvref_t<V> out_vec;
			for(std::size_t i = 0; i < vector_dimension<V>; i++)
				out_vec[i] = std::forward<V>(v)[i] + s;

			return out_vec;
		}

		/**
		 * @brief operator-
		 * @tparam V must satisfy the vector_concept concept
		 * @tparam S must satisfy the arithmetic concept
		 * @param v vector object
		 * @param s scalar object
		 * @return vector after subtracting the scalar value from vector
		 */
		template<vector_concept V, arithmetic S>
		constexpr auto operator-(V &&v, S &s) noexcept
		{
			std::remove_cvref_t<V> out_vec;
			for(std::size_t i = 0; i < vector_dimension<V>; i++)
				out_vec[i] = std::forward<V>(v)[i] - s;

			return out_vec;
		}

		/**
		 * @brief operator*
		 * @tparam V must satisfy the vector_concept concept
		 * @tparam S must satisfy the arithmetic concept
		 * @param v vector object
		 * @param s scalar object
		 * @return vector after multiplicating the scalar value by a vector
		 */
		template<vector_concept V, arithmetic S>
		constexpr auto operator*(V &&v, S s) noexcept
		{
			std::remove_cvref_t<V> out_vec;
			for(std::size_t i = 0; i < vector_dimension<V>; i++)
				out_vec[i] = std::forward<V>(v)[i] * s;

			return out_vec;
		}

		/**
		 * @brief operator/
		 * @tparam V must satisfy the vector_concept concept
		 * @tparam S must satisfy the arithmetic concept
		 * @param v vector object
		 * @param s scalar object
		 * @return vector after the dividing the vector by a scalar value
		 */
		template<vector_concept V, arithmetic S>
		constexpr auto operator/(V &&v, S &s) noexcept
		{
			std::remove_cvref_t<V> out_vec;
			for(std::size_t i = 0; i < vector_dimension<V>; i++)
				out_vec[i] = std::forward<V>(v)[i] / s;

			return out_vec;
		}

		/**
		 * @brief shrink_vector
		 * @tparam OFFSET offset within vector
		 * @tparam DIMENSION dimension of new vector
		 * @tparam ALIGNMENT alignment of new vector
		 * @param v vector object to shrink
		 * @return shrinked vector
		 */
		template<std::size_t OFFSET,
				 std::size_t DIMENSION,
				 std::size_t ALIGNMENT,
				 vector_concept V>
			requires
				(DIMENSION > 0) &&
				(vector_dimension<V> >= OFFSET + DIMENSION)
		constexpr auto shrink_vector(V &&v) noexcept
		{
			vector<vector_value_type<V>, DIMENSION, ALIGNMENT> out_vec;
			for(std::size_t i = OFFSET; i < OFFSET + DIMENSION; i++)
				out_vec[i - OFFSET] = std::forward<V>(v)[i];

			return out_vec;
		}

		/**
		 * @brief take_vector_view
		 * @tparam OFFSET offset within vector
		 * @tparam DIMENSION dimension of new vector_view
		 * @param v vector object to taking a view over it
		 */
		template<std::size_t OFFSET,
				 std::size_t DIMENSION,
				 vector_concept V>
			requires
				(DIMENSION > 0) &&
				(vector_dimension<V> >= OFFSET + DIMENSION)
		constexpr auto take_vector_view(V &&v) noexcept
		{
			vector_view<vector_value_type<V>, DIMENSION> out_view;
			out_view.data = &std::forward<V>(v)[OFFSET];

			return out_view;
		}

		namespace glsl
		{
			inline namespace scalar
			{
				using bvec2 = vector<bool, 2, alignof(std::uint32_t)>;
				using ivec2 = vector<std::int32_t, 2>;
				using uvec2 = vector<std::uint32_t, 2>;
				using vec2 = vector<float, 2>;
				using dvec2 = vector<double, 2>;

				using bvec3 = vector<bool, 3, alignof(std::uint32_t)>;
				using ivec3 = vector<std::int32_t, 3>;
				using uvec3 = vector<std::uint32_t, 3>;
				using vec3 = vector<float, 3>;
				using dvec3 = vector<double, 3>;

				using bvec4 = vector<bool, 4, alignof(std::uint32_t)>;
				using ivec4 = vector<std::int32_t, 4>;
				using uvec4 = vector<std::uint32_t, 4>;
				using vec4 = vector<float, 4>;
				using dvec4 = vector<double, 4>;
			};

			namespace std140
			{
				using bvec2 = vector<bool, 2, alignof(std::uint32_t) * 2>;
				using ivec2 = vector<std::int32_t, 2, alignof(std::int32_t) * 2>;
				using uvec2 = vector<std::uint32_t, 2, alignof(std::uint32_t) * 2>;
				using vec2 = vector<float, 2, alignof(float) * 2>;
				using dvec2 = vector<double, 2, alignof(double) * 2>;

				using bvec3 = vector<bool, 3, alignof(std::uint32_t) * 4>;
				using ivec3 = vector<std::int32_t, 3, alignof(std::int32_t) * 4>;
				using uvec3 = vector<std::uint32_t, 3, alignof(std::uint32_t) * 4>;
				using vec3 = vector<float, 3, alignof(float) * 4>;
				using dvec3 = vector<double, 3, alignof(double) * 4>;

				using bvec4 = vector<bool, 4, alignof(std::uint32_t) * 4>;
				using ivec4 = vector<std::int32_t, 4, alignof(std::int32_t) * 4>;
				using uvec4 = vector<std::uint32_t, 4, alignof(std::uint32_t) * 4>;
				using vec4 = vector<float, 4, alignof(float) * 4>;
				using dvec4 = vector<double, 4, alignof(double) * 4>;
			};

			namespace std430
			{
				using bvec2 = std140::bvec2;
				using ivec2 = std140::ivec2;
				using uvec2 = std140::uvec2;
				using vec2 = std140::vec2;
				using dvec2 = std140::dvec2;

				using bvec3 = std140::bvec3;
				using ivec3 = std140::ivec3;
				using uvec3 = std140::uvec3;
				using vec3 = std140::vec3;
				using dvec3 = std140::dvec3;

				using bvec4 = std140::bvec4;
				using ivec4 = std140::ivec4;
				using uvec4 = std140::uvec4;
				using vec4 = std140::vec4;
				using dvec4 = std140::dvec4;
			};
		};
	};
};
