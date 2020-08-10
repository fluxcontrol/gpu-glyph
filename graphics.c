#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "gl_core_3_3.h"

#include "debug.h"
#include "font.h"
#include "matrix.h"
#include "graphics.h"

#ifdef _WIN32
	/*
	 * windows.h (via minwindef.h) は空の#defineを規定する為に必要
	 */
	#undef near
	#undef far
#endif

static GLuint load_shader_file(const char *, const int);
static void key_callback(GLFWwindow *, int, int, int, int);
static void window_size_callback(GLFWwindow *, int, int);
static void framebuffer_size_callback(GLFWwindow*, int, int);
void chkglerr(const char *);
//void shader_program(render_object * const, const char *, const char *);
void update_mvp(Matrix, const render_object *, Matrix);
void initGL(GLFWwindow *);


Matrix perspective_projection;
Matrix orthographic_projection;
Vector *vertex_array;
size_t vertex_array_size;




GLenum glenumerr = {0};
const char * glerr2str(GLenum errcode) {
	switch (errcode) {
		case GL_INVALID_ENUM:
			return "invalid enum";
		case GL_INVALID_VALUE:
			return "invalid value";
		case GL_INVALID_OPERATION:
			return "invalid operation";
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			return "invalid framebuffer operation";
		case GL_OUT_OF_MEMORY:
			return "out of memory: ";
		case GL_STACK_UNDERFLOW:
			return "stack underflow";
		case GL_STACK_OVERFLOW:
			return "stack overflow";
	}
	return "";
}

/*
void APIENTRY *debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
void APIENTRY *debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
	if (userParam == NULL)
		return NULL;

	printf("ERROR: GL: %s\n", message);
	return 1;
}
//typedef void (APIENTRY *debug_callback)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) debug_callback_func;
*/



enum {
	Vertex_Shader,
	Fragment_Shader,
	Geometry_Shader,
	Tesselation_Shader,
	Compute_Shader,
};
static const char* shader_names[] = {
	[Vertex_Shader] = "vertex",
	[Fragment_Shader] = "fragment",
	[Geometry_Shader] = "geometry",
	[Tesselation_Shader] = "tesselation",
	[Compute_Shader] = "compute",
};
static GLuint shader_types[] = {
	[Vertex_Shader]		= GL_VERTEX_SHADER,
	[Fragment_Shader]	= GL_FRAGMENT_SHADER,
	[Geometry_Shader]	= GL_GEOMETRY_SHADER,
	[Tesselation_Shader]	= GL_TESSELATION_SHADER,
	[Compute_Shader] 	= GL_COMPUTE_SHADER,
};

GLuint load_shader_file(const char *source_file, const int shader_type) {
	GLuint shader = {0};
	if (source_file == NULL) {
		debug("invalid shader source filename");
		return 0;
	}
	FILE *f = NULL;
	GLchar *shader_source = NULL;

	f = fopen(source_file, "rb");
	if (f == NULL) {
		debug("failed to open file: %s", source_file);
		goto FINISH_LOADSHADER;
	}
	if (fseek(f, 0L, SEEK_END) < 0) {
		debug("failed to seek to end of file: %s", source_file);
		goto FINISH_LOADSHADER;
	}
	long file_len = ftell(f) + 1;
	if (fseek(f, 0L, SEEK_SET) < 0) {
		debug("failed to seek to start of file: %s", source_file);
		goto FINISH_LOADSHADER;
	}
	shader_source = calloc(file_len, sizeof(GLchar));
	if (shader_source == NULL) {
		debug("failed to allocate memory for shader source: %s", source_file);
		goto FINISH_LOADSHADER;
	}
	size_t nread = fread(shader_source, sizeof(GLchar), file_len - 1, f);
	if (nread == 0) {
		debug("failed to read shader source: %s", source_file);
		goto FINISH_LOADSHADER;
	}
	fclose(f);
	f = NULL;

	char log[512] = {0};
	int status = 0;
	// clear the error buffer
	CHKGLERR();
	shader = glCreateShader(shader_types[shader_type]);\
	CHKGLERR("glCreateShader (%s)", shader_names[shader_type]);\
	glShaderSource(shader, 1, (const GLchar *const *)&shader_source, NULL);\
	CHKGLERR("glShaderSource (%s)", shader_names[shader_type]);\
	glCompileShader(shader);\
	CHKGLERR("glCompileShader (%s)", shader_names[shader_type]);\
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);\
	if (status == 0) {\
		glGetShaderInfoLog(shader, 512, NULL, log);\
		debug("ERROR: GL: %s shader: %s\n", shader_names[shader_type], log);\
	}
	goto FINISH_LOADSHADER;

FINISH_LOADSHADER:
	if (f != NULL)
		fclose(f);
	if (shader_source != NULL)
		free(shader_source);
	return shader;
}


