LVG (Lion Vector Graphics)
==========

[![Build Status](https://travis-ci.org/lieff/lvg.svg)](https://travis-ci.org/lieff/lvg)
[![Build Status](https://ci.appveyor.com/api/projects/status/69br8ic9dwlfeu48?svg=true)](https://ci.appveyor.com/project/lieff/lvg)

LVG is lightweight replacement for flash. Basic idea: we have some assets in package and sctipt that renders them.
It's designed to use very small runtime (around 200kb without video codecs) for player and can run on android, ios, web and more.
It's uses C scripts natively and javascript for web.

Why C scripts? Because C compiler is small, and runtime written in C, so no other lagnuage needed and script can be compiled in to save the space.

Following assets currently supported:

 * SVG vector images (loaded using [nanoSVG](https://github.com/memononen/nanosvg))
 * Raster images (loaded using stb_image.h)
 * MP3 files
 * Flash SWF files (can contain vector, raster, video and audio)

Render can be done using following backends:

 * NVIDIA Path Rendering
 * [nanoVG](https://github.com/memononen/nanovg)

## Current Status:

Works:

 * SVG, image, mp3 resources with tcc scripting.
 * limited SWF support.
 * SWF: shapes, images, video, audio (mp3, adpcm, pcm) loading and rendering (some errors in converting to SVG shape fornat).
 * SWF: load action script

Not works:

 * SWF: morph shapes (only first shape showed).
 * SWF: clip layers
 * SWF: interpret action script 3
 * SWF: bitmap filters
 * SWF: network access
 * SWF: audio mixer

[Tiger demo](https://lieff.github.io/lvg_player.html?file=test.lvg)

[Nanovg demo](https://lieff.github.io/lvg_player.html?file=test_nanovg.lvg)

[Nuklear demo](https://lieff.github.io/lvg_player.html?file=test_nuklear_gles2.lvg)

![Screenshot](svg_tiger.png?raw=true "demo")

## Building

For windows and linux install all build dependencies with the following command:

```
apt-get install -qq -y git mercurial cmake yasm libglfw3-dev libsdl2-dev libavcodec-dev mingw-w64 wget
```

For macos brew must be installed:

```
brew install upx glfw sdl2 ffmpeg tcc
```

LVG uses meson as build system. Actual building with the following commands in project directory:

```
mkdir build
cd build
meson ..
ninja
```

## Interesting graphics libraries

 * https://github.com/01org/fastuidraw
 * https://skia.org/
 * https://cairographics.org/
 * http://www.antigrain.com/
 * http://scripts.sil.org/cms/scripts/page.php?site_id=projects&item_id=graphite_home
 * https://sourceforge.net/projects/shivavg/
 * https://github.com/micahpearlman/MonkVG
 * https://github.com/KhronosGroup/OpenVG-Registry

## Flash related projects:

 * https://www.gnu.org/software/gnash/
 * https://swfdec.freedesktop.org/wiki/
 * http://www.swftools.org/
 * http://www.libming.org/

LVG System is developed and maintained by https://github.com/lieff
