#include "debug.h"
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftbbox.h>
#include <freetype/ftoutln.h>
#include <freetype/tttables.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "font.h"
#include "matrix.h"


enum { point_tCurve, point_tControl };


FT_Library ft = {0};


uint32_t unicode_codepoint(uint32_t const utfchar) {
	uint32_t ut = utfchar;
	unsigned bytes = 0;
	if (!(ut & 0x80) && (ut & 0x7F))
		bytes = 1;
	if ((ut & 0xE0) == 0xC0)
		bytes = 2;
	if ((ut & 0xF0) == 0xE0)
		bytes = 3;
	if ((ut & 0xF8) == 0xF0)
		bytes = 4;
	if (bytes == 1)
		return utfchar;

	uint32_t cp = {0};
	uint8_t *ucb = (uint8_t *)&ut;
	uint8_t *cpb = (uint8_t *)&cp;

	ut &= 0x3F3F3FFF;
	switch(bytes) {
		case 2:
			cpb[1] = (ucb[0] & 0x1F) >> 2;
			cpb[0] = (ucb[0] << 6) | ucb[1];
			break;
		case 3:
			cpb[1] = (ucb[0] << 4) | (ucb[1] >> 2);
			cpb[0] = (ucb[1] << 6) | ucb[2];
			break;
		case 4:
			cpb[2] = ((ucb[0] & 0x07) << 2) | (cpb[1] >> 4);
			cpb[1] = (ucb[1] << 4) | (ucb[2] >> 2);
			cpb[0] = (ucb[2] << 6) | ucb[3];
			break;
	}
	return cp;
}

size_t str2glyphs (uint32_t **glyphset, const char * input_string) {
	if (input_string == NULL) {
		debug("input string null");
		return 0;
	}

	size_t len = strlen(input_string);
	if (len == 0) {
		debug("input string len zero");
		return 0;
	}

	uint8_t *str = (uint8_t *)input_string;
	*glyphset = calloc(len, sizeof(uint32_t));
	size_t count = {0};
	char *c = NULL;

	for (size_t i=0; i < len; count++) {
		if (count > len) {
			debug("count > len");
			return 0;
		}
		unsigned bytes = 0;
		c = (char *)(&(*glyphset)[count]);
		if (!(str[i] & 0x80) && (str[i] & 0x7F)) {
			bytes = 1;
		}
		else {
			if ((str[i] & 0xE0) == 0xC0)
				bytes = 2;
			if ((str[i] & 0xF0) == 0xE0)
				bytes = 3;
			if ((str[i] & 0xF8) == 0xF0)
				bytes = 4;
		}
		if (! bytes) {
			debug("invalid character: zero bytes");
			return 0;
		}
		if ((i + bytes - 1) < len) {
			for (unsigned j=0; j < bytes; j++) {
				if (j && (str[i+j] & 0xC0) != 0x80) {
					debug("invalid byte [%u]", j);
					return 0;
				}
				c[j] = str[i+j];
			}
		}
		if (! c[0]) {
			debug("invalid character");
			return 0;
		}
		i += bytes;
	}

	return count;
}

