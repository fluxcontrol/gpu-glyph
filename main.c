#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <gl_core_3_3.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "debug.h"
#include "font.h"
#include "graphics.h"
#include "matrix.h"

void cleanup(GLFWwindow *w, font *f, char *text) {
	if (f != NULL) {
		font_done(f);
		free(f);
	}

	if (text != NULL) {
		free(text);
	}

	glfwDestroyWindow(w);
	glfwTerminate();
}

int main(int argc, char *argv[]) {
	GLFWwindow *window = NULL;

	if (!glfwInit()) {
		return -1;
	}

	window = graphics_window();
	if (!window) {
		glfwTerminate();
		return -1;
	}

	const char fontfile[] = "/usr/share/fonts/TTF/Arimo-Regular.ttf";
	font *f = font_new(argc >= 2 ? argv[1] : fontfile);
	if (f == NULL || f->ftptr == NULL)
		return 1;
	font_ptsize(f, (argc >= 4) ? strtof(argv[3], 0) : 18.0f);
	int w = 0, h = 0;
	glfwGetWindowSize(window, &w, &h);
	font_resolution2f(f, w, h);
	glfwSetWindowUserPointer(window, &f);

	char *text = NULL;
	if (argc >= 3) {
		text = argv[2];
	}
	else {
		text = calloc(2, sizeof(char));
		text[0] = 'e';
		text[1] = 0;
	}

	int status = {0};
	while (!glfwWindowShouldClose(window)) {
		CHKGLERR(); // clear the error buffer first
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		CHKGLERR("glClear (draw_frame)");

		status = render_string(f
				,text				// text string to render
				,(Vector2f){-1.0, -0.9}		// origin position of rendered text string
				,1.0f				// scale factor
				,(Vector4f){0.2, 0.8, 0.3, 1.0}	// color to render the text with, RGBA
				);
		if (status < 0) {
			cleanup(window, f, argc >= 3 ? NULL : text);
			return 1;
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	cleanup(window, f, argc >= 3 ? NULL : text);
	return 0;
}

