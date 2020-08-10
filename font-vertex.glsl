#version 330 core

out vec4 color;


uniform mat4 mvp;
uniform int glyph_emsize;
uniform float glyph_ptsize;
uniform float glyph_units;
uniform ivec4 glyph_bbox;
uniform float glyph_advance;
uniform ivec2 glyph_resolution;
uniform vec4 glyph_color;


layout(location = 0) in ivec2 vpos;


void main()
{
	float scale = glyph_ptsize / (glyph_emsize * glyph_units);
	gl_Position = mvp * vec4(vpos.xy * scale, -1.0, 1.0);
	//gl_Position = mvp * vec4(vpos.xy / glyph_resolution.xy * glyph_ptsize / glyph_units, -1.0, 1.0);
	color = vec4(glyph_color.xyzw);
}
