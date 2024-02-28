/**
 * @file
 *
 * Represnets matrix concepts and its operators
 */

#pragma once

#include "math_common.hpp"
#include "vector_common.hpp"
#include <utility>

namespace hrs
{
	namespace math
	{
		/**
		 * @brief Forward declaration for matrix class
		 */
		template<arithmetic T, std::size_t _ROWS, std::size_t _COLS, std::size_t _ALIGNMENT = alignof(T)>
			requires (_ROWS > 0 && _COLS > 0)
		struct matrix;

		/**
		 * @brief Forward declaration for matrix_view class
		 */
		template<arithmetic T, std::size_t _ROWS, std::size_t _COLS>
		struct matrix_view;

		/**
		 * @brief The matrix_concept concept
		 *
		 * Imposes restrictions for type which must have behaviour like a matrix
		 */
		template<typename M>
		concept matrix_concept =
			std::unsigned_integral<decltype(std::remove_reference_t<M>::ROWS)> &&
			std::unsigned_integral<decltype(std::remove_reference_t<M>::COLS)> &&
			std::unsigned_integral<decltype(std::remove_reference_t<M>::ALIGNMENT)> &&
			arithmetic<std::remove_reference_t<decltype(std::declval<M>()[std::size_t{}][std::size_t{}])>>;

		/**
		 * @brief The matrix_view_concept concept
		 *
		 * Imposes restrictions for type which must have behaviour like a matrix_view
		 */
		template<typename M>
		concept matrix_view_concept =
			matrix_concept<M> &&
			std::integral<decltype(std::declval<M>().stride())>;

		/**
		 * @brief matrix_rows
		 * @tparam M must satisfy the matrix_concept concept
		 *
		 * Help variable that stores matrix rows count
		 */
		template<matrix_concept M>
		constexpr inline std::size_t matrix_rows = std::remove_reference_t<M>::ROWS;

		/**
		 * @brief matrix_cols
		 * @tparam M must satisfy the matrix_concept concept
		 *
		 * Help variable that stores matrix columns count
		 */
		template<matrix_concept M>
		constexpr inline std::size_t matrix_cols = std::remove_reference_t<M>::COLS;

		/**
		 * @brief matrix_alignment
		 * @tparam M must satisfy the matrix_concept concept
		 *
		 * Help variable that stores matrix alignment
		 */
		template<matrix_concept M>
		constexpr inline std::size_t matrix_alignment = std::remove_reference_t<M>::ALIGNMENT;

		/**
		 * @brief matrix_value_type
		 * @tparam M must satisfy the matrix_concept concept
		 *
		 * Help using that stores matrix value type
		 */
		template<matrix_concept M>
		using matrix_value_type =
			std::remove_reference_t<decltype(std::declval<M>()[std::size_t{}][std::size_t{}])>;

		/**
		 * @brief matrix_row_type
		 * @tparam M must satisfy the matrix_concept concept
		 *
		 * Help using that stores matrix row type
		 */
		template<matrix_concept M>
		using matrix_row_type = std::remove_reference_t<decltype(std::declval<M>()[std::size_t{}])>;

		/**
		 * @brief operator +=
		 * @tparam M0 must satisfy the matrix_concept concept
		 * @tparam M1 must satisfy the matrix_concept concept
		 * @param m0 first matrix object
		 * @param m1 second matrix object
		 * @return reference to first matrix
		 */
		template<matrix_concept M0, matrix_concept M1>
		constexpr M0 & operator+=(M0 &m0, M1 &&m1) noexcept
		{
			constexpr std::size_t min_rows = min(matrix_rows<M0>, matrix_rows<M1>);

			for(std::size_t i = 0; i < min_rows; i++)
				m0[i] += std::forward<M1>(m1)[i];

			return m0;
		}

		/**
		 * @brief operator -=
		 * @tparam M0 must satisfy the matrix_concept concept
		 * @tparam M1 must satisfy the matrix_concept concept
		 * @param m0 first matrix object
		 * @param m1 second matrix object
		 * @return reference to first matrix
		 */
		template<matrix_concept M0, matrix_concept M1>
		constexpr M0 & operator-=(M0 &m0, M1 &&m1) noexcept
		{
			constexpr std::size_t min_rows = min(matrix_rows<M0>, matrix_rows<M1>);

			for(std::size_t i = 0; i < min_rows; i++)
				m0[i] -= std::forward<M1>(m1)[i];

			return m0;
		}

