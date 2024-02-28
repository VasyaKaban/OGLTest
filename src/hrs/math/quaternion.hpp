/**
 * @file
 *
 * Represent the quaternion class
 */

#pragma once

#include "math_common.hpp"
#include "vector_common.hpp"
#include "matrix_common.hpp"
#include <cmath>

namespace hrs
{
	namespace math
	{
		/**
		 * @brief The quaternion class
		 * @tparam T must satisfy the floating_point concept
		 *
		 * @warning Assume we work with target coordinate system:
		 * x - right, y - up, z - forward
			  y
			  |	     z
			  |	    /
			  |    /
			  |   /
			  |  /
			  | /
			  |/
			  +--------------x
			All rotate operations will be performed with respect of this coordinate system
		 */
		template<std::floating_point T>
		struct quaternion
		{
			T x;///<x component of a quaternion
			T y;///<y component of a quaternion
			T z;///<z component of a quaternion
			T w;///<w component of a quaternion

			/**
			 * @brief quaternion
			 * @param _x x component
			 * @param _y y component
			 * @param _z z component
			 * @param _w w component
			 */
			constexpr quaternion(T _x = 0, T _y = 0, T _z = 0, T _w = 1) noexcept
				: x(_x), y(_y), z(_z), w(_w) {}
			~quaternion() = default;

			/**
			 * @brief quaternion
			 * @tparam V must satisfy the vector_concept concept
			 * @param v vector of quaternion parameters
			 *
			 * Simply assignes elemnts of vector from 0, 1, 2 and 3 indices.
			 * If vector dimension is less than requested component index than these components
			 * will be filled with zeroes(components x, y and z) and w will be filled with 1.
			 * Mapping for components is next: x - 0, y - 1, z - 2, w - 3
			 */
			template<vector_concept V>
			constexpr quaternion(V &&v) noexcept
			{
				x = std::forward<V>(v)[0];
				if constexpr(vector_dimension<V> > 3)//4 and more
				{
					y = std::forward<V>(v)[1];
					z = std::forward<V>(v)[2];
					w = std::forward<V>(v)[3];
				}
				else if(vector_dimension<V> == 3)//3
				{
					y = std::forward<V>(v)[1];
					z = std::forward<V>(v)[2];
					w = 0;
				}
				else if(vector_dimension<V> == 2)//2
				{
					y = std::forward<V>(v)[1];
					z = 0;
					w = 0;
				}
			}

			/**
			 * @brief quaternion
			 * @tparam V must satisfy the vector_concept concept
			 * @param a vector-axis
			 * @param angle angle value in radians
			 *
			 * Creates a quaternion based on axis and angle
			 */
			template<vector_concept V>
			constexpr quaternion(V &&a, T angle) noexcept
			{
				T half_sin = std::sin(angle / 2);
				T half_cos = std::cos(angle / 2);
				w = half_cos;
				x = std::forward<V>(a)[0] * half_sin;
				if constexpr(vector_dimension<V> > 2)//3 and more
				{
					y = std::forward<V>(a)[1] * half_sin;
					z = std::forward<V>(a)[2] * half_sin;
				}
				else if(vector_dimension<V> == 2)//2
				{
					y = std::forward<V>(a)[1] * half_sin;
					z = 0;
				}
			}

