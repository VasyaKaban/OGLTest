/**
 * @file
 *
 * Represents the common math functions, concepts and types
 */

#pragma once

#include <concepts>
#include <numbers>

namespace hrs
{
	namespace math
	{
		/**
		 * @brief arithmetic
		 *
		 * Concept over std::is_arithmetic
		 */
		template<typename T>
		concept arithmetic = std::is_arithmetic_v<T>;

		/**
		 * @brief default_eps
		 *
		 * Default small value for bound checking(epsilon)
		 */
		constexpr inline double default_eps = 0.000001;

		/**
		 * @brief is_near
		 * @tparam A must satisfy the arithmetic concept
		 * @tparam F must satisfy the floating_point concept
		 * @param value checkable value
		 * @param bound value that is compared with biased value
		 * @param eps bias for value
		 * @return true if value is within bound and false otherwise
		 *
		 *
		 * Simply makes the next check:
		 * @code
		 *	if((value - eps) <= bound && bound <= (value + eps))
		 *		return true;
		 *
		 *	return false;
		 * @endcode
		 * Used for length optimizations when we want to avoid square root operation
		 * in case length is equals to one or very close to it(with +-eps bias)
		 */
		template<arithmetic A, std::floating_point F>
		constexpr bool is_near(A value, F bound, F eps = default_eps)
		{
			using COMMON_F = std::common_type_t<F, A>;
			if constexpr(std::same_as<F, COMMON_F>)
			{
				COMMON_F floated_value = static_cast<COMMON_F>(value);
				if((floated_value - eps) <= bound && bound <= (floated_value + eps))
					return true;

				return false;
			}
			else
			{
				COMMON_F floated_eps = static_cast<COMMON_F>(eps);
				if((value - floated_eps) <= bound && bound <= (value + floated_eps))
					return true;

				return false;
			}
		}

		/**
		 * @brief max_from_t_and_args
		 * @tparam C must satisfy the arithmetic concept
		 * @tparam T0 must satisfy the arithmetic concept
		 * @tparam T1 must satisfy the arithmetic concept
		 * @tparam Args must satisfy the arithmetic concept
		 * @param t0 fisrt argument
		 * @param t1 second argument
		 * @param args tail arguments
		 * @return maximum among t0, t1 and args
		 *
		 * Help function for recursive search of maximum value
		 */
		template<arithmetic C, arithmetic T0, arithmetic T1, arithmetic ...Args>
		constexpr auto max_from_t_and_args(T0 t0, T1 t1, Args ...args) noexcept -> C
		{
			if constexpr(sizeof...(Args) == 0)
			{
				if(static_cast<C>(t0) > static_cast<C>(t1))
					return t0;
				else
					return t1;
			}
			else
			{
				if(static_cast<C>(t0) > static_cast<C>(t1))
					return max_from_t_and_args<C>(t0, args...);
				else
					return max_from_t_and_args<C>(t1, args...);
			}
		}

		/**
		 * @brief max
		 * @tparam Args must satisfy arithmetic concept
		 * @param args arguments among which we wants to find max
		 *
		 * Finds the maximum among the passed arguments
		 */
		template<arithmetic ...Args>
			requires (sizeof...(Args) > 0)
		constexpr auto max(Args ...args) noexcept
		{
			if constexpr(sizeof...(Args) == 1)
				return [](auto t){return t;}(args...);
			else
				return max_from_t_and_args<std::common_type_t<Args...>>(args...);
		}

		/**
		 * @brief min_from_t_and_args
		 * @tparam C must satisfy the arithmetic concept
		 * @tparam T0 must satisfy the arithmetic concept
		 * @tparam T1 must satisfy the arithmetic concept
		 * @tparam Args must satisfy the arithmetic concept
		 * @param t0 fisrt argument
		 * @param t1 second argument
		 * @param args tail arguments
		 * @return minimum among t0, t1 and args
		 *
		 * Help function for recursive search of minimum value
		 */
		template<arithmetic C, arithmetic T0, arithmetic T1, arithmetic ...Args>
		constexpr auto min_from_t_and_args(T0 t0, T1 t1, Args ...args) noexcept -> C
		{
			if constexpr(sizeof...(Args) == 0)
			{
				if(static_cast<C>(t0) < static_cast<C>(t1))
					return t0;
				else
					return t1;
			}
			else
			{
				if(static_cast<C>(t0) < static_cast<C>(t1))
					return max_from_t_and_args<C>(t0, args...);
				else
					return max_from_t_and_args<C>(t1, args...);
			}
		}

		/**
		 * @brief max
		 * @tparam Args must satisfy the arithmetic concept
		 * @param args arguments among which we wants to find min
		 *
		 * Finds the minimum among the passed arguments
		 */
		template<arithmetic ...Args>
			requires (sizeof...(Args) > 0)
		constexpr auto min(Args ...args) noexcept
		{
			if constexpr(sizeof...(Args) == 1)
				return [](auto t){return t;}(args...);
			else
				return min_from_t_and_args<std::common_type_t<Args...>>(args...);
		}

		/**
		 * @brief deg_to_rad
		 * @tparam F must satisfy the afloating_point concept
		 * @tparam T must satisfy the arithmetic concept
		 * @param deg value in degrees
		 * @return converted value in degrees into radians
		 *
		 * Uses the F type as return type from function like common_type<F, T>
		 */
		template<std::floating_point F = float, arithmetic T>
		constexpr auto deg_to_rad(T deg) noexcept
		{
			using OUT_TYPE = std::common_type_t<F, T>;
			return static_cast<OUT_TYPE>(deg) * std::numbers::pi_v<OUT_TYPE> / static_cast<OUT_TYPE>(180.0f);
		}

		/**
		 * @brief rad_to_deg
		 * @tparam F must satisfy the afloating_point concept
		 * @tparam T must satisfy the arithmetic concept
		 * @param deg value in radians
		 * @return converted value in radians into degrees
		 *
		 * Uses the F type as return type from function like common_type<F, T>
		 */
		template<std::floating_point F = float, arithmetic T>
		constexpr auto rad_to_deg(T deg) noexcept
		{
			using OUT_TYPE = std::common_type_t<F, T>;
			return static_cast<OUT_TYPE>(deg) * static_cast<OUT_TYPE>(180.0f) / std::numbers::pi_v<OUT_TYPE>;
		}
	};
};