		/**
		 * @brief operator +=
		 * @tparam M must satisfy the matrix_concept concept
		 * @tparam A must satisfy the arithmetic concept
		 * @param m matrix object
		 * @param value scalar
		 * @return reference to passed matrix
		 *
		 * Makes the addition of a scalar to each element of matrix
		 */
		template<matrix_concept M, arithmetic A>
		constexpr M & operator+=(M &m, A value) noexcept
		{
			for(std::size_t i = 0; i < matrix_rows<M>; i++)
				for(std::size_t j = 0; i < matrix_cols<M>; j++)
					m[i][j] += value;

			return m;
		}

		/**
		 * @brief operator -=
		 * @tparam M must satisfy the matrix_concept concept
		 * @tparam A must satisfy the arithmetic concept
		 * @param m matrix object
		 * @param value scalar
		 * @return reference to passed matrix
		 *
		 * Makes the subtraction of a scalar from each element of matrix
		 */
		template<matrix_concept M, arithmetic A>
		constexpr M & operator-=(M &m, A value) noexcept
		{
			for(std::size_t i = 0; i < matrix_rows<M>; i++)
				for(std::size_t j = 0; i < matrix_cols<M>; j++)
					m[i][j] -= value;

			return m;
		}

		/**
		 * @brief operator *=
		 * @tparam M must satisfy the matrix_concept concept
		 * @tparam A must satisfy the arithmetic concept
		 * @param m matrix object
		 * @param value scalar
		 * @return reference to passed matrix
		 *
		 * Makes the multiplication of a scalar by each element of matrix
		 */
		template<matrix_concept M, arithmetic A>
		constexpr M & operator*=(M &m, A value) noexcept
		{
			for(std::size_t i = 0; i < matrix_rows<M>; i++)
				for(std::size_t j = 0; i < matrix_cols<M>; j++)
					m[i][j] *= value;

			return m;
		}

		/**
		 * @brief operator /=
		 * @tparam M must satisfy the matrix_concept concept
		 * @tparam A must satisfy the arithmetic concept
		 * @param m matrix object
		 * @param value scalar
		 * @return reference to passed matrix
		 *
		 * Divide each element of the matix by a scalar
		 */
		template<matrix_concept M, arithmetic A>
		constexpr M & operator/=(M &m, A value) noexcept
		{
			for(std::size_t i = 0; i < matrix_rows<M>; i++)
				for(std::size_t j = 0; i < matrix_cols<M>; j++)
					m[i][j] /= value;

			return m;
		}

		/**
		 * @brief operator *
		 * @tparam M0 must satisfy the matrix_concept concept
		 * @tparam M1 must satisfy the matrix_concept concept
		 * @param m0 first matrix object
		 * @param m1 second matrix object
		 * @return matrices multiplication result
		 */
		template<matrix_concept M0, matrix_concept M1>
			requires (matrix_cols<M0> == matrix_rows<M1>)
		constexpr auto operator*(M0 &&m0, M1 &&m1) noexcept
		{
			matrix<std::common_type_t<matrix_value_type<M0>, matrix_value_type<M1>>,
				   matrix_rows<M0>, matrix_cols<M1>, matrix_alignment<M0>> out_mat;

			for(std::size_t i = 0; i < matrix_rows<M0>; i++)
			{
				//for each row in m0
				for(std::size_t j = 0; j < matrix_cols<M1>; j++)
				{
					//for each col in m1
					out_mat[i][j] = 0;
					for(std::size_t k = 0; k < matrix_rows<M1>; k++)
					{
						//for each element in row and col
						out_mat[i][j] += std::forward<M0>(m0)[i][k] * std::forward<M1>(m1)[k][j];
					}
				}
			}

			return out_mat;
		}