			/**
			 * @brief quaternion
			 * @tparam M nust satisfy the natrix_concept concept
			 * @param m matrix which is used in matrrix-to-quaternion transformation
			 * @warning This function assumes that matrix rows are already normalized!
			 *
			 * Transforms matrix into quaternion via next formulas:@n
			 * \f$ q_{w} = \frac{\sqrt{a_{0,0} + a_{1,1} + a_{2,2} + 1}}{2} \f$@n
			 * \f$ q_{x} = \sqrt{\frac{a_{0,0} + a_{2,1} - a_{1,2} + 1}{2}} - q_{w} \f$@n
			 * \f$ q_{y} = \sqrt{\frac{a_{1,1} + a_{0,2} - a_{2,0} + 1}{2}} - q_{w} \f$@n
			 * \f$ q_{z} = \sqrt{\frac{a_{2,2} + a_{1,0} - a_{0,1} + 1}{2}} - q_{w} \f$@n
			 */
			template<matrix_concept M>
				requires (matrix_rows<M> >= 3 && matrix_cols<M> >= 3)
			constexpr quaternion(M &&m) noexcept
			{
				w = std::sqrt(static_cast<T>(std::forward<M>(m)[0][0] +
											 std::forward<M>(m)[1][1] +
											 std::forward<M>(m)[2][2]) + static_cast<T>(1.0)) /
					static_cast<T>(2.0);

				x = std::sqrt((static_cast<T>(std::forward<M>(m)[0][0] +
											  std::forward<M>(m)[2][1] -
											  std::forward<M>(m)[1][2]) +
							   static_cast<T>(1.0)) /
							  static_cast<T>(2.0)) - w;

				y = std::sqrt((static_cast<T>(std::forward<M>(m)[1][1] +
											  std::forward<M>(m)[0][2] -
											  std::forward<M>(m)[2][0]) +
							   static_cast<T>(1.0)) /
							  static_cast<T>(2.0)) - w;

				z = std::sqrt((static_cast<T>(std::forward<M>(m)[2][2] +
											  std::forward<M>(m)[1][0] -
											  std::forward<M>(m)[0][1]) +
							   static_cast<T>(1.0)) /
							  static_cast<T>(2.0)) - w;
			}

			/**
			 * @brief quaternion
			 * @tparam F must satisfy the floating_point concept
			 * @param q quaternion to assign
			 */
			template<std::floating_point F>
			constexpr quaternion(const quaternion<F> &q) noexcept
				: x(q.x), y(q.y), z(q.z), w(q.w) {}

			/**
			 * @brief operator =
			 * @tparam F must satisfy the floating_point concept
			 * @param q quaternion to assign
			 * @return target quaternion
			 */
			template<std::floating_point F>
			constexpr quaternion & operator=(const quaternion<F> &q) noexcept
			{
				x = q.x;
				y = q.y;
				z = q.z;
				w = q.w;

				return *this;
			}

			/**
			 * @brief operator =
			 * @tparam V must satisfy the vector_concept concept
			 * @param v vector of quaternion parameters
			 * @return reference to quternion afteer assignment operaion
			 *
			 * Simply assignes elemnts of vector from 0, 1, 2 and 3 indices.
			 * If vector dimension is less than requested component index than these components
			 * will be filled with zeroes(components x, y and z) and w will be filled with 1.
			 * Mapping for components is next: x - 0, y - 1, z - 2, w - 3
			 */
			template<vector_concept V>
			constexpr quaternion & operator=(V &&v) noexcept
			{
				x = std::forward<V>(v)[0];
				if constexpr(vector_dimension<V> > 3)//4 and more
				{
					y = std::forward<V>(v)[1];
					z = std::forward<V>(v)[2];
					w = std::forward<V>(v)[3];
				}
				else if(vector_dimension<V> == 3)//3
				{
					y = std::forward<V>(v)[1];
					z = std::forward<V>(v)[2];
					w = 1;
				}
				else if(vector_dimension<V> == 2)//2
				{
					y = std::forward<V>(v)[1];
					z = 0;
					w = 1;
				}
				return *this;
			}

			/**
			 * @brief length
			 * @param eps bias for normalized length optimization
			 * @return length pf quaternion
			 */
			constexpr T length(T eps = default_eps) const noexcept
			{
				T dot_prod = x * x + y * y + z * z + w * w;
				if(is_near(dot_prod, static_cast<T>(1.0f), eps))
					return dot_prod;

				return std::sqrt(dot_prod);
			}