int glyph_init(font const f[1], glyph_object glyph[1]) {
	int status = {0};
	if (f->ftptr == NULL) {
		debug("invalid font object");
		return FONT_ERR_NOFONT;
	}
	if (glyph->utf8char == 0) {
		debug("no glyph to load");
		return FONT_ERR_NOGLYPH;
	}

	glyph->charstr[0] = ((char *)(&glyph->utf8char))[0];
	glyph->charstr[1] = ((char *)(&glyph->utf8char))[1];
	glyph->charstr[2] = ((char *)(&glyph->utf8char))[2];
	glyph->charstr[3] = ((char *)(&glyph->utf8char))[3];
	glyph->charstr[4] = 0;

	FT_Face face = *(FT_Face*)(f->ftptr);

	int err = 0;
	uint32_t c = unicode_codepoint(glyph->utf8char);
	glyph->codepoint = c;
	FT_UInt cind = FT_Get_Char_Index(face, (FT_ULong)c);
	err = FT_Load_Glyph(face, cind, FT_LOAD_NO_SCALE);
	if (err || face->glyph->glyph_index == 0) {
		debug("error loading glyph: '%s': %#x", glyph->charstr, err);
		return FONT_ERR_LOADGLYPH;
	}

	//glyph.advance = face->glyph->advance.x;
	glyph->advance = face->glyph->metrics.horiAdvance;
	glyph->leftbearing = face->glyph->metrics.horiBearingX;
	/*
	debug("glyph metrics for '%s':"
			"\n\twidth x height: %ld x %ld"
			"\n\tmetrics advance: (h: %ld, v: %ld)"
			"\n\tmetrics horizontal bearing: (%ld, %ld)"
			"\n\tmetrics vertical bearing: (%ld, %ld)"
			"\n\tlinear (unhinted) advance: (h: %ld, v: %ld)"
			"\n\tadvance: (h: %ld, v: %ld)"
			"\n\tbearing deltas: (lsb: %ld, rsb: %ld)"
			,glyph->charstr
			,face->glyph->metrics.width, face->glyph->metrics.height
			,face->glyph->metrics.horiAdvance, face->glyph->metrics.vertAdvance
			,face->glyph->metrics.horiBearingX, face->glyph->metrics.horiBearingY
			,face->glyph->metrics.vertBearingX, face->glyph->metrics.vertBearingY
			,face->glyph->linearHoriAdvance, face->glyph->linearVertAdvance
			,face->glyph->advance.x, face->glyph->advance.y
			,face->glyph->lsb_delta, face->glyph->rsb_delta
			);
	*/

	status = glyph_cbox(f, glyph);
	if (status != FONT_ERR_OK) {
		//debug("couldn't load cbox");
		return status;
	}
	status = glyph_numpoints(f, glyph);
	if (status != FONT_ERR_OK) {
		//debug("couldn't load numpoints");
		return status;
	}
	status = glyph_numcurves(f, glyph);
	if (status != FONT_ERR_OK) {
		//debug("couldn't load numcurves");
		return status;
	}
	status = glyph_curves(f, glyph);
	if (status != FONT_ERR_OK) {
		//debug("couldn't load curves");
		return status;
	}
	status = glyph_points(f, glyph, 0, 0);
	if (status != FONT_ERR_OK) {
		//debug("couldn't load points");
		return status;
	}

	/*
	debug("Glyph loaded:"
			"\n\tcharacter code of '%s': %#0x"
			"\n\tfont glyph index of '%s': %u"
			"\n\thorizontal leftbearing: %d"
			"\n\thorizontal advance: %d"
			"\n\tnum paths: %zu"
			"\n\tnum total vertices: %zu"
			"\n\tbounding box: (%d, %d)  (%d, %d)"
			,glyph->charstr, c
			,glyph->charstr, cind
			,glyph->leftbearing
			,glyph->advance
			,glyph->num_paths
			,glyph->num_points
			,glyph->bbox[0], glyph->bbox[1], glyph->bbox[2], glyph->bbox[3]
			);
	*/

	return FONT_ERR_OK;
}

int glyph_numpoints(const font f[1], glyph_object glyph[1]) {
	if (f->ftptr == NULL) {
		debug("invalid font object");
		return FONT_ERR_NOFONT;
	}
	FT_Face face = *(FT_Face*)(f->ftptr);
	if (face->glyph->glyph_index == 0) {
		debug("glyph invalid");
		return FONT_ERR_BADGLYPH;
	}
	unsigned p = face->glyph->outline.n_points;
	if (p < 1) {
		//debug("glyph defines no points");
		return FONT_ERR_BADGLYPH;
	}
	if (glyph != NULL)
		glyph->num_points = p;

	return FONT_ERR_OK;
}

int glyph_numcontrolpoints(const font f[1], glyph_object glyph[1], short start, short end, unsigned num_points[1]) {
	if (f->ftptr == NULL) {
		debug("invalid font object");
		return FONT_ERR_NOFONT;
	}
	FT_Face face = *(FT_Face*)(f->ftptr);
	if (face->glyph->glyph_index == 0) {
		debug("glyph invalid");
		return FONT_ERR_BADGLYPH;
	}
	int num = 0;
	short e = face->glyph->outline.n_points;
	if (end)
		e = end + 1;
	for (short i=start; i < e; i++) {
		if ((face->glyph->outline.tags[i] & 0x1) != 1)
			num++;
	}
	*num_points = num;

	return FONT_ERR_OK;
}

