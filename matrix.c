/*
** Function definitions for vector and matrix manipulations
*/

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "matrix.h"


// Matrix Math
#define M(matrix, row, col) matrix[(col << 2) + row]

#define fsize sizeof(float)

MatrixP matrix_Print(Matrix in, const char *name) {
	printf("Matrix (%s):\n", name);
	for (int i=0; i<4; i++) {
		printf("\t[ %f %f %f %f ]\n", M(in,i,0), M(in,i,1), M(in,i,2), M(in,i,3));
	}
	printf("\n");
	fflush(stdout);
	return in;
}

/*
 * in = matrix to operate on
 */
MatrixP matrix_Identity(Matrix in) {
	memset(in, 0.0f, fsize*16);
	M(in,0,0) = 1.0f;
	M(in,1,1) = 1.0f;
	M(in,2,2) = 1.0f;
	M(in,3,3) = 1.0f;
	return in;
}

/*
 * out = matrix where result is stored
 * in = matrix to operate on
 */
MatrixP matrix_Transpose(Matrix out, Matrix in) {
	Matrix temp;

	// initialize the temp array
	memset(temp, 0, sizeof(temp));

	for (int row=0;row<4;row++) {
		for (int col=0;col<4;col++) {
			M(temp,row,col) = M(in,col,row);
		}
	}

	// copy the results to the result matrix
	memcpy(out, temp, sizeof(temp));
	return out;
}

/*
 * vec = the vector to operate on (write to)
 * in = the matrix data to set the vector with
 * row = row to set, starts at 0 (should be between 0 and 3)
 */
Vector2fP matrix_GetRowVec2(Vector2f vec, const Matrix in, const int row) {
	vec[0] = M(in, row, 0);
	vec[1] = M(in, row, 1);
	return vec;
}

/*
 * in = matrix to operate on
 * vec = the data to set the matrix with
 * row = row to set, starts at 0 (should be between 0 and 3)
 */
MatrixP matrix_SetRowVec2(Matrix in, const Vector2f vec, const int row) {
	// the first three cols are the same op regardless of row
	// though the third row should probably be 0,0,0,1 ...
	M(in, row, 0) = vec[0];
	M(in, row, 1) = vec[1];

	// if row 3, then final value must be 1.0
	if (row == 3) {
		M(in, row, 3) = 1.0f;
	}
	// otherwise, it should be 0.0
	else {
		M(in, row, 3) = 0.0f;
	}
	return in;
}

/*
 * vec = the vector to operate on (write to)
 * in = the matrix data to set the vector with
 * row = row to set, starts at 0 (should be between 0 and 3)
 */
Vector2fP matrix_GetColVec2(Vector2f vec, const Matrix in, const int col) {
	vec[0] = M(in, 0, col);
	vec[1] = M(in, 1, col);
	return vec;
}

/*
 * in = matrix to operate on
 * vec = the data to set the matrix with
 * row = row to set, starts at 0 (should be between 0 and 3)
 */
MatrixP matrix_SetColVec2(Matrix in, const Vector2f vec, const int col) {
	// the first three cols are the same op regardless of row
	// though the third row should probably be 0,0,0,1 ...
	M(in, 0, col) = vec[0];
	M(in, 1, col) = vec[1];

	// if row 3, then final value must be 1.0
	if (col == 3) {
		//M(in, row, 2) = 0.0f;
		M(in, 3, col) = 1.0f;
	}
	// otherwise, it should be 0.0
	else {
		//M(in, row, 2) = 0.0f;
		M(in, 3, col) = 0.0f;
	}
	return in;
}

/*
 * vec = the vector to operate on (write to)
 * in = the matrix data to set the vector with
 * row = row to set, starts at 0 (should be between 0 and 3)
 */
VectorP matrix_GetRow3v(Vector vec, const Matrix in, const int row) {
	vec[0] = M(in, row, 0);
	vec[1] = M(in, row, 1);
	vec[2] = M(in, row, 2);
	return vec;
}

/*
 * in = matrix to operate on
 * vec = the data to set the matrix with
 * row = row to set, starts at 0 (should be between 0 and 3)
 */
MatrixP matrix_SetRow3v(Matrix in, const Vector vec, const int row) {
	// the first three cols are the same op regardless of row
	// though the third row should probably be 0,0,0,1 ...
	M(in, row, 0) = vec[0];
	M(in, row, 1) = vec[1];
	M(in, row, 2) = vec[2];

	// if row 3, then final value must be 1.0
	if (row == 3) {
		M(in, row, 3) = 1.0f;
	}
	// otherwise, it should be 0.0
	else {
		M(in, row, 3) = 0.0f;
	}
	return in;
}

