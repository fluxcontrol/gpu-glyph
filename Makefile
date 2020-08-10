CC = clang
#CC = gcc

local_CFLAGS = -std=c17 -I./ -O0 -g -Wall -pedantic-errors $(CFLAGS)
local_LDFLAGS = -lfreetype -lGL -lglfw $(LDFLAGS)

RELEASE_APP = demo
DEBUG_APP = demo.debug

.PHONY: all debug
all: $(RELEASE_APP) vshader fshader font-vshader font-fshader
debug: $(DEBUG_APP) vshader fshader font-vshader font-fshader


vshader: vertex.glsl
	glslangValidator -S vert $<
fshader: fragment.glsl
	glslangValidator -S frag $<
font-vshader: font-vertex.glsl
	glslangValidator -S vert $<
font-fshader: font-fragment.glsl
	glslangValidator -S frag $<


$(DEBUG_APP): debug_graphics.o debug_font.o debug_matrix.o gl_core_3_3.o debug_main.o
	$(CC) $(local_LDFLAGS) -o $@ $^
debug_main.o: main.c font.h matrix.h graphics.h gl_core_3_3.h debug.h
	$(CC) -g -O0 -DDEBUG $(DEFINES) $(local_CFLAGS) -c -o $@ $<
debug_graphics.o: graphics.c graphics.h matrix.h gl_core_3_3.h debug.h
	$(CC) -g -O0 -DDEBUG $(DEFINES) $(local_CFLAGS) -c -o $@ $<
debug_matrix.o: matrix.c matrix.h debug.h
	$(CC) -g -O0 -DDEBUG $(DEFINES) $(local_CFLAGS) -c -o $@ $<
debug_font.o: font.c font.h matrix.h debug.h
	$(CC) -g -O0 -DDEBUG $(DEFINES) $(local_CFLAGS) -I/usr/include/freetype2 -c -o $@ $<



$(RELEASE_APP): graphics.o font.o matrix.o gl_core_3_3.o main.o
	$(CC) $(local_LDFLAGS) -o $@ $^
main.o: main.c font.h matrix.h graphics.h gl_core_3_3.h debug.h
	$(CC) $(DEFINES) $(local_CFLAGS) -c -o $@ $<
graphics.o: graphics.c graphics.h matrix.h gl_core_3_3.h debug.h
	$(CC) $(DEFINES) $(local_CFLAGS) -c -o $@ $<
font.o: font.c font.h matrix.h debug.h
	$(CC) $(DEFINES) $(local_CFLAGS) -I/usr/include/freetype2 -c -o $@ $<
matrix.o: matrix.c matrix.h debug.h
	$(CC) $(DEFINES) $(local_CFLAGS) -c -o $@ $<


gl_core_3_3.o: gl_core_3_3.c gl_core_3_3.h
	$(CC) $(DEFINES) $(local_CFLAGS) -c -o $@ $<


clean:
	@rm *.o
