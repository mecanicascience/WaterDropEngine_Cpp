#pragma once

#include "Vector3.hpp"
#include "Quaternion.hpp"

namespace wde::physics {
	/**
	 * Holds a 4D Matrix
	 */
	class Matrix4 {
		protected:
			/** Matrix data */
			double _data[12] {};

		public:
			/** Creates a new identity matrix 4x4 */
			Matrix4() {
				_data[1] = _data[2] = _data[3] = _data[4] = _data[6] = _data[7] = _data[8] = _data[9] = _data[11] = 0;
				_data[0] = _data[5] = _data[10] = 1;
			}


			// Math operations
			/**
			 * Sets the matrix to be the inverse of the given matrix.
			 * @param m The matrix to invert and use to set this.
			 */
			void setInverse(const Matrix4 &m) {
				double det = getDeterminant();
				if (det == 0) // Not invertible
					return;
				det = ((double) 1.0) / det;

				_data[0] = (-m._data[9]*m._data[6]+m._data[5]*m._data[10])*det;
				_data[4] = (m._data[8]*m._data[6]-m._data[4]*m._data[10])*det;
				_data[8] = (-m._data[8]*m._data[5]+m._data[4]*m._data[9])*det;

				_data[1] = (m._data[9]*m._data[2]-m._data[1]*m._data[10])*det;
				_data[5] = (-m._data[8]*m._data[2]+m._data[0]*m._data[10])*det;
				_data[9] = (m._data[8]*m._data[1]-m._data[0]*m._data[9])*det;

				_data[2] = (-m._data[5]*m._data[2]+m._data[1]*m._data[6])*det;
				_data[6] = (+m._data[4]*m._data[2]-m._data[0]*m._data[6])*det;
				_data[10] = (-m._data[4]*m._data[1]+m._data[0]*m._data[5])*det;

				_data[3] = (m._data[9]*m._data[6]*m._data[3]
				            -m._data[5]*m._data[10]*m._data[3]
				            -m._data[9]*m._data[2]*m._data[7]
				            +m._data[1]*m._data[10]*m._data[7]
				            +m._data[5]*m._data[2]*m._data[11]
				            -m._data[1]*m._data[6]*m._data[11])*det;
				_data[7] = (-m._data[8]*m._data[6]*m._data[3]
				            +m._data[4]*m._data[10]*m._data[3]
				            +m._data[8]*m._data[2]*m._data[7]
				            -m._data[0]*m._data[10]*m._data[7]
				            -m._data[4]*m._data[2]*m._data[11]
				            +m._data[0]*m._data[6]*m._data[11])*det;
				_data[11] =(m._data[8]*m._data[5]*m._data[3]
				            -m._data[4]*m._data[9]*m._data[3]
				            -m._data[8]*m._data[1]*m._data[7]
				            +m._data[0]*m._data[9]*m._data[7]
				            +m._data[4]*m._data[1]*m._data[11]
				            -m._data[0]*m._data[5]*m._data[11])*det;
			}

			/** @return a new matrix containing the inverse of this matrix. */
			Matrix4 inverse() const {
				Matrix4 result;
				result.setInverse(*this);
				return result;
			}

			/** Inverts the matrix */
			void invert() { setInverse(*this); }

			/**
			 * Transform the given direction vector by this matrix, without reading W value (local -> world coordinates).
			 * @note Using this method supposes that this matrix represents a scale and shear transformation matrix.
			 * @param vector The vector to transform.
			 */
			Vector3 transformDirection(const Vector3 &vector) const {
				return {
					vector.x * _data[0] +
					vector.y * _data[1] +
					vector.z * _data[2],

					vector.x * _data[4] +
					vector.y * _data[5] +
					vector.z * _data[6],

					vector.x * _data[8] +
					vector.y * _data[9] +
					vector.z * _data[10]
				};
			}

			/**
			 * Transform the given direction vector by the inverse of this matrix, without reading W value (world -> local coordinates).
			 * @note Using this method supposes that this matrix represents a scale and shear transformation matrix.
			 * @param vector The vector to transform.
			 */
			Vector3 transformInverseDirection(const Vector3 &vector) const {
				// Inverse = Transpose
				return {
						vector.x * _data[0] +
						vector.y * _data[4] +
						vector.z * _data[8],

						vector.x * _data[1] +
						vector.y * _data[5] +
						vector.z * _data[9],

						vector.x * _data[2] +
						vector.y * _data[6] +
						vector.z * _data[10]
				};
			}

			/**
			 * Transform the given vector by the transformational inverse of this matrix, without reading W value (local -> world coordinates).
			 * @param vector The vector to transform.
			 */
			Vector3 transform(const Vector3 &vector) const {
				return (*this) * vector;
			}

