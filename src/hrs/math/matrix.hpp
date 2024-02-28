/**
 * @file
 *
 * Represents the matrix class
 */

#pragma once

#include "matrix_common.hpp"
#include "vector.hpp"

namespace hrs
{
	namespace math
	{
		/**
		 * @brief The matrix class
		 * @tparam T must satisfy the arithmetic concept
		 * @tparam _ROWS count of rows
		 * @tparam _COLS count of columns
		 * @tparam _ALIGNMENT alignment of inner row-vectors
		 */
		template<arithmetic T, std::size_t _ROWS, std::size_t _COLS, std::size_t _ALIGNMENT>
			requires (_ROWS > 0 && _COLS > 0)
		struct matrix
		{
			constexpr static std::size_t ROWS = _ROWS;///<rows value
			constexpr static std::size_t COLS = _COLS;///<columns value
			constexpr static std::size_t ALIGNMENT = _ALIGNMENT;///<alignment value
			using row_type = vector<T, COLS, ALIGNMENT>;///<row type

			using iterator = row_type *;///<iterator type
			using const_iterator = const row_type *;///<constant iterator type

			row_type data[ROWS];///<data array

			constexpr matrix() = default;
			~matrix() = default;

			/**
			 * @brief matrix
			 * @tparam M must satisfy the matrix_concept concept
			 * @param mat matrix to assign
			 */
			template<matrix_concept M>
			constexpr matrix(M &&mat) noexcept
			{
				constexpr std::size_t min_rows = min(ROWS, matrix_rows<M>);
				for(std::size_t i = 0; i < min_rows; i++)
				{
					(*this)[i] = std::forward<M>(mat)[i];
				}
			}

			/**
			 * @brief matrix
			 * @tparam Rows must satisfy the vector_concept concept
			 * @param rows vector objects to assign to the inner data
			 */
			template<vector_concept ...Rows>
			constexpr matrix(Rows &&...rows) noexcept : data(std::forward<Rows>(rows)...) {}

			/**
			 * @brief operator =
			 * @tparam M must satisfy the matrix_concept concept
			 * @param mat matrix to assign
			 * @return reference to this matrix
			 */
			template<matrix_concept M>
			constexpr matrix & operator=(M &&mat) noexcept
			{
				constexpr std::size_t min_rows = min(ROWS, matrix_rows<M>);
				for(std::size_t i = 0; i < min_rows; i++)
				{
					(*this)[i] = std::forward<M>(mat)[i];
				}
				return *this;
			}

			/**
			 * @brief identity
			 * @param value value which is being set to the elements at the equal indices of rows and columns
			 * @return identity matrix with main diagonal filled with value
			 */
			constexpr matrix static identity(T value = 1) noexcept
				requires (ROWS == COLS)
			{
				matrix mat;
				for(std::size_t i = 0; i < ROWS; i++)
					mat[i][i] = value;

				return mat;
			}

			/**
			 * @brief transpose
			 * @tparam NEW_ALIGNMENT alignment parameter of new matrix
			 * @return transposed matrix
			 */
			template<std::size_t NEW_ALIGNMENT = ALIGNMENT>
			constexpr auto transpose() const noexcept
			{
				matrix<T, COLS, ROWS, NEW_ALIGNMENT> out_mat;

				for(std::size_t i = 0; i < ROWS; i++)
					for(std::size_t j = 0; j < COLS; j++)
						out_mat[j][i] = (*this)[i][j];


				return out_mat;
			}

			/**
			 * @brief operator []
			 * @param i index of the requested row within the inner data array
			 * @return reference to the requested row
			 *
			 * Used for non-constant objects
			 */
			constexpr row_type & operator[](std::size_t i) noexcept
			{
				return data[i];
			}

			/**
			 * @brief operator []
			 * @param i index of the requested row within the inner data array
			 * @return reference to the requested row
			 *
			 * Used for constant objects
			 */
			constexpr const row_type & operator[](std::size_t i) const noexcept
			{
				return data[i];
			}

			/**
			 * @brief begin
			 * @return iterator to the start of the inner data
			 *
			 * Used for non-constant objects
			 */
			constexpr iterator begin() noexcept
			{
				return data;
			}

			/**
			 * @brief begin
			 * @return iterator to the start of the inner data
			 *
			 * Used for constant objects
			 */
			constexpr const_iterator begin() const noexcept
			{
				return data;
			}

			/**
			 * @brief end
			 * @return iterator to the end of the inner data
			 *
			 * Used for non-constant objects
			 */
			constexpr iterator end() noexcept
			{
				return data + ROWS;
			}

			/**
			 * @brief end
			 * @return iterator to the end of the inner data
			 *
			 * Used for constant objects
			 */
			constexpr const_iterator end() const noexcept
			{
				return data + ROWS;
			}
		};

		/**
		 * @brief matrix
		 * @tparam Rows must satisfy the vector_concept concept
		 *
		 * Deduction guide for matrix.
		 * Deduces matrix type as common type of valye type of rows argument,
		 * rows count as max rows' dimension and alignment as max rows' alignment.
		 */
		template<vector_concept ...Rows>
		matrix(Rows &&...) -> matrix<std::common_type_t<vector_value_type<Rows>...>,
									 sizeof...(Rows),
									 max(vector_dimension<Rows>...),
									 max(vector_alignment<Rows>...)>;
	};
};