int glyph_numcurvepoints(const font f[1], glyph_object glyph[1], short start, short end, unsigned num_points[1]) {
	if (f->ftptr == NULL) {
		debug("invalid font object");
		return FONT_ERR_NOFONT;
	}
	FT_Face face = *(FT_Face*)(f->ftptr);
	if (face->glyph->glyph_index == 0) {
		debug("glyph invalid");
		return FONT_ERR_BADGLYPH;
	}
	debug("glyph outline points: %hd", face->glyph->outline.n_points);
	int num = 0;
	short e = face->glyph->outline.n_points;
	if (end)
		e = end + 1;
	for (short i=start; i < e; i++) {
		if ((face->glyph->outline.tags[i] & 0x1) == 1)
			num++;
	}
	*num_points = num;

	return FONT_ERR_OK;
}

int glyph_numcurves(const font f[1], glyph_object glyph[1]) {
	if (f->ftptr == NULL) {
		debug("invalid font object");
		return FONT_ERR_NOFONT;
	}
	FT_Face face = *(FT_Face*)(f->ftptr);
	if (face->glyph->glyph_index == 0) {
		debug("no valid glyph loaded");
		return FONT_ERR_BADGLYPH;
	}

	unsigned p = face->glyph->outline.n_contours;
	if (p < 1) {
		debug("glyph defines no curves");
		return FONT_ERR_GLYPH_CURVES;
	}
	glyph->num_paths = p;

	return FONT_ERR_OK;
}

int glyph_cbox(const font f[1], glyph_object glyph[1]) {
	if (f->ftptr == NULL) {
		debug("invalid font object");
		return FONT_ERR_NOFONT;
	}
	FT_Face face = *(FT_Face*)(f->ftptr);
	if (face->glyph->glyph_index == 0) {
		debug("no valid glyph loaded");
		return FONT_ERR_BADGLYPH;
	}
	FT_BBox bbox = {0};
	FT_Outline_Get_CBox(&face->glyph->outline, &bbox);
	/*
	err = FT_Outline_Get_CBox(&face->glyph->outline, &bbox);
	if (err) {
		debug("error obtaining glyph bbox: %#x", err);
		return FONT_ERR_BBOX;
	}
	*/
	glyph->bbox[0] = (int)bbox.xMin;
	glyph->bbox[1] = (int)bbox.yMin;
	glyph->bbox[2] = (int)bbox.xMax;
	glyph->bbox[3] = (int)bbox.yMax;
	return FONT_ERR_OK;
}

int glyph_bbox(const font f[1], glyph_object glyph[1]) {
	if (f->ftptr == NULL) {
		debug("invalid font object");
		return FONT_ERR_NOFONT;
	}
	FT_Face face = *(FT_Face*)(f->ftptr);
	if (face->glyph->glyph_index == 0) {
		debug("no valid glyph loaded");
		return FONT_ERR_BADGLYPH;
	}
	FT_BBox bbox = {0};
	int err = FT_Outline_Get_BBox(&face->glyph->outline, &bbox);
	if (err) {
		debug("error obtaining glyph bbox: %#x", err);
		return FONT_ERR_BBOX;
	}
	glyph->bbox[0] = (int)bbox.xMin;
	glyph->bbox[1] = (int)bbox.yMin;
	glyph->bbox[2] = (int)bbox.xMax;
	glyph->bbox[3] = (int)bbox.yMax;
	return FONT_ERR_OK;
}

int glyph_curves(const font f[1], glyph_object glyph[1]) {
	if (f->ftptr == NULL) {
		debug("invalid font object");
		return FONT_ERR_NOFONT;
	}
	FT_Face face = *(FT_Face*)(f->ftptr);
	if (face->glyph->glyph_index == 0) {
		debug("no valid glyph loaded");
		return FONT_ERR_BADGLYPH;
	}
	FT_Outline outline = face->glyph->outline;

	unsigned ncurves = glyph->num_paths;
	if (ncurves < 1) {
		debug("glyph defines no curves");
		return FONT_ERR_GLYPH_CURVES;
	}

	glyph->paths = calloc(ncurves, sizeof(unsigned));
	if (glyph->paths == NULL) {
		debug("failed to allocate memory for glyph: %u paths", ncurves);
		return FONT_ERR_GLYPH_CURVES;
	}

	for (short i=0; i < ncurves; i++) {
		glyph->paths[i] = outline.contours[i];
	}

	return FONT_ERR_OK;
}