			/**
			 * @brief normalize
			 * @param eps bias for normalized length optimization
			 * @return normalized quaternion
			 */
			constexpr quaternion normalize(T eps = default_eps) const noexcept
			{
				quaternion out_q = *this;
				auto inv_len = static_cast<T>(1.0) / length(eps);
				out_q.x * inv_len;
				out_q.y * inv_len;
				out_q.z * inv_len;
				out_q.w * inv_len;
				return out_q;
			}

			/**
			 * @brief get_half_cos
			 * @return w component
			 * @warning This function assumes that the quaternion is already normalized!
			 */
			constexpr T get_half_cos() const noexcept
			{
				return w;
			}

			/**
			 * @brief get_half_sin
			 * @return half sine of quaternion
			 * @warning This function assumes that the quaternion is already normalized!
			 *
			 * Simply uses the next formula:
			 * \f$
			 *	sin\frac{\alpha}{2} = \sqrt{1 - w^{2}}
			 * \f$
			 */
			constexpr T get_half_sin() const noexcept
			{
				return std::sqrt(static_cast<T>(1.0) - std::pow(get_half_cos(), 2));
			}

			/**
			 * @brief conjugate
			 * @return conjugated quaternion
			 *
			 * Simply changes sign of x, y, z components
			 */
			constexpr quaternion conjugate() const noexcept
			{
				return quaternion(-x, -y, -z, w);
			}

			/**
			 * @brief operator~
			 * @return conjugated quaternion
			 *
			 * Same as conjugate function
			 */
			constexpr quaternion operator~() const noexcept
			{
				return conjugate();
			}

			/**
			 * @brief operator *=
			 * @tparam F must satisfy the floating_point concept
			 * @param q quaternion for multiplication operation
			 * @return the result quaternion from quaternion-by-quaternion multiplication
			 *
			 * Same as operator*
			 */
			template<std::floating_point F>
			constexpr quaternion & operator*=(const quaternion<F> &q) noexcept
			{
				const auto tmp_q = *this;
				x = tmp_q.y * q.z - q.y * tmp_q.z + tmp_q.w * q.x + q.w * tmp_q.x;
				y = tmp_q.z * q.x - q.z * tmp_q.x + tmp_q.w * q.y + q.w * tmp_q.y;
				z = tmp_q.x * q.y - q.x * tmp_q.y + tmp_q.w * q.z + q.w * tmp_q.z;
				w = tmp_q.w * q.w - (tmp_q.x * q.x + tmp_q.y * q.y + tmp_q.z * q.z);

				return *this;
			}

			/**
			 * @brief operator *
			 * @param q quaternion for multiplication operation
			 * @return the result quaternion from quaternion-by-quaternion multiplication
			 *
			 * Uses formula that is described below:@n
			 *
			 * \f$
			 *	a \ and \ b \ - quaternions \\
			 *	ab = (a_{\vec{v}}, a_{w})(b_{\vec{v}}, b_{w}) =
			 *	a_{\vec{v}} \times b_{\vec{v}} +
			 *	b_{w}a_{\vec{v}} + a_{w}b_{\vec{v}} +
			 *	a_{w}b_{w} - a_{\vec{v}} \cdot b_{\vec{v}}
			 * \f$
			 *
			 */
			template<std::floating_point F>
			constexpr auto operator*(const quaternion<F> &q) const noexcept
			{
				quaternion<std::common_type_t<T, F>> out_q;
				out_q.x = y * q.z - q.y * z + w * q.x + q.w * x;
				out_q.y = z * q.x - q.z * x + w * q.y + q.w * y;
				out_q.z = x * q.y - q.x * y + w * q.z + q.w * z;
				out_q.w = w * q.w - (x * q.x + y * q.y + z * q.z);
				return out_q;
			}

			/**
			 * @brief vector_part
			 * @tparam ALIGNMENT alignment of out vector
			 * @return vector part of quaternion as a 3-component vector
			 */
			template<std::size_t ALIGNMENT = alignof(T)>
			constexpr auto vector_part() const noexcept
			{
				return vector<T, 3, ALIGNMENT>(x, y, z);
			}

