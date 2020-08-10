#pragma once

// PI itself
#define PI       3.14159265358979323846264380f

// PI * 2
#define PI2      6.28318530717958647692528760f

// PI / x
#define PIo2     1.57079632679489661923132190f
#define PIo180   0.01745329251994329576923691f
#define PIo360   0.00872664625997164788461845f

// x / PI
#define PIu2     0.63661977236758134307553496f
#define PIu180  57.29577951308232087679814721f
#define PIu360 114.59155902616464175359629442f

// alternative names
#define TWO_PI   PI2
#define DEG_RAD  PIo180
#define RAD_DEG  PIu180


#include "gl_core_3_3.h"
typedef GLfloat Color3f[3]; // r, g, b
typedef GLfloat Color4f[4]; // r, g, b, a
typedef GLdouble Quat[4]; // x, y, z, w
typedef GLfloat Matrix[16];
typedef GLfloat* MatrixP;
typedef GLint Vector2i[2]; // x, y
typedef GLint* Vector2iP; // x, y
typedef GLfloat Vector2f[2]; // x, y
typedef GLfloat* Vector2fP;
typedef GLint Vector3i[3]; // x, y
typedef GLint* Vector3iP; // x, y
typedef GLfloat Vector3f[3]; // x, y, z
typedef GLfloat* Vector3fP;
typedef GLint Vector4i[4]; // x, y
typedef GLint* Vector4iP; // x, y
typedef GLfloat Vector4f[4]; // x, y, z, w
typedef GLfloat* Vector4fP;
typedef GLdouble Vector2d[2]; // x, y
typedef GLdouble* Vector2dP;
typedef GLdouble Vector3d[3]; // x, y, z
typedef GLdouble* Vector3dP;
typedef GLdouble Vector4d[4]; // x, y, z, w
typedef GLdouble* Vector4dP;
typedef GLfloat Vector[3]; // x, y, z
typedef GLfloat* VectorP;

// Matrix Functions
MatrixP matrix_Print(Matrix, const char *);
MatrixP matrix_Identity(Matrix);
MatrixP matrix_Transpose(Matrix, Matrix);
Vector2fP matrix_GetRowVec2(Vector2f, const Matrix, const int);
MatrixP matrix_SetRowVec2(Matrix, const Vector2f, const int);
Vector2fP matrix_GetColVec2(Vector2f, const Matrix, const int);
MatrixP matrix_SetColVec2(Matrix, const Vector2f, const int);
VectorP matrix_GetRow3v(Vector, const Matrix, const int);
MatrixP matrix_SetRow3v(Matrix, const Vector, const int);
VectorP matrix_GetCol3v(Vector, const Matrix, const int);
MatrixP matrix_SetCol3v(Matrix, const Vector, const int);
MatrixP matrix_Normalize(Matrix);
#define matrix_Multiply(x,y,z) matrix_Mult(x,y,z)
MatrixP matrix_Mult(Matrix, Matrix, Matrix);
#define matrix_ScalarMultiply(x,y,z) matrix_ScalarMult(x,y,z)
MatrixP matrix_ScalarMult(Matrix, Matrix, const float);
MatrixP matrix_ScalarAdd(Matrix, Matrix, const float);
#define matrix_Rotate(m, a) matrix_RotateZ(m, a)
MatrixP matrix_RotateX(Matrix, const float);
MatrixP matrix_RotateY(Matrix, const float);
MatrixP matrix_RotateZ(Matrix, const float);
MatrixP matrix_RotateVec2(Matrix, const float, const Vector2f);
MatrixP matrix_Rotate2f(Matrix, const float, const float, const float);
MatrixP matrix_Rotate3f(Matrix, const float, const float, const float, const float);
MatrixP matrix_Translatev(Matrix, const Vector);
MatrixP matrix_TranslateVec2(Matrix, const Vector2f);
MatrixP matrix_Translate2f(Matrix, const float, const float);
MatrixP matrix_Translate3f(Matrix, const float, const float, const float);
MatrixP matrix_Scalev(Matrix, const Vector);
MatrixP matrix_ScaleVec2(Matrix, const Vector2f);
MatrixP matrix_Scale(Matrix, const float);
MatrixP matrix_Scale2f(Matrix, const float, const float);
MatrixP matrix_Scale3f(Matrix, const float, const float, const float);
MatrixP matrix_Scale4f(Matrix, const float, const float, const float, const float);
MatrixP matrix_Perspective(Matrix, const float, const float, const float, const float, const float, const float);
#define matrix_PerspectiveFOV(m,d,w,h,n,f) matrix_PerspectiveFOVdeg(m,d,w,h,n,f)
MatrixP matrix_PerspectiveFOVrad(Matrix, const float, const int, const int, const float, const float);
MatrixP matrix_PerspectiveFOVdeg(Matrix, const float, const int, const int, const float, const float);
MatrixP matrix_Orthographic(Matrix, const float, const float, const float, const float, const float, const float);
MatrixP matrix_View(Matrix, Vector, Vector, Vector);

// Vector Functions
VectorP vector_Add(Vector, Vector, Vector);
VectorP vector_Sub(Vector, Vector, Vector);
VectorP vector_ScalarAdd(Vector, Vector, const float);
VectorP vector_ScalarMult(Vector, Vector, const float);
VectorP vector_ScalarDiv(Vector, Vector, const float);
#define vector_Cross(x,y,z) vector_CrossProduct(x,y,z)
VectorP vector_CrossProduct(Vector, Vector, Vector);
float vector_Magnitude(const Vector);
VectorP vector_Normalize(Vector, Vector);
#define vector_Dot(x,y) vector_DotProduct(x,y)
float vector_DotProduct(const Vector, const Vector);
#define vector_Angle(x,y) vector_AngleRad(x,y)
float vector_AngleDeg(const Vector, const Vector);
float vector_AngleRad(const Vector, const Vector);