int glyph_points(const font f[1], glyph_object glyph[1], const size_t start, const size_t end) {
	if (f->ftptr == NULL) {
		debug("invalid font object");
		return FONT_ERR_NOFONT;
	}
	FT_Face face = *(FT_Face*)(f->ftptr);
	if (face->glyph->glyph_index == 0) {
		debug("no valid glyph loaded");
		return FONT_ERR_BADGLYPH;
	}
	FT_Outline outline = face->glyph->outline;

	glyph->points = calloc(glyph->num_points, sizeof(Vector3i));
	if (glyph->points == NULL) {
		debug("failed to allocate memory for glyph_points");
		return FONT_ERR_GLYPH_POINTS;
	}

	size_t e = end ? end + 1 : glyph->num_points;
	for (size_t i=start; i < e; i++) {
		glyph->points[i][0] = outline.points[i].x;
		glyph->points[i][1] = outline.points[i].y;
		glyph->points[i][2] = ((outline.tags[i] & 0x1) == 1) ? point_tCurve : point_tControl;
	}

	return FONT_ERR_OK;
}

int glyph_controlpoints(const font f[1], glyph_object glyph[1], Vector2i v[1], short start, short end) {
	if (f->ftptr == NULL) {
		debug("invalid font object");
		return FONT_ERR_NOFONT;
	}
	FT_Face face = *(FT_Face*)(f->ftptr);
	if (face->glyph->glyph_index == 0) {
		debug("no valid glyph loaded");
		return FONT_ERR_BADGLYPH;
	}
	FT_Outline outline = face->glyph->outline;

	unsigned e = end ? end + 1 : glyph->num_points;
	if (e < 1) {
		debug("invalid number of points: %u", e);
		return FONT_ERR_GLYPH_POINTS;
	}
	unsigned idx = 0;
	for (short i=start; i < e; i++) {
		if ((outline.tags[i] & 0x1) == 0) {
			v[idx][0] = outline.points[i].x;
			v[idx][1] = outline.points[i].y;
			idx++;
		}
	}

	return FONT_ERR_OK;
}

int glyph_curvepoints(const font f[1], glyph_object glyph[1], Vector2i v[1], short start, short end) {
	if (f->ftptr == NULL) {
		debug("invalid font object");
		return FONT_ERR_NOFONT;
	}
	FT_Face face = *(FT_Face*)(f->ftptr);
	if (face->glyph->glyph_index == 0) {
		debug("no valid glyph loaded");
		return FONT_ERR_BADGLYPH;
	}
	FT_Outline outline = face->glyph->outline;

	unsigned e = end ? end + 1 : glyph->num_points;
	if (e < 1) {
		debug("invalid number of points: %u", e);
		return FONT_ERR_GLYPH_POINTS;
	}
	unsigned idx = 0;
	for (short i=start; i < e; i++) {
		if ((outline.tags[i] & 0x1) == 1) {
			v[idx][0] = outline.points[i].x;
			v[idx][1] = outline.points[i].y;
			idx++;
		}
	}

	return FONT_ERR_OK;
}

int glyph_done(glyph_object *glyph) {
	if (glyph->points != NULL)
		free(glyph->points);
	if (glyph->paths != NULL)
		free(glyph->paths);
	return 0;
}