static void key_callback(GLFWwindow* window, const int key, const int scancode, const int action, const int mods) {
	switch (key) {
		case GLFW_KEY_ESCAPE:
			if (action == GLFW_PRESS)
				glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
	}
}

static void window_size_callback(GLFWwindow* window, const int width, const int height) {
	int w,h;
	glfwGetFramebufferSize(window, &w, &h);
	framebuffer_size_callback(window, w, h);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	/*
	*/
	font *fptr = glfwGetWindowUserPointer(window);
	if (fptr != NULL) {
		debug("Value of f: %p", (void *)fptr);
		//font_resolution2f(f, width, height);
	}

	float ratio = width/(float)height;
	matrix_Orthographic(orthographic_projection, -ratio, ratio, -ratio, ratio, -1.0f,  1.0f);

	float fov = 45.0f;
	float near = 0.1f;
	float far = 100.0f;
	matrix_PerspectiveFOV(perspective_projection, fov, width, height, near, far);
}

void shader_program(render_object * robj, const char *vshader_file, const char *fshader_file) {
	CHKGLERR(); // clear the error buffer first

	int status = 0;
	char log[512] = {0};
	GLuint vertex_shader = {0};
	GLuint fragment_shader = {0};
	GLuint prg = {0};

	vertex_shader = load_shader_file(vshader_file ? vshader_file : "vertex.glsl", Vertex_Shader);
	fragment_shader = load_shader_file(fshader_file ? fshader_file : "fragment.glsl", Fragment_Shader);

	prg = glCreateProgram();
	CHKGLERR("glCreateProgram");
	glAttachShader(prg, vertex_shader);
	CHKGLERR("glAttachShader (vertex)");
	glAttachShader(prg, fragment_shader);
	CHKGLERR("glAttachShader (fragment)");
	glLinkProgram(prg);
	CHKGLERR("glLinkProgram");
	glGetProgramiv(prg, GL_LINK_STATUS, &status);
	if (!status) {
		glGetProgramInfoLog(prg, 512, NULL, log);
		CHKGLERR("glGetProgramInfoLog");
		debug("ERROR: GL: link shader program: %s", log);
		return;
	}
	else {
		glDeleteShader(vertex_shader);
		CHKGLERR("glDeleteShader (vertex)");
		glDeleteShader(fragment_shader);
		CHKGLERR("glDeleteShader (fragment)");
	}
	robj->shader = prg;

	glUseProgram(robj->shader);
	CHKGLERR("glUseProgram (robj->shader)");
	robj->mvp_uniform = glGetUniformLocation(prg, "mvp");
	CHKGLERR("glGetUniformLocation (mvp)");
	if (robj->mvp_uniform == -1) {
		debug("ERROR: GL: uniform mvp points to invalid location (optimized out of shader?)");
	}
}

void renderobj_init(render_object * const robj, const char *vshader, const char *fshader) {
	void *vdata = NULL;
	size_t vsize_vdata = {0},
	       vsize_component = {0},
	       vsize_vertex = {0},
	       vstride = {0},
	       voffset = {0};
	unsigned vcomponents = {0};
	GLuint vnum = {0},
	       vtype = {0},
	       vnormalize = {0};

	CHKGLERR(); // clear the error buffer first
	shader_program(robj, vshader, fshader);
	glUseProgram(robj->shader);
	CHKGLERR("glUseProgram (%u)", robj->shader);
	glGenVertexArrays(1, &(robj->vertex_array));
	CHKGLERR("glGenVertexArrays");
	glBindVertexArray(robj->vertex_array);
	CHKGLERR("glBindVertexArray");
	if (robj->buffers == NULL) {
		robj->buffers = calloc(robj->num_attributes, sizeof(GLuint));
		if (robj->buffers == NULL) {
			debug("failed to allocate memory for robj->buffers");
			return;
		}
	}
	glGenBuffers(robj->num_attributes, robj->buffers);
	CHKGLERR("glGenBuffers");

	for (unsigned i=0; i < robj->num_attributes; i++) {
		vdata = robj->attributes[i].data;
		vsize_vdata = robj->attributes[i].data_size;
		switch (robj->attributes[i].type) {
			case GL_INT:
				vsize_component = sizeof(GLint);
				vtype = GL_INT;
				break;
			case GL_FLOAT:
				vsize_component = sizeof(GLfloat);
				vtype = GL_FLOAT;
				break;
		}
		vsize_vertex = vsize_component * robj->attributes[i].components;
		vcomponents = robj->attributes[i].components;

		vnum = vsize_vdata / vsize_vertex;
		vnormalize = robj->attributes[i].normalize ? GL_TRUE : GL_FALSE;
		vstride = robj->attributes[i].stride;
		voffset = robj->attributes[i].offset;
		if (robj->num_vertices == 0) {
			switch(robj->attributes[0].type) {
				case GL_FLOAT:
					vsize_component = sizeof(GLfloat);
					break;
				case GL_INT:
					vsize_component = sizeof(GLint);
					break;
			}
			vsize_vertex = vsize_component * robj->attributes[0].components;
			robj->num_vertices = robj->attributes[0].data_size / vsize_vertex;
		}
		else {
			vsize_vertex = robj->attributes[0].data_size / robj->num_vertices;
			vsize_component = vsize_vertex / robj->attributes[0].components;
		}
		glBindBuffer(GL_ARRAY_BUFFER, robj->buffers[i]);
		CHKGLERR("glBindBuffer (attribute: %u)", i);
		glBufferData(GL_ARRAY_BUFFER, robj->attributes[i].data_size, robj->attributes[i].data, GL_STATIC_DRAW);
		CHKGLERR("glBufferData (attribute: %u)", i);
		glEnableVertexAttribArray(i);
		CHKGLERR("glEnableVertexAttribArray (attribute: %u)", i);
		glVertexAttribPointer(i, robj->attributes[i].components, robj->attributes[i].type, robj->attributes[i].normalize ? GL_TRUE : GL_FALSE, robj->attributes[i].stride, (void *)robj->attributes[i].offset);
		CHKGLERR("glVertexAttribPointer (attribute: %u)", i);
	}
}