/*
 * vec = the vector to operate on (write to)
 * in = the matrix data to set the vector with
 * col = col to set, starts at 0 (should be between 0 and 3)
 */
VectorP matrix_GetCol3v(Vector vec, const Matrix in, const int col) {
	vec[0] = M(in, 0, col);
	vec[1] = M(in, 1, col);
	vec[2] = M(in, 2, col);
	return vec;
}

/*
 * in = matrix to operate on
 * vec = the data to set the matrix with
 * col = col to set, starts at 0 (should be between 0 and 3)
 */
MatrixP matrix_SetCol3v(Matrix in, const Vector vec, const int col) {
	// the first three rows are the same op regardless of col
	M(in, 0, col) = vec[0];
	M(in, 1, col) = vec[1];
	M(in, 2, col) = vec[2];

	// if col 3, then final value must be 1.0
	if (col == 3) {
		M(in, 3, col) = 1.0f;
	}
	// otherwise, it should be 0.0
	else {
		M(in, 3, col) = 0.0f;
	}
	return in;
}

/*
 * in = matrix to operate on
 */
MatrixP matrix_Normalize(Matrix in) {
	Vector x,y,z;

	matrix_GetRow3v(x, in, 0);
	matrix_GetRow3v(y, in, 1);
	matrix_GetRow3v(z, in, 2);
	/*
	vector_Normalize(x,x);
	vector_Normalize(y,y);
	vector_Normalize(z,z);
	*/
	vector_CrossProduct(x,y,z);
	vector_CrossProduct(y,z,x);
	vector_CrossProduct(z,x,y);
	/*
	vector_Normalize(x,x);
	vector_Normalize(y,y);
	vector_Normalize(z,z);
	*/
	matrix_SetRow3v(in, x, 0);
	matrix_SetRow3v(in, y, 1);
	matrix_SetRow3v(in, z, 2);

	/*
	float magnitude = 0;
	for (int col=0; col<=3; col++) {
		magnitude = sqrt((M(in,1,col)*M(in,1,col)) + (M(in,2,col)*M(in,2,col)) + (M(in,3,col)*M(in,3,col)) + (M(in,4,col)*M(in,4,col)));
		M(in,1,col) /= magnitude;
		M(in,2,col) /= magnitude;
		M(in,3,col) /= magnitude;
		M(in,4,col) /= magnitude;
	}
	*/
	return in;
}

/*
 * out = matrix where result is stored
 * a = left-hand matrix in the equation
 * b = right-hand matrix in the equation
 */
MatrixP matrix_Mult(Matrix out, Matrix a, Matrix b) {
	Matrix temp;

	// initialize the temp array
	memset(temp, 0, fsize*16);

	for (int row=0;row<4;row++) {
		M(temp,row,0) = M(a,row,0) * M(b,0,0) + M(a,row,1) * M(b,1,0) + M(a,row,2) * M(b,2,0) + M(a,row,3) * M(b,3,0);
		M(temp,row,1) = M(a,row,0) * M(b,0,1) + M(a,row,1) * M(b,1,1) + M(a,row,2) * M(b,2,1) + M(a,row,3) * M(b,3,1);
		M(temp,row,2) = M(a,row,0) * M(b,0,2) + M(a,row,1) * M(b,1,2) + M(a,row,2) * M(b,2,2) + M(a,row,3) * M(b,3,2);
		M(temp,row,3) = M(a,row,0) * M(b,0,3) + M(a,row,1) * M(b,1,3) + M(a,row,2) * M(b,2,3) + M(a,row,3) * M(b,3,3);
	}

	// copy the results to the result matrix
	memcpy(out, temp, fsize*16);
	return out;
}

/*
 * out = matrix where result is stored
 * in = matrix to multiply over
 * factor = the scalar to multiply the matrix by
 */
MatrixP matrix_ScalarMult(Matrix out, Matrix in, const float factor) {
	for (int row=0;row<4;row++) {
		M(out,row,0) = M(in,row,0) * factor;
		M(out,row,1) = M(in,row,1) * factor;
		M(out,row,2) = M(in,row,2) * factor;
		M(out,row,3) = M(in,row,3) * factor;
	}
	return out;
}