int font_init(font fontobj[1], const char *font_file) {
	if (fontobj == NULL)
		return FONT_ERR_NOFONT;
	if (fontobj->ftptr != NULL)
		return FONT_ERR_FONTEXISTS;
	int err = 0;
	if (ft == 0) {
		err = FT_Init_FreeType(&ft);
		if (err) {
			debug("Error loading freetype2: 0x%x", err);
			return FONT_ERR_LOADFT;
		}
	}

	FT_Face *f = calloc(1, sizeof(FT_Face));
	fontobj->ftptr = f;
	err = FT_New_Face(ft, font_file, 0, f);
	if (err) {
		debug("error loading font: %s: %#x", font_file, err);
		return FONT_ERR_LOADFONT;
	}
	if (! (FT_IS_SFNT(*f) && FT_IS_SCALABLE(*f))) {
		debug("font file is of unsupported type");
		return FONT_ERR_LOADFONT;
	}
	TT_MaxProfile *tt = (TT_MaxProfile *)FT_Get_Sfnt_Table(*f, FT_SFNT_MAXP);
	if (tt == NULL) {
		debug("failed to load TrueType table data");
		return FONT_ERR_LOADFONT;
	}

	err = FT_Select_Charmap(*f, FT_ENCODING_UNICODE);
	if (err) {
		debug("error setting font charmap: 0x%x", err);
		return FONT_ERR_SETCHARMAP;
	}

	fontobj->max_points = tt->maxPoints;
	debug("FONT TT MAX POINTS: %zu", fontobj->max_points);

	fontobj->max_bbox[0] = (*f)->bbox.xMin;
	fontobj->max_bbox[1] = (*f)->bbox.yMin;
	fontobj->max_bbox[2] = (*f)->bbox.xMax;
	fontobj->max_bbox[3] = (*f)->bbox.yMax;

	fontobj->emsize = (*f)->units_per_EM;
	fontobj->lineheight = (*f)->height;
	fontobj->max_advance = (*f)->max_advance_width;
	fontobj->space_width = fontobj->max_advance;
	fontobj->ptsize = 12.0f;
	fontobj->glyph_units = 64.0f;

	debug("Font loaded:"
			"\n\tface name: %s"
			"\n\ttotal number of glyphs: %lu"
			"\n\tEM size: %zu"
			"\n\tline height: %zu"
			"\n\tmax advance: %zu"
			"\n\tmax bbox: (%d,%d) (%d,%d)"
			,(*f)->family_name
			,(*f)->num_glyphs
			,fontobj->emsize
			,fontobj->lineheight
			,fontobj->max_advance
			,fontobj->max_bbox[0] ,fontobj->max_bbox[1] ,fontobj->max_bbox[2] ,fontobj->max_bbox[3]
			);

	return FONT_ERR_OK;
}

int font_done(font f[1]) {
	int err = 0;
	if (f->ftptr == NULL)
		return FONT_ERR_NOFONT;

	render_object *gs = &f->glyphslot;
	if (gs->attributes[0].data != NULL) {
		free(gs->attributes[0].data);
	}
	if (gs->attributes != NULL) {
		free(gs->attributes);
	}
	if (gs->buffers != NULL) {
		glDeleteBuffers(gs->num_attributes, gs->buffers);
		glDeleteVertexArrays(1, &gs->vertex_array);
		free(gs->buffers);
	}

	err = FT_Done_Face(*(FT_Face *)(f->ftptr));
	if (err) {
		debug("failed to destruct font object");
		return FONT_ERR_UNLOADFONT;
	}
	free(f->ftptr);
	f->ftptr = NULL;
	err = FT_Done_FreeType(ft);
	if (err) {
		debug("failed to unload freetype library instance");
		return FONT_ERR_UNLOADFT;
	}
	return FONT_ERR_OK;
}

font* font_new(const char *fontfile) {
	font *f = (font *)calloc(1, sizeof(font));
	if (f == NULL) {
		debug("failed to allocate a font object");
		return f;
	}
	render_object *gs = &(f->glyphslot);
	gs->num_attributes = 1;
	gs->attributes = calloc(1, sizeof(vertex_attribute));
	if (gs->attributes == NULL) {
		debug("failed to allocate memory for glyphslot data buffer");
		return 0;
	}
	gs->attributes[0].type = GL_INT;
	gs->attributes[0].components = 2;
	gs->attributes[0].normalize = GL_FALSE;

	if (font_init(f, fontfile) != FONT_ERR_OK) {
		debug("failed to initialize font object");
		font_done(f);
		free(gs->attributes);
		free(f);
		return 0;
	}

	gs->attributes[0].data_size = f->max_points * sizeof(Vector2i);
	gs->attributes[0].data = calloc(f->max_points, sizeof(Vector2i));
	if (gs->attributes[0].data == NULL) {
		debug("failed to allocate memory for glyphslot data buffer");
		font_done(f);
		free(gs->attributes);
		free(gs->attributes[0].data);
		free(f);
		return 0;
	}

	shader_program(gs, "font-vertex.glsl", "font-fragment.glsl");
	GLuint emszloc = glGetUniformLocation(gs->shader, "glyph_emsize");
	if (emszloc != -1) {
		glUniform1i(emszloc, f->emsize);
		CHKGLERR("glUniform1i (emsize)");
	}
	font_ptsize(f, f->ptsize);
	font_glyphunits(f, f->glyph_units);
	font_color(f, (Vector4f){1.0, 1.0, 1.0, 1.0});

	glUseProgram(gs->shader);
	CHKGLERR("glUseProgram (%u)", gs->shader);
	glGenVertexArrays(1, &(gs->vertex_array));
	CHKGLERR("glGenVertexArrays");
	glBindVertexArray(gs->vertex_array);
	CHKGLERR("glBindVertexArray");
	if (gs->buffers == NULL) {
		gs->buffers = calloc(gs->num_attributes, sizeof(GLuint));
	}
	glGenBuffers(gs->num_attributes, gs->buffers);
	CHKGLERR("glGenBuffers");
	glBindBuffer(GL_ARRAY_BUFFER, gs->buffers[0]);
	CHKGLERR("glBindBuffer (attribute: %u)", 0);
	glBufferData(GL_ARRAY_BUFFER, gs->attributes[0].data_size, gs->attributes[0].data, GL_DYNAMIC_DRAW);
	CHKGLERR("glBufferData (attribute: %u)", 0);
	glEnableVertexAttribArray(0);
	CHKGLERR("glEnableVertexAttribArray (attribute: %u)", 0);
	glVertexAttribIPointer(0, gs->attributes[0].components, gs->attributes[0].type, gs->attributes[0].stride, (void *)gs->attributes[0].offset);
	CHKGLERR("glVertexAttribIPointer (attribute: %u)", 0);

	return f;
}