		/**
		 * @brief operator +
		 * @tparam M0 must satisfy the matrix_concept concept
		 * @tparam M1 must satisfy the matrix_concept concept
		 * @param m0 first matrix object
		 * @param m1 second matrix object
		 * @return matrices addition result
		 */
		template<matrix_concept M0, matrix_concept M1>
		constexpr auto operator+(M0 &&m0, M1 &&m1) noexcept
		{
			constexpr std::size_t max_rows = max(matrix_rows<M0>, matrix_rows<M1>);
			constexpr std::size_t max_cols = max(matrix_cols<M0>, matrix_cols<M1>);
			constexpr std::size_t max_alignment = max(matrix_alignment<M0>, matrix_alignment<M1>);

			matrix<std::common_type_t<matrix_value_type<M0>, matrix_value_type<M1>>,
				   max_rows, max_cols, max_alignment> out_mat(std::forward<M0>(m0));

			for(std::size_t i = 0; i < matrix_rows<M1>; i++)
				out_mat[i] += std::forward<M1>(m1)[i];

			return out_mat;
		}

		/**
		 * @brief operator -
		 * @tparam M0 must satisfy the matrix_concept concept
		 * @tparam M1 must satisfy the matrix_concept concept
		 * @param m0 first matrix object
		 * @param m1 second matrix object
		 * @return matrices subtracting result
		 */
		template<matrix_concept M0, matrix_concept M1>
		constexpr auto operator-(M0 &&m0, M1 &&m1) noexcept
		{
			constexpr std::size_t max_rows = max(matrix_rows<M0>, matrix_rows<M1>);
			constexpr std::size_t max_cols = max(matrix_cols<M0>, matrix_cols<M1>);
			constexpr std::size_t max_alignment = max(matrix_alignment<M0>, matrix_alignment<M1>);

			matrix<std::common_type_t<matrix_value_type<M0>, matrix_value_type<M1>>,
				   max_rows, max_cols, max_alignment> out_mat(std::forward<M0>(m0));

			for(std::size_t i = 0; i < matrix_rows<M1>; i++)
				out_mat[i] -= std::forward<M1>(m1)[i];

			return out_mat;
		}

		/**
		 * @brief operator +
		 * @tparam M must satisfy the matrix_concept concept
		 * @tparam A must satisfy the arithmetic concept
		 * @param m matrix object
		 * @param value scalar
		 * @return matrix after performing an addition operation on each element of passed matrix with a scalar
		 */
		template<matrix_concept M, arithmetic A>
		constexpr auto operator+(M &&m, A value) noexcept
		{
			std::remove_cvref_t<M> out_m = std::forward<M>(m);
			for(std::size_t i = 0; i < matrix_rows<M>; i++)
				for(std::size_t j = 0; i < matrix_cols<M>; j++)
					out_m[i][j] += value;

			return out_m;
		}

		/**
		 * @brief operator -
		 * @tparam M must satisfy the matrix_concept concept
		 * @tparam A must satisfy the arithmetic concept
		 * @param m matrix object
		 * @param value scalar
		 * @return matrix after performing a subtract operation on each element of passed matrix with a scalar
		 */
		template<matrix_concept M, arithmetic A>
		constexpr auto operator-(M &&m, A value) noexcept
		{
			std::remove_cvref_t<M> out_m = std::forward<M>(m);
			for(std::size_t i = 0; i < matrix_rows<M>; i++)
				for(std::size_t j = 0; i < matrix_cols<M>; j++)
					out_m[i][j] -= value;

			return out_m;
		}

		/**
		 * @brief operator *
		 * @tparam M must satisfy the matrix_concept concept
		 * @tparam A must satisfy the arithmetic concept
		 * @param m matrix object
		 * @param value scalar
		 * @return matrix after performing a multiplication operation on each element of passed matrix by a scalar
		 */
		template<matrix_concept M, arithmetic A>
		constexpr auto operator*(M &&m, A value) noexcept
		{
			std::remove_cvref_t<M> out_m = std::forward<M>(m);
			for(std::size_t i = 0; i < matrix_rows<M>; i++)
				for(std::size_t j = 0; i < matrix_cols<M>; j++)
					out_m[i][j] *= value;

			return out_m;
		}

		/**
		 * @brief operator /
		 * @tparam M must satisfy the matrix_concept concept
		 * @tparam A must satisfy the arithmetic concept
		 * @param m matrix object
		 * @param value scalar
		 * @return matrix after performing a division operation on each element of passed matrix by a scalar
		 */
		template<matrix_concept M, arithmetic A>
		constexpr auto operator/(M &&m, A value) noexcept
		{
			std::remove_cvref_t<M> out_m = std::forward<M>(m);
			for(std::size_t i = 0; i < matrix_rows<M>; i++)
				for(std::size_t j = 0; i < matrix_cols<M>; j++)
					out_m[i][j] /= value;

			return out_m;
		}

