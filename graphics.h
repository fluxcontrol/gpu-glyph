#pragma once
#define GLFW_INCLUDE_NONE
#include <stdbool.h>
#include <GLFW/glfw3.h>
#include "gl_core_3_3.h"
#include "matrix.h"


#define WINWIDTH 640
#define WINHEIGHT 480
#define BGCOLOR_R 0.4
#define BGCOLOR_G 0.4
#define BGCOLOR_B 0.7
#define WINTITLE "Hello World"


#define RENDER_2D	0x02
#define RENDER_3D	0x03


typedef struct {
	Vector3f	pos;
	Color4f		color;
} vertex;

typedef struct {
	unsigned type;
	unsigned components;
	size_t data_size;
	void *data;
	size_t stride;
	size_t offset;
	bool normalize;
} vertex_attribute;

typedef struct {
	GLuint vertex_array;
	GLuint *buffers;
	GLuint num_vertices;
	GLuint shader;
	GLint mvp_uniform;
	GLuint drawtype;
	unsigned num_attributes;
	vertex_attribute *attributes;
	GLuint num_indices;
	GLuint *indices;
} render_object;


/*
GLuint load_shader_file(const char *, const int);
*/
GLFWwindow* graphics_window(void);
void shader_program(render_object * const, const char *, const char *);
void shader_init(render_object * const, const char *, const char *);
void render(const unsigned, const render_object *, Matrix, const size_t, const size_t, const unsigned);
#define render2d(...) render(RENDER_2D, __VA_ARGS__)
#define render3d(...) render(RENDER_3D, __VA_ARGS__)


/*
void shader_init_Vector(render_object *, Vector *, size_t, const char *, const char *);
void shader_init2i(render_object *, Vector2i *, size_t, const char *, const char *);
void shader_init3i(render_object *, Vector3i *, size_t, const char *, const char *);
void shader_init2f(render_object *, Vector2f *, size_t, const char *, const char *);
void shader_init3f(render_object *, Vector3f *, size_t, const char *, const char *);
#define shader_init(X, Y, R, V, F)\
	_Generic(( X), \
		Vector2i* : shader_init2i, \
		Vector2f* : shader_init2f, \
		Vector3i* : shader_init3i, \
		Vector3f* : shader_init3f, \
		default : shader_init_Vector)(R, X, Y, V, F)
*/


#ifndef GL_TESSELATION_SHADER
#define GL_TESSELATION_SHADER 9998
#endif
#ifndef GL_COMPUTE_SHADER
#define GL_COMPUTE_SHADER 9999
#endif
#ifndef GL_STACK_UNDERFLOW
#define GL_STACK_UNDERFLOW 9998
#endif
#ifndef GL_STACK_OVERFLOW
#define GL_STACK_OVERFLOW 9999
#endif
#define PRNTGLERR(...) \
	do {\
		if (DEBUG_ON) {\
			TRACE_DEBUG("ERROR: GL: " TRACE_ARGFIRST(__VA_ARGS__) "%.0d", TRACE_ARGREMAIN(__VA_ARGS__));\
		}\
	} while(0)
extern GLenum glenumerr;
const char * glerr2str(GLenum errcode);
#define CHKGLERR(...) \
	glenumerr = glGetError();\
	while (glenumerr != GL_NO_ERROR) {\
		PRNTGLERR("%s: " TRACE_ARGFIRST(__VA_ARGS__) "%.0d", glerr2str(glenumerr), TRACE_ARGREMAIN(__VA_ARGS__));\
		glenumerr = glGetError();\
	}