int font_ptsize(font f[1], const float point_size) {
	f->ptsize = point_size;
	glUseProgram(f->glyphslot.shader);
	GLint loc = glGetUniformLocation(f->glyphslot.shader, "glyph_ptsize");
	if (loc == -1)
		return 0;
	glUniform1f(loc, point_size);
	return 1;
}

int font_glyphunits(font f[1], const float glyph_units) {
	f->glyph_units = glyph_units;
	glUseProgram(f->glyphslot.shader);
	GLint loc = glGetUniformLocation(f->glyphslot.shader, "glyph_units");
	if (loc == -1)
		return 0;
	glUniform1f(loc, glyph_units);
	return 1;
}

int font_resolution2f(font f[1], const float width, const float height) {
	glUseProgram(f->glyphslot.shader);
	GLint loc = glGetUniformLocation(f->glyphslot.shader, "glyph_resolution");
	if (loc == -1)
		return 0;
	glUniform2iv(loc, 1, (Vector2i){width, height});
	return 1;
}

int font_resolution2iv(font f[1], const Vector2i resolution) {
	glUseProgram(f->glyphslot.shader);
	GLint loc = glGetUniformLocation(f->glyphslot.shader, "glyph_resolution");
	if (loc == -1)
		return 0;
	glUniform2iv(loc, 1, resolution);
	return 1;
}

int font_color(font f[1], const Vector4f color) {
	glUseProgram(f->glyphslot.shader);
	GLint loc = glGetUniformLocation(f->glyphslot.shader, "glyph_color");
	if (loc == -1)
		return 0;
	glUniform4fv(loc, 1, color);
	return 1;
}

int font_bbox(font f[1], const Vector4i bbox) {
	glUseProgram(f->glyphslot.shader);
	GLint loc = glGetUniformLocation(f->glyphslot.shader, "glyph_bbox");
	if (loc == -1)
		return 0;
	glUniform4iv(loc, 1, bbox);
	return 1;
}

int font_advance(font f[1], const float advance) {
	glUseProgram(f->glyphslot.shader);
	GLint loc = glGetUniformLocation(f->glyphslot.shader, "glyph_advance");
	if (loc == -1)
		return 0;
	glUniform1f(loc, advance);
	return 1;
}

int font_leftbearing(font f[1], const float leftbearing) {
	glUseProgram(f->glyphslot.shader);
	GLint loc = glGetUniformLocation(f->glyphslot.shader, "glyph_leftbearing");
	if (loc == -1)
		return 0;
	glUniform1f(loc, leftbearing);
	return 1;
}

int font_topbearing(font f[1], const float topbearing) {
	glUseProgram(f->glyphslot.shader);
	GLint loc = glGetUniformLocation(f->glyphslot.shader, "glyph_topbearing");
	if (loc == -1)
		return 0;
	glUniform1f(loc, topbearing);
	return 1;
}