/*
 * out = matrix where result is stored
 * in = matrix to add to
 * factor = the scalar to add the matrix by
 */
MatrixP matrix_ScalarAdd(Matrix out, Matrix m, const float factor) {
	for (int row=0;row<4;row++) {
		M(out,row,0) = M(m,row,0) + factor;
		M(out,row,1) = M(m,row,1) + factor;
		M(out,row,2) = M(m,row,2) + factor;
		M(out,row,3) = M(m,row,3) + factor;
	}
	return out;
}

/*
 * out = matrix where result is stored
 * angle = angle of rotation (in radians)
 */
MatrixP matrix_RotateX(Matrix out, const float angle) {
	float s, c;

	matrix_Identity(out);
	//if (angle > 0 && angle < PI) {
		s = ((float)sin(angle)),
		c = ((float)cos(angle));

		M(out,1,1) = c;
		M(out,1,2) = s;
		M(out,2,1) = -s;
		M(out,2,2) = c;
	//}
	matrix_Normalize(out);
	return out;
}

/*
 * out = matrix where result is stored
 * angle = angle of rotation (in radians)
 */
MatrixP matrix_RotateY(Matrix out, const float angle) {
	float s, c;

	matrix_Identity(out);
	//if (angle > 0 && angle < PI) {
		s = ((float)sin(angle)),
		c = ((float)cos(angle));

		M(out,0,0) = c;
		M(out,0,2) = -s;
		M(out,2,0) = s;
		M(out,2,2) = c;
	//}
	matrix_Normalize(out);
	return out;
}

/*
 * out = matrix where result is stored
 * angle = angle of rotation (in radians)
 */
MatrixP matrix_RotateZ(Matrix out, const float angle) {
	float s, c;

	matrix_Identity(out);
	//if (angle > 0 && angle < PI) {
		s = ((float)sin(angle)),
		c = ((float)cos(angle));

		M(out,0,1) = c;
		M(out,0,1) = s;
		M(out,1,0) = -s;
		M(out,1,1) = c;
	//}
	matrix_Normalize(out);
	return out;
}

/*
 * out = matrix where result is stored
 * angle = angle of rotation (in radians)
 * axis = vector representing the position to rotate about
 */
MatrixP matrix_RotateVec2(Matrix out, const float angle, const Vector point) {
	Matrix t, r;

	matrix_TranslateVec2(t, point);
	matrix_RotateZ(r, angle);
	matrix_Mult(out, r, t);
	return out;
}

/*
 * out = matrix where result is stored
 * axis = vector representing the position to rotate about
 * angle = angle of rotation (in radians)
 */
MatrixP matrix_Rotate2f(Matrix out, const float angle, const float x, const float y) {
	matrix_RotateVec2(out, angle, (Vector2f){x, y});
	return out;
}

/*
 * out = matrix where result is stored
 * angle = angle of rotation (in radians)
 * axis = vector representing axis of rotation
 */
MatrixP matrix_RotateVec3(Matrix out, const float angle, const Vector axis) {
	Vector a = {
		axis[0],
		axis[1],
		axis[2],
	};
#define x a[0]
#define y a[1]
#define z a[2]
	float mag = vector_Magnitude(axis),
	      s = ((float)sin(angle)),
	      c = ((float)cos(angle)),
	      xs, ys, zs, onec, xy, xz, yz;

	// ensure the matrix is properly orthogonal
	if (mag == 0.0f) {
		matrix_Identity(out);
		return out;
	}
	
	x /= mag;
	y /= mag;
	z /= mag;
	onec = 1 - c;
	xs = x * s;
	ys = y * s;
	zs = z * s;
	xy = x * y;
	xz = x * z;
	yz = y * z;

	M(out,0,0) = (onec * x * x) + c;
	M(out,0,1) = (onec * xy) + zs;
	M(out,0,2) = (onec * xz) - ys;
	M(out,1,0) = (onec * xy) - zs;
	M(out,1,1) = (onec * y * y) + c;
	M(out,1,2) = (onec * yz) + xs;
	M(out,2,0) = (onec * xz) + ys;
	M(out,2,1) = (onec * yz) - xs;
	M(out,2,2) = (onec * z * z) + c;

	M(out,0,3) = 0.0f;
	M(out,1,3) = 0.0f;
	M(out,2,3) = 0.0f;

	M(out,3,0) = 0.0f;
	M(out,3,1) = 0.0f;
	M(out,3,2) = 0.0f;

	M(out,3,3) = 1.0f;
#undef x
#undef y
#undef z
	matrix_Normalize(out);
	return out;
}

