/**
 * @file
 *
 * Represents the vector_view class
 */

#pragma once

#include "matrix_common.hpp"
#include "vector_view.hpp"

namespace hrs
{
	namespace math
	{
		/**
		 * @brief The matrix_view class
		 * @tparam T must satisfy the arithmetic concept
		 * @tparam _ROWS rows dimension of view
		 * @tparam _COLS columns dimension of view
		 *
		 * View is constructed as it's represented on scheme below:
		 *
		 * 1 2 3 4 5 # # #-+
		 * 1 2 * * 5 # # # |--------+
		 * 1 2 * * 5 # # # |\       |\
		 * 1 2 * * 5 # # # |-\------+ \
		 * 1 2 3 4 5 # # #-+  \        \__COLS
		 * |_______|     |     \__cols
		 * |   | |\      |
		 * |   |_| \_rows|
		 * |	\        |
		 * |	 \__ROWS |
		 * |_____________|
		 *	    stride
		 *
		 * mat: T = float, rows -> 4; cols -> 5; alignment -> 32(alignof(float) * 8)
		 *
		 * stride = 8el -> same as alignment
		 */
		template<arithmetic T, std::size_t _ROWS, std::size_t _COLS>
		struct matrix_view
		{
			constexpr static std::size_t ROWS = _ROWS;///<count of rows
			constexpr static std::size_t COLS = _COLS;///<count of columns
			constexpr static std::size_t ALIGNMENT = alignof(T);///<alignment value
			using row_type = vector_view<T, COLS>;///<row type of inner data pointer

			/**
			 * @brief The mat_iterator class
			 *
			 * Used as iterator for matrix_view
			 */
			struct mat_iterator
			{
				std::size_t elem_stride;///<stride within parent matrix
				T *scalar_ptr;///<pointer to the firts element of the target row

				/**
				 * @brief mat_iterator
				 * @param _scalar_ptr pointer to the first element of row
				 * @param _elem_stride stride within the parent matrix object
				 */
				constexpr mat_iterator(T *_scalar_ptr = nullptr,
									   std::size_t _elem_stride = 0) noexcept
					: scalar_ptr(_scalar_ptr),
					  elem_stride(_elem_stride) {}

				~mat_iterator() = default;
				mat_iterator(const mat_iterator &) noexcept = default;
				mat_iterator(mat_iterator &&) noexcept = default;
				mat_iterator & operator=(const mat_iterator &) noexcept = default;
				mat_iterator & operator=(mat_iterator &&) noexcept = default;

				bool operator==(const mat_iterator &) const noexcept = default;

				/**
				 * @brief operator ++
				 * @return reference to target iterator
				 *
				 * Simply adds elem_stride to the inner pointer
				 */
				constexpr mat_iterator & operator++() noexcept
				{
					scalar_ptr = scalar_ptr+ elem_stride;
					return *this;
				}

				/**
				 * @brief operator ++
				 * @return copy iterator of this iterator before increment
				 *
				 * Simply adds elem_stride to the inner pointer
				 */
				constexpr mat_iterator operator++(int) noexcept
				{
					auto it = *this;
					scalar_ptr = scalar_ptr+ elem_stride;

					return it;
				}

				/**
				 * @brief operator*
				 * @return vector_view over the inner pointer
				 */
				auto operator*() noexcept
				{
					return vector_view<T, COLS>(scalar_ptr);
				}

				/**
				 * @brief operator*
				 * @return vector_view over the inner pointer
				 */
				auto operator*() const noexcept
				{
					return vector_view<const T, COLS>(scalar_ptr);
				}

#warning operator-> LOOKS VERY DANGEROUS CAUSE IT USES REINTERPRET_CAST!!! SEE SOMETHING ABOUT STANDARD LAYOUT!!!
				/**
				 * @brief operator->
				 * @return pointer to the vector_view over the inner pointer
				 */
				auto operator->() noexcept
				{
					return reinterpret_cast<const vector_view<T, COLS> *>(scalar_ptr);
				}

				/**
				 * @brief operator->
				 * @return pointer to the vector_view over the inner pointer
				 */
				auto operator->() const noexcept
				{
					return reinterpret_cast<const vector_view<const T, COLS> *>(scalar_ptr);
				}
			};

