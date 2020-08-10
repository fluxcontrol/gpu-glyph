#pragma once
#include <stdint.h>
#include "matrix.h"
#include "graphics.h"

#define FONT_ERR_OK		(  0  )
#define FONT_ERR_LOADFT		( -1  )
#define FONT_ERR_UNLOADFT	( -2  )
#define FONT_ERR_NOFONT		( -3  )
#define FONT_ERR_FONTEXISTS	( -4  )
#define FONT_ERR_LOADFONT	( -5  )
#define FONT_ERR_UNLOADFONT	( -6  )
#define FONT_ERR_SETCHARMAP	( -7  )
#define FONT_ERR_NOGLYPH	( -8  )
#define FONT_ERR_LOADGLYPH	( -9  )
#define FONT_ERR_BADGLYPH	( -10 )
#define FONT_ERR_BBOX		( -11 )
#define FONT_ERR_GLYPH_CURVES	( -12 )
#define FONT_ERR_GLYPH_POINTS	( -13 )

typedef struct {
	void *ftptr;
	size_t emsize;
	float ptsize;
	float glyph_units;
	size_t lineheight;
	size_t space_width;
	size_t max_advance;
	size_t max_points;
	Vector4i max_bbox;
	render_object glyphslot;
} font;

typedef struct {
	char charstr[5];
	uint32_t utf8char;
	uint32_t codepoint;
	int advance;
	int leftbearing;
	int topbearing;
	Vector4i bbox;
	size_t num_paths;
	unsigned *paths;
	size_t num_points;
	Vector3i *points;
	unsigned num_verts;
	Vector2i *verts;
	unsigned num_controls;
	Vector2i *controls;
	unsigned *num_bypath_verts;
	unsigned *num_bypath_controls;
	Vector2i **bypath_verts;
	Vector2i **bypath_controls;
	size_t num_vertices;
	Vector2i *vertbuf;
} glyph_object;

size_t str2glyphs (uint32_t **, const char *);
uint32_t unicode_codepoint(uint32_t const);

font* font_new(const char *);
int font_init(font[1], const char *);
int font_done(font[1]);
int font_ptsize(font[1], const float);
int font_glyphunits(font[1], const float);
int font_resolution2f(font[1], const float, const float);
int font_resolution2iv(font[1], const Vector2i);
int font_color(font[1], const Vector4f);
int font_bbox(font[1], const Vector4i);
int font_advance(font[1], const float);
int font_leftbearing(font[1], const float);
int font_topbearing(font[1], const float);

int glyph_init(font const [1], glyph_object[1]);
int glyph_numpoints(const font[1], glyph_object[1]);
int glyph_numcontrolpoints(const font[1], glyph_object[1], short, short, unsigned[1]);
int glyph_numcurvepoints(const font[1], glyph_object[1], short, short, unsigned[1]);
int glyph_numcurves(const font[1], glyph_object[1]);
int glyph_bbox(const font[1], glyph_object[1]);
int glyph_cbox(const font[1], glyph_object[1]);
int glyph_curves(const font[1], glyph_object[1]);
int glyph_points(const font[1], glyph_object[1], const size_t, const size_t);
int glyph_controlpoints(const font[1], glyph_object[1], Vector2i[1], short, short);
int glyph_curvepoints(const font[1], glyph_object[1], Vector2i[1], short, short);
int glyph_load (font[1], glyph_object[1], uint32_t);
void glyph_gl(font[1], glyph_object[1]);

int render_string (font[1], const char *, const Vector2f, const float, Vector4f);