/*
 * out = matrix where result is stored
 * vec = position to translate to
 */
MatrixP matrix_Translatev(Matrix out, const Vector vec) {
	matrix_Identity(out);
	M(out, 0, 3) = vec[0];
	M(out, 1, 3) = vec[1];
	M(out, 2, 3) = vec[2];
	M(out, 3, 3) = 1.0f;
	return out;
}

/*
 * out = matrix where result is stored
 * vec = position to translate to
 */
MatrixP matrix_TranslateVec2(Matrix out, const Vector2f vec) {
	matrix_Identity(out);
	M(out, 0, 3) = vec[0];
	M(out, 1, 3) = vec[1];
	M(out, 2, 3) = 0;
	M(out, 3, 3) = 1.0f;
	return out;
}

/*
 * out = matrix where result is stored
 * x = x position to translate to
 * y = y position to translate to
 */
MatrixP matrix_Translate2f(Matrix out, const float x, const float y) {
	matrix_TranslateVec2(out, (Vector2f){x,y});
	return out;
}

/*
 * out = matrix where result is stored
 * x = x value
 * y = y value
 * z = z value
 */
MatrixP matrix_Translate3f(Matrix out, const float x, const float y, const float z) {
	matrix_Identity(out);
	M(out, 0, 3) = x;
	M(out, 1, 3) = y;
	M(out, 2, 3) = z;
	M(out, 3, 3) = 1.0f;
	return out;
}

/*
 * out = matrix where result is stored
 * factor = amount to scale by
 */
MatrixP matrix_Scale(Matrix out, const float factor) {
	matrix_Identity(out);
	M(out, 0, 0) = factor;
	M(out, 1, 1) = factor;
	M(out, 2, 2) = factor;
	return out;
}

MatrixP matrix_Perspective(Matrix out, const float left, const float right, const float bottom, const float top, const float near, const float far) {
	float fmn = far - near;
	float rml = right - left;
	float tmb = top - bottom;
	float n2 = near * 2;

	M(out, 0, 0) =  n2 / rml;
	M(out, 1, 0) =  0.0f;
	M(out, 2, 0) =  0.0f;
	M(out, 3, 0) =  0.0f;
	M(out, 0, 1) =  0.0f;
	M(out, 1, 1) =  n2 / tmb;
	M(out, 2, 1) =  0.0f;
	M(out, 3, 1) =  0.0f;
	M(out, 0, 2) =  0.0f;
	M(out, 1, 2) =  0.0f;
	M(out, 2, 2) = -(far + near) / fmn;
	M(out, 3, 2) = -1.0f;
	M(out, 0, 3) =  0.0f;
	M(out, 1, 3) =  0.0f;
	M(out, 2, 3) = -n2  * far / fmn;
	M(out, 3, 3) =  0.0f;
	return out;
}

MatrixP matrix_PerspectiveFOVdeg(Matrix out, const float degrees, const int width, const int height, const float near, const float far) {
	float aspect = near * tan(degrees * PIo360);
	matrix_Perspective(out, -aspect, aspect, -aspect, aspect, near, far);
	return out;
}

MatrixP matrix_PerspectiveFOVrad(Matrix out, const float degrees, const int width, const int height, const float near, const float far) {
	float aspect = near * tan(degrees/2);
	matrix_Perspective(out, -aspect, aspect, -aspect, aspect, near, far);
	return out;
}

MatrixP matrix_Orthographic(Matrix out, const float left, const float right, const float bottom, const float top, const float near, const float far) {
	float fmn = far - near;
	float rml = right - left;
	float tmb = top - bottom;

	M(out, 0, 0) =  2 / rml;
	M(out, 0, 1) =  0.0f;
	M(out, 0, 2) =  0.0f;
	M(out, 0, 3) =  -(right + left) / rml;
	M(out, 1, 0) =  0.0f;
	M(out, 1, 1) =  2 / tmb;
	M(out, 1, 2) =  0.0f;
	M(out, 1, 3) =  -(top + bottom) / tmb;
	M(out, 2, 0) =  0.0f;
	M(out, 2, 1) =  0.0f;
	M(out, 2, 2) = -2 / fmn;
	M(out, 2, 3) = -(far + near) / fmn;
	M(out, 3, 0) =  0.0f;
	M(out, 3, 1) =  0.0f;
	M(out, 3, 2) =  0.0f;
	M(out, 3, 3) =  1.0f;
	return out;
}

