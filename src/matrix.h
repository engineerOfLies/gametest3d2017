#ifndef __MATRIX_H__
#define __MATRIX_H__

typedef float Matrix3[9];

typedef float Matrix4[16];

/**
	@brief performs a dot product between these two matrices
	@param a the multiplier
	@param b the multiplicant
	@returns the dot product
*/
float matrix4_dot(
	Matrix4 a, /**<line item parameter descriptions*/
	Matrix4 b
	);
#endif