int glyph_load (font f[1], glyph_object glyph[1], uint32_t f_character) {
	memset(glyph, 0, sizeof(glyph_object));
	glyph->utf8char = f_character;
	glyph->bbox[0] = INT_MIN;
	glyph->bbox[1] = INT_MIN;
	glyph->bbox[2] = INT_MAX;
	glyph->bbox[3] = INT_MAX;

	int err = glyph_init(f, glyph);
	if (err != FONT_ERR_OK) {
		/*
		char s[5] = {
			((char *)&f_character)[0],
			((char *)&f_character)[1],
			((char *)&f_character)[2],
			((char *)&f_character)[3],
			0
		};
		debug("failed to load glyph '%s': %d", s, err);
		*/
		return err;
	}

	glyph_gl(f, glyph);

	font_bbox(f, glyph->bbox);
	font_advance(f, glyph->advance);
	/*
	font_leftbearing(f, glyph->leftbearing);
	font_rightbearing(f, glyph->rightbearing);
	*/

	return FONT_ERR_OK;
}

void glyph_gl(font f[1], glyph_object glyph[1]) {
	render_object *glyphslot = &(f->glyphslot);

	size_t numpoints = glyph->num_points;
	Vector2i points[numpoints];
	for (unsigned i=0; i < numpoints; i++) {
		points[i][0] = glyph->points[i][0];
		points[i][1] = glyph->points[i][1];
	}

	vertex_attribute *pos_attr = glyphslot->attributes[0].data;
	if (pos_attr != NULL) {
		memset(pos_attr, 0, sizeof(render_object));
	}
	glyphslot->num_vertices = numpoints;
	memcpy(pos_attr, points, numpoints);

	CHKGLERR(); // clear the error buffer first
	glBindBuffer(GL_ARRAY_BUFFER, glyphslot->buffers[0]);
	CHKGLERR("glBindBuffer (glyphslot->buffers[0])");
	glBufferSubData(GL_ARRAY_BUFFER, 0, numpoints * sizeof(Vector2i), points);
	CHKGLERR("glBufferSubData (glyphslot->buffers[0])");
}

int render_string (font f[1], const char *ustring, const Vector2f origin, float scale, Vector4f color) {
	if (f == NULL) {
		debug("invalid font object");
		return -1;
	}
	if (ustring == NULL) {
		debug("invalid text string");
		return -2;
	}
	static Matrix mvpmat={0}, m={0};
	matrix_Identity(mvpmat);

	matrix_Translate3f(m, origin[0], origin[1], 0.0);
	matrix_Mult(mvpmat, mvpmat, m);

	matrix_Scale(m, scale);
	matrix_Mult(mvpmat, mvpmat, m);

	uint32_t *glyphset = NULL;
	size_t num_glyphs = {0};
	num_glyphs = str2glyphs(&glyphset, ustring);
	if (! num_glyphs) {
		debug("could not convert input string");
		return -3;
	};

	render_object *gs = &(f->glyphslot);
	glyph_object glyph = {0};

	font_color(f, color ? color : (Vector4f){1.0, 1.0, 1.0, 1.0});

	glDisable(GL_CULL_FACE);
	CHKGLERR("glDisable (GL_CULL_FACE)");
	size_t advance = 0;
	unsigned start = 0;
	for (unsigned i=0; i < num_glyphs; i++) {
		if (glyph_load(f, &glyph, glyphset[i]) != FONT_ERR_OK) {
			advance = f->space_width * 0.8;
			continue;
		}
		advance += glyph.advance;

		matrix_Translate3f(m,
				//(float)((advance + glyph.leftbearing) * (f->ptsize / (float)(f->emsize * f->glyph_units))),
				//(float)((advance - glyph.leftbearing) * (f->ptsize / (float)(f->emsize * f->glyph_units))),
				(float)((advance) * (f->ptsize / (float)(f->emsize * f->glyph_units))),
				0.0,
				0.0);
		matrix_Mult(mvpmat, mvpmat, m);

		GLuint vnum = gs->num_vertices;
		//render2d(gs, mvpmat, start, vnum - start, GL_POINTS);
		//render2d(gs, mvpmat, start, vnum - start, GL_LINES);
		render2d(gs, mvpmat, start, vnum - start, GL_LINE_STRIP);
		//render2d(gs, mvpmat, start, vnum - start, GL_LINE_LOOP);
		//render2d(gs, mvpmat, start, vnum - start, GL_TRIANGLES);
		//render2d(gs, mvpmat, start, vnum - start, GL_TRIANGLE_FAN);
		//render2d(gs, mvpmat, start, vnum - start, GL_TRIANGLE_STRIP);

		advance = 0.0f;
		glyph_done(&glyph);
	}
	glEnable(GL_CULL_FACE);
	CHKGLERR("glEnable (GL_CULL_FACE)");

	free(glyphset);
	glyphset = NULL;

	return 1;
}