			/**
			 * @brief to_matrix
			 * @tparam ALIGNMENT alignment of out matrix
			 * @return rotate matrix based on quaternion
			 *
			 * Makes rotate matrix based on next formula:@n
			 * \f$
			 * \begin{pmatrix}
			 *	2(q^{2}_{w} + q^{2}_x) - 1 & 2(q_{x}q_{y} - q_{z}q_{w}) & 2(q_{x}q_{z} + q_{y}q_{w}) \\
			 *	2(q_{x}q_{y} + q_{z}q_{w}) & 2(q^{2}_{w} + q^{2}_y) - 1 & 2(q_{y}q_{z} - q_{x}q_{w})\\
			 *	2(q_{x}q_{z} - q_{y}q_{w}) & 2(q_{y}q_{z} + q_{x}q_{w}) & 2(q^{2}_{w} + q^{2}_z) - 1
			 * \end{pmatrix}
			 * \f$
			 *
			 * @warning This function assumes that the quaternion is already normalized!
			 */
			template<std::size_t ALIGNMENT = alignof(T)>
			constexpr auto to_matrix() const noexcept
			{
				const T w_pow_2 = w * w;
				return matrix<T, 3, 3, ALIGNMENT>
				{
					hrs::math::vector{static_cast<T>(2.0) * (w_pow_2 + x * x) - static_cast<T>(1.0),
					static_cast<T>(2.0) * (x * y - z * w),
					static_cast<T>(2.0) * (x * z + y * w)},

					hrs::math::vector{static_cast<T>(2.0) * (x * y + z * w),
					static_cast<T>(2.0) * (w_pow_2 + y * y) - static_cast<T>(1.0),
					static_cast<T>(2.0) * (y * z - x * w)},

					hrs::math::vector{static_cast<T>(2.0) * (x * z - y * w),
					static_cast<T>(2.0) * (y * z + x * w),
					static_cast<T>(2.0) * (w_pow_2 + z * z) - static_cast<T>(1.0)}
				};
			}

			/**
			 * @brief rotate_vector
			 * @tparam V must satisfy the vector_concept concept
			 * @tparam ALIGNMENT out vector alignment parameter
			 * @param v vector to rotate
			 * @return rotated vector based on quaternion and passed vector
			 *
			 * Uses Olinde Rodrigues formula to rotate a vector with a quaternion:
			 * Original axis-angle based:
			 * \f$\vec{r} = \vec{v}cos\alpha + (\vec{v} \times \vec{w})sin\alpha +
			 * \vec{w}(\vec{w} \cdot \vec{v})( 1-cos\alpha )\f$
			 *
			 * Quaternion form:
			 * \f$q^{-1}\vec{v}q = \vec{v}(2q^{2}_{w}-1) + 2q_{w}(\vec{v} \times q_{\vec{v}}) +
			 *  2q_{\vec{v}}(q_{\vec{v}} \cdot \vec{v})\f$
			 *
			 * @warning This function assumes that the quaternion is already normalized!
			 */
			template<vector_concept V, std::size_t ALIGNMENT = alignof(T)>
			constexpr auto rotate_vector(V &&v) const noexcept
			{
				vector<T, 3, ALIGNMENT> out_v = std::forward<V>(v);
				auto vec_q = vector_part();
				return out_v * (static_cast<T>(2.0) * w * w - static_cast<T>(1.0)) +
					   static_cast<T>(2.0) * w * (out_v ^ vec_q) +
					   static_cast<T>(2.0) * vec_q * (vec_q * out_v);
			}
		};

		/**
		 * @brief Deduction guide for quaternion class
		 * @tparam V must satisfy the vector_concept concept
		 *
		 * Deduces quaternion type based on passed vector type like
		 * common_type<vector_value_type, double>
		 */
		template<vector_concept V>
		quaternion(V &&) -> quaternion<std::common_type_t<vector_value_type<V>, double>>;
	};
};