		/**
		 * @brief operator -
		 * @tparam V must satisfy the vector_concept concept
		 * @tparam M must satisfy the matrix_concept concept
		 * @param v vector object
		 * @param m matrix object
		 * @return result vector from vector-by-matrix multiplication operation
		 *
		 * This operator assumes that vectors are row-major objects
		 */
		template<vector_concept V, matrix_concept M>
			requires (vector_dimension<V> == matrix_rows<M>)
		constexpr auto operator*(V &&v, M &&m) noexcept
		{
			vector<std::common_type_t<vector_value_type<V>, matrix_value_type<M>>,
				   matrix_cols<M>, max(vector_alignment<V>, matrix_alignment<M>)> out_vec;

			for(std::size_t i = 0; i < matrix_cols<M>; i++)
			{
				for(std::size_t j = 0; j < vector_dimension<V>; j++)
					out_vec += v[j] * m[j][i];
			}

			return out_vec;
		}

		/**
		 * @brief shrink_matrix
		 * @tparam ROWS_OFFSET offset within matrix rows dimension
		 * @tparam COLS_OFFSET offset within matrix columns dimension
		 * @tparam ROWS rows parameter of a new matrix
		 * @tparam COLS columns parameter of a new matrix
		 * @tparam ALIGNMENT aliognment parameter of a new matrix
		 * @tparam M must satisfy the matrix_concept concept
		 * @param mat matrix to shrink
		 */
		template<std::size_t ROWS_OFFSET,
				 std::size_t COLS_OFFSET,
				 std::size_t ROWS,
				 std::size_t COLS,
				 std::size_t ALIGNMENT,
				 matrix_concept M>
			requires
				(ROWS > 0 && COLS > 0) &&
				(matrix_rows<M> >= ROWS_OFFSET + ROWS) &&
				(matrix_cols<M> >= COLS_OFFSET + COLS)
		constexpr auto shrink_matrix(M &&mat) noexcept
		{
			matrix<matrix_value_type<M>, ROWS, COLS, ALIGNMENT> out_mat;

			for(std::size_t i = ROWS_OFFSET; i < ROWS_OFFSET + ROWS; i++)
				for(std::size_t j = COLS_OFFSET; j < COLS_OFFSET + COLS; j++)
					out_mat[i - ROWS_OFFSET][j - COLS_OFFSET] = std::forward<M>(mat)[i][j];

			return out_mat;
		}

		/**
		 * @brief take_matrix_view
		 * @tparam ROWS_OFFSET offset within matrix rows dimension
		 * @tparam COLS_OFFSET offset within matrix columns dimension
		 * @tparam ROWS rows parameter of a new matrix_view
		 * @tparam COLS columns parameter of a new matrix_view
		 * @tparam M must satisfy the matrix_concept concept
		 * @param mat matrix to view
		 */
		template<std::size_t ROWS_OFFSET,
				 std::size_t COLS_OFFSET,
				 std::size_t ROWS,
				 std::size_t COLS,
				 matrix_concept M>
			requires
			(ROWS > 0 && COLS > 0) &&
			(matrix_rows<M> >= ROWS_OFFSET + ROWS) &&
			(matrix_cols<M> >= COLS_OFFSET + COLS)
		constexpr auto take_matrix_view(M &&mat) noexcept
		{
			using view_type = matrix_view<matrix_value_type<M>, ROWS, COLS>;
			std::size_t elem_stride = 0;
			if constexpr(matrix_alignment<M> > sizeof(matrix_row_type<M>))
				elem_stride = matrix_alignment<M> / sizeof(matrix_value_type<M>);
			else
				elem_stride = sizeof(matrix_row_type<M>) / sizeof(matrix_value_type<M>);

			matrix_row_type<view_type> row(&std::forward<M>(mat)[ROWS_OFFSET][COLS_OFFSET]);

			return view_type(row, elem_stride);
		}