			/**
			 * Transform the given vector by the transformational inverse of this matrix, without reading W value (world -> local coordinates).
			 * @note Using this method supposes that this matrix represents a scale and shear transformation matrix.
			 * @param vector The vector to transform.
			 */
			Vector3 transformInverse(const Vector3 &vector) const {
				Vector3 tmp = vector;
				tmp.x -= _data[3];
				tmp.y -= _data[7];
				tmp.z -= _data[11];
				return {
						tmp.x * _data[0] +
						tmp.y * _data[4] +
						tmp.z * _data[8],

						tmp.x * _data[1] +
						tmp.y * _data[5] +
						tmp.z * _data[9],

						tmp.x * _data[2] +
						tmp.y * _data[6] +
						tmp.z * _data[10]
				};
			}



			// Operators
			Matrix4 operator*(const Matrix4 &o) const {
				Matrix4 result;
				result._data[0] = (o._data[0]*_data[0]) + (o._data[4]*_data[1]) + (o._data[8]*_data[2]);
				result._data[4] = (o._data[0]*_data[4]) + (o._data[4]*_data[5]) + (o._data[8]*_data[6]);
				result._data[8] = (o._data[0]*_data[8]) + (o._data[4]*_data[9]) + (o._data[8]*_data[10]);

				result._data[1] = (o._data[1]*_data[0]) + (o._data[5]*_data[1]) + (o._data[9]*_data[2]);
				result._data[5] = (o._data[1]*_data[4]) + (o._data[5]*_data[5]) + (o._data[9]*_data[6]);
				result._data[9] = (o._data[1]*_data[8]) + (o._data[5]*_data[9]) + (o._data[9]*_data[10]);

				result._data[2] = (o._data[2]*_data[0]) + (o._data[6]*_data[1]) + (o._data[10]*_data[2]);
				result._data[6] = (o._data[2]*_data[4]) + (o._data[6]*_data[5]) + (o._data[10]*_data[6]);
				result._data[10] = (o._data[2]*_data[8]) + (o._data[6]*_data[9]) + (o._data[10]*_data[10]);

				result._data[3] = (o._data[3]*_data[0]) + (o._data[7]*_data[1]) + (o._data[11]*_data[2]) + _data[3];
				result._data[7] = (o._data[3]*_data[4]) + (o._data[7]*_data[5]) + (o._data[11]*_data[6]) + _data[7];
				result._data[11] = (o._data[3]*_data[8]) + (o._data[7]*_data[9]) + (o._data[11]*_data[10]) + _data[11];
				return result;
			}

			Vector3 operator*(const Vector3 &vector) const {
				return {
						vector.x * _data[0] +
						vector.y * _data[1] +
						vector.z * _data[2] + _data[3],

						vector.x * _data[4] +
						vector.y * _data[5] +
						vector.z * _data[6] + _data[7],

						vector.x * _data[8] +
						vector.y * _data[9] +
						vector.z * _data[10] + _data[11]
				};
			}


			// Getters and setters
			/** @return The matrix determinant */
			double getDeterminant() const {
				return -_data[8]*_data[5]*_data[2]+
						_data[4]*_data[9]*_data[2]+
						_data[8]*_data[1]*_data[6]-
						_data[0]*_data[9]*_data[6]-
						_data[4]*_data[1]*_data[10]+
						_data[0]*_data[5]*_data[10];
			}

			/**
			 * Sets this matrix to be the rotation matrix corresponding to the given quaternion
			 * @param q The quaternion corresponding to the new rotation.
			 * @param pos The new position.
			 */
			void setOrientationAndPos(const Quaternion &q, const Vector3 &pos) {
				_data[0] = 1 - (2*q.j*q.j + 2*q.k*q.k);
				_data[1] = 2*q.i*q.j + 2*q.k*q.r;
				_data[2] = 2*q.i*q.k - 2*q.j*q.r;
				_data[3] = pos.x;

				_data[4] = 2*q.i*q.j - 2*q.k*q.r;
				_data[5] = 1 - (2*q.i*q.i  + 2*q.k*q.k);
				_data[6] = 2*q.j*q.k + 2*q.i*q.r;
				_data[7] = pos.y;

				_data[8] = 2*q.i*q.k + 2*q.j*q.r;
				_data[9] = 2*q.j*q.k - 2*q.i*q.r;
				_data[10] = 1 - (2*q.i*q.i  + 2*q.j*q.j);
				_data[11] = pos.z;
			}

			/**
			 * @param The index of an axis (X = 0, Y = 1, Z = 2)
			 * @return The vector representing the axis
			 */
			Vector3 getAxisVector(int i) const {
				return { _data[i], _data[i+4], _data[i+8] };
			}
	};
}
