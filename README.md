# gpu-glyph
An evaluation framework for glyph rendering algorithms on the GPU.

## What is this for?
This is a personal project to be used for validating and benchmarking different
algorithms for rendering text on the GPU. Currently the framework only enables
OpenGL as a backend, but support for Vulkan may also be added in the future.

## What algorithms does it cover?
Currently the framework only renders directly as GL_LINE_STRIP, though
depending on the font and size this is already surprisingly legible.

In future versions, signed distance fields (SDF), direct bezier interpolation
via winding as explained by Evan Wallace in
[Easy Scalable Text Rendering on the GPU](https://medium.com/@evanwallace/easy-scalable-text-rendering-on-the-gpu-c3f4d782c5ac),
direct glyph render to texture, texture atlases, and other techniques will be
explored and evaluated via comparison. I will also be evaluating differences in
techniques of glyph caching vs. direct interpolation and rendering.

## Usage
Currently the framework doesn't have the various algorithms for evaluation
implemented, so at this time it only offers a demo program that renders text
directly as GL_LINE_STRIP. The information for this section will change as
various glyph rendering algorithms are implemented for comparison.

To render text using the demo program, first the demo program has to be
compiled. It can either be compiled with or without debugging information and
symbols.

```bash
make all
```
or

```bash
make debug
```

Once the demo is built,

```bash
./demo /usr/share/fonts/TTF/FreeMono.ttf 'Test Message Here' 10
```

or, for the debug version,

```bash
./demo.debug /usr/share/fonts/TTF/FreeMono.ttf 'Test Message Here' 10
```

The first argument is the (truetype) font file to use for testing, the 2nd
argument is the text to render, and the 3rd argument is the point size at which
to render.