void renderobj_done (render_object *robj) {
	if (robj == NULL) {
		debug("invalid render_object");
		return;
	}
	if (robj->buffers) {
		free(robj->buffers);
		robj->buffers = NULL;
	}
	if (robj->attributes) {
		for (unsigned i=0; i < robj->num_attributes; i++) {
			if (robj->attributes[i].data != NULL) {
				free(robj->attributes[i].data);
				robj->attributes[i].data = NULL;
			}
			free(robj->attributes);
			robj->attributes = NULL;
		}
	}
}

void update_mvp(Matrix scene, const render_object *robj, Matrix proj) {
	GLint mvploc = robj->mvp_uniform;
	if (scene != NULL) {
		//GLint mvploc = glGetUniformLocation(robj->shader, "mvp");
		if (mvploc != -1) {
			Matrix mvp = {0};
			matrix_Mult(mvp, scene, proj);
			glUniformMatrix4fv(mvploc, 1, GL_FALSE, (const GLfloat*)scene);
			CHKGLERR("glUniformMatrix4fv (mvp: %u)", mvploc);
		}
		else {
			debug("ERROR: GL: uniform mvp points to invalid location (optimized out of shader?)");
			return;
		}
	}
}

void render(const unsigned proj_type, const render_object *robj, Matrix scene, const size_t start, const size_t end, const unsigned draw_type) {
	glUseProgram(robj->shader);
	CHKGLERR("glUseProgram (program: %u)", robj->shader);
	glBindVertexArray(robj->vertex_array);
	CHKGLERR("glBindVertexArray (vao: %u)", robj->vertex_array);
	switch (proj_type) {
		case RENDER_2D:
			update_mvp(scene, robj, orthographic_projection);
			break;
		case RENDER_3D:
			update_mvp(scene, robj, perspective_projection);
			break;
	}
	glDrawArrays(draw_type, start, end > 0 ? end : robj->num_vertices);
	CHKGLERR("glDrawArrays (%u)", draw_type);
}

GLFWwindow *graphics_window(void) {
	GLFWwindow *window;

	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	window = glfwCreateWindow(
			WINWIDTH,
			WINHEIGHT,
			WINTITLE,
			NULL,
			NULL
			);
	if (!window)
		return NULL;

	if (!ogl_LoadFunctions()) {
		debug("Failed to load OpenGL functions");
		exit(2);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwSetKeyCallback(window, key_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	initGL(window);

	return window;
}

void initGL(GLFWwindow* window) {
	// Winding and Culling
	glFrontFace(GL_CCW);
	CHKGLERR("glFrontFace (GL_CCW)");
	glCullFace(GL_BACK);
	CHKGLERR("glCullFace (GL_BACK)");
	glEnable(GL_CULL_FACE);
	CHKGLERR("glEnable (GL_DEPTH_TEST)");

	// Blending
	glEnable(GL_BLEND);
	CHKGLERR("glEnable (GL_DEPTH_TEST)");
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	CHKGLERR("glBlendFunc (GL_SRC_ALPHA, GL_ONE)");

	// Depth
	glEnable(GL_DEPTH_TEST);
	CHKGLERR("glEnable (GL_DEPTH_TEST)");
	glDepthFunc(GL_LEQUAL);
	CHKGLERR("glDepthFunc (GL_LEQUAL)");

	// Shading and perspective
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	CHKGLERR("glHint (GL_LINE_SMOOTH_HINT)");
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	CHKGLERR("glHint (GL_POLYGON_SMOOTH_HINT)");
								 
	glClearColor(BGCOLOR_R, BGCOLOR_G, BGCOLOR_B, 0.0);
	CHKGLERR("glClearColor");

	framebuffer_size_callback(window, WINWIDTH, WINHEIGHT);
}