			using iterator = mat_iterator;///<iterator type

			row_type row_view;///<view over first row
			std::size_t elem_stride;///<stride within parent matrix object

			/**
			 * @brief matrix_view
			 * @param _row_view starting view over vector type
			 * @param _elem_stride elements stride witin parent object
			 */
			constexpr matrix_view(row_type _row_view = {}, std::size_t _elem_stride = 0) noexcept
				: row_view(_row_view), elem_stride(_elem_stride) {}

			~matrix_view() = default;

			/**
			 * @brief matrix_view
			 * @tparam M must satisfy the matrix_concept concept
			 * @param m matrix for taking a view
			 */
			template<matrix_concept M>
			constexpr matrix_view(M &&m) noexcept
			{
				if constexpr(matrix_view_concept<M>)
				{
					elem_stride = std::forward<M>(m).stride();
				}
				else
				{
					if constexpr(matrix_alignment<M> > sizeof(matrix_row_type<M>))
						elem_stride = matrix_alignment<M> / sizeof(matrix_value_type<M>);
					else
						elem_stride = sizeof(matrix_row_type<M>) / sizeof(matrix_value_type<M>);
				}

				row_view = std::forward<M>(m)[0];
			}

			/**
			 * @brief operator=
			 * @tparam M must satisfy the matrix_concept concept
			 * @param m matrix for taking a view
			 * @return reference to matrix view
			 */
			template<matrix_concept M>
			constexpr matrix_view & operator=(M &&m) noexcept
			{
				if constexpr(matrix_view_concept<M>)
				{
					elem_stride = std::forward<M>(m).stride();
				}
				else
				{
					if constexpr(matrix_alignment<M> > sizeof(matrix_row_type<M>))
						elem_stride = matrix_alignment<M> / sizeof(matrix_value_type<M>);
					else
						elem_stride = sizeof(matrix_row_type<M>) / sizeof(matrix_value_type<M>);
				}

				row_view = std::forward<M>(m)[0];

				return *this;
			}

			/**
			 * @brief transpose
			 * @tparam NEW_ALIGNMENT alignment of transposed matrix
			 * @return transposed matrix based on view
			 */
			template<std::size_t NEW_ALIGNMENT>
			constexpr auto transpose() const noexcept
			{
				matrix<T, COLS, ROWS, NEW_ALIGNMENT> out_mat;

				for(std::size_t i = 0; i < ROWS; i++)
					for(std::size_t j = 0; j < COLS; j++)
						out_mat[j][i] = (*this)[i][j];


				return out_mat;
			}

			/**
			 * @brief stride
			 * @return stride value within the parent object
			 */
			constexpr std::size_t stride() const noexcept
			{
				return elem_stride;
			}

			/**
			 * @brief operator []
			 * @param i index within the parent object inner array
			 * @return requested row at passed index
			 */
			constexpr row_type operator[](std::size_t i) noexcept
			{
				return {reinterpret_cast<T *>(&row_view[0]) + i * elem_stride};
			}

			/**
			 * @brief operator []
			 * @param i index within the parent object inner array
			 * @return requested row at passed index
			 */
			constexpr const row_type operator[](std::size_t i) const noexcept
			{
				return {reinterpret_cast<const T *>(&row_view[0]) + i * elem_stride};
			}

			/**
			 * @brief begin
			 * @return iterator to the start of the parent object
			 */
			constexpr iterator begin() noexcept
			{
				return iterator(&row_view[0], elem_stride);
			}

			/**
			 * @brief begin
			 * @return iterator to the start of the parent object
			 */
			constexpr iterator begin() const noexcept
			{
				return iterator(&row_view[0], elem_stride);
			}

			/**
			 * @brief end
			 * @return iterator to the end of the parent object
			 */
			constexpr iterator end() noexcept
			{
				return iterator(&(*this)[ROWS][0], elem_stride);
			}

			/**
			 * @brief end
			 * @return iterator to the end of the parent object
			 */
			constexpr iterator end() const noexcept
			{
				return iterator(&(*this)[ROWS][0], elem_stride);
			}
		};
	};
};