		namespace glsl
		{
			inline namespace scalar
			{
				using mat2x2 = matrix<float, 2, 2>;
				using mat2x3 = matrix<float, 2, 3>;
				using mat2x4 = matrix<float, 2, 4>;
				using mat3x2 = matrix<float, 3, 2>;
				using mat3x3 = matrix<float, 3, 3>;
				using mat3x4 = matrix<float, 3, 4>;
				using mat4x2 = matrix<float, 4, 2>;
				using mat4x3 = matrix<float, 4, 3>;
				using mat4x4 = matrix<float, 4, 4>;

				using dmat2x2 = matrix<double, 2, 2>;
				using dmat2x3 = matrix<double, 2, 3>;
				using dmat2x4 = matrix<double, 2, 4>;
				using dmat3x2 = matrix<double, 3, 2>;
				using dmat3x3 = matrix<double, 3, 3>;
				using dmat3x4 = matrix<double, 3, 4>;
				using dmat4x2 = matrix<double, 4, 2>;
				using dmat4x3 = matrix<double, 4, 3>;
				using dmat4x4 = matrix<double, 4, 4>;
			};

			//assume that alignof(float) = sizeof(float) = 4;
			//assume that alignof(double) = sizeof(double) = 8;
			namespace std140
			{
				using mat2x2 = matrix<float, 2, 2, alignof(float) * 4>;
				using mat2x3 = matrix<float, 2, 3, alignof(float) * 4>;
				using mat2x4 = matrix<float, 2, 4, alignof(float) * 4>;
				using mat3x2 = matrix<float, 3, 2, alignof(float) * 4>;
				using mat3x3 = matrix<float, 3, 3, alignof(float) * 4>;
				using mat3x4 = matrix<float, 3, 4, alignof(float) * 4>;
				using mat4x2 = matrix<float, 4, 2, alignof(float) * 4>;
				using mat4x3 = matrix<float, 4, 3, alignof(float) * 4>;
				using mat4x4 = matrix<float, 4, 4, alignof(float) * 4>;

				using dmat2x2 = matrix<double, 2, 2, alignof(float) * 4>;
				using dmat2x3 = matrix<double, 2, 3, alignof(float) * 4>;
				using dmat2x4 = matrix<double, 2, 4, alignof(float) * 4>;
				using dmat3x2 = matrix<double, 3, 2, alignof(float) * 8>;
				using dmat3x3 = matrix<double, 3, 3, alignof(float) * 8>;
				using dmat3x4 = matrix<double, 3, 4, alignof(float) * 8>;
				using dmat4x2 = matrix<double, 4, 2, alignof(float) * 8>;
				using dmat4x3 = matrix<double, 4, 3, alignof(float) * 8>;
				using dmat4x4 = matrix<double, 4, 4, alignof(float) * 8>;
			};

			namespace std430
			{
				using mat2x2 = matrix<float, 2, 2, alignof(float) * 2>;
				using mat2x3 = matrix<float, 2, 3, alignof(float) * 2>;
				using mat2x4 = matrix<float, 2, 4, alignof(float) * 2>;
				using mat3x2 = matrix<float, 3, 2, alignof(float) * 4>;
				using mat3x3 = matrix<float, 3, 3, alignof(float) * 4>;
				using mat3x4 = matrix<float, 3, 4, alignof(float) * 4>;
				using mat4x2 = matrix<float, 4, 2, alignof(float) * 4>;
				using mat4x3 = matrix<float, 4, 3, alignof(float) * 4>;
				using mat4x4 = matrix<float, 4, 4, alignof(float) * 4>;

				using dmat2x2 = matrix<double, 2, 2, alignof(double) * 2>;
				using dmat2x3 = matrix<double, 2, 3, alignof(double) * 2>;
				using dmat2x4 = matrix<double, 2, 4, alignof(double) * 2>;
				using dmat3x2 = matrix<double, 3, 2, alignof(double) * 4>;
				using dmat3x3 = matrix<double, 3, 3, alignof(double) * 4>;
				using dmat3x4 = matrix<double, 3, 4, alignof(double) * 4>;
				using dmat4x2 = matrix<double, 4, 2, alignof(double) * 4>;
				using dmat4x3 = matrix<double, 4, 3, alignof(double) * 4>;
				using dmat4x4 = matrix<double, 4, 4, alignof(double) * 4>;
			};
		};
	};
};