MatrixP matrix_View(Matrix out, Vector pos, Vector target, Vector up) {
	Vector x,y,z;

	//vector_Sub(z, target, pos);
	vector_Sub(z, pos, target);
	vector_Normalize(z,z);
	vector_Cross(x, z, up);
	vector_Normalize(x,x);
	vector_Cross(y, x, z);
	vector_Normalize(y,y);

	M(out, 0, 0) =  x[0];
	M(out, 0, 1) =  x[1];
	M(out, 0, 2) =  x[2];
	M(out, 1, 0) =  y[0];
	M(out, 1, 1) =  y[1];
	M(out, 1, 2) =  y[2];
	M(out, 2, 0) =  z[0];
	M(out, 2, 1) =  z[1];
	M(out, 2, 2) =  z[2];
	M(out, 3, 0) =  0.0f;
	M(out, 3, 1) =  0.0f;
	M(out, 3, 2) =  0.0f;
	M(out, 0, 3) =  -x[0]*pos[0] - x[1]*pos[1] - x[2]*pos[2];
	M(out, 1, 3) =  -y[0]*pos[0] - y[1]*pos[1] - y[2]*pos[2];
	M(out, 2, 3) =  -z[0]*pos[0] - z[1]*pos[1] - z[2]*pos[2];
	M(out, 3, 3) =  1.0f;
	return out;
}
#undef M


// Vector Math
VectorP vector_Add(Vector out, Vector a, Vector b) {
	Vector temp;

	temp[0] = a[0] + b[0];
	temp[1] = a[1] + b[1];
	temp[2] = a[2] + b[2];

	memcpy(out, temp, fsize*3);
	return out;
}

VectorP vector_Sub(Vector out, Vector a, Vector b) {
	Vector temp;

	temp[0] = a[0] - b[0];
	temp[1] = a[1] - b[1];
	temp[2] = a[2] - b[2];

	memcpy(out, temp, fsize*3);
	return out;
}

VectorP vector_ScalarMult(Vector out, Vector in, const float s) {
	Vector temp;

	temp[0] = in[0] * s;
	temp[1] = in[1] * s;
	temp[2] = in[2] * s;

	memcpy(out, temp, fsize*3);
	return out;
}

VectorP vector_ScalarDiv(Vector out, Vector in, const float s) {
	Vector temp;

	temp[0] = in[0] / s;
	temp[1] = in[1] / s;
	temp[2] = in[2] / s;

	memcpy(out, temp, fsize*3);
	return out;
}

VectorP vector_ScalarAdd(Vector out, Vector in, const float s) {
	Vector temp;

	temp[0] = in[0] + s;
	temp[1] = in[1] + s;
	temp[2] = in[2] + s;

	memcpy(out, temp, fsize*3);
	return out;
}

VectorP vector_CrossProduct(Vector out, Vector a, Vector b) {
	Vector temp;

	temp[0] = (a[1] * b[2]) - (a[2] * b[1]);
	temp[1] = (a[2] * b[0]) - (a[0] * b[2]);
	temp[2] = (a[0] * b[1]) - (a[1] * b[0]);

	memcpy(out, temp, fsize*3);
	return out;
}

float vector_Magnitude(const Vector v) {
#define x v[0]
#define y v[1]
#define z v[2]
	return (float)sqrt(x*x + y*y + z*z);
#undef x
#undef y
#undef z
}

VectorP vector_Normalize(Vector out, Vector in) {
	Vector temp;
	float magnitude = vector_Magnitude(in);
	
	temp[0] = in[0] / magnitude;
	temp[1] = in[1] / magnitude;
	temp[2] = in[2] / magnitude;

	memcpy(out, temp, fsize*3);
	return out;
}

float vector_DotProduct(const Vector v1, const Vector v2) {
	return (v1[0] * v2[0]) + (v1[1] * v2[1]) + (v1[2] * v2[2]);
}

float vector_AngleRad(const Vector v1, const Vector v2) {
	return (float)acos(vector_DotProduct(v1,v2) / (vector_Magnitude(v1) * vector_Magnitude(v2)));
}

float vector_AngleDeg(const Vector v1, const Vector v2) {
	return (float)acos(vector_DotProduct(v1,v2) / (vector_Magnitude(v1) * vector_Magnitude(v2) )) * RAD_DEG;
}

#undef fsize

