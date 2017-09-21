LVG (Lion Vector Graphics)
==========

[![Build Status](https://travis-ci.org/lieff/lvg.svg)](https://travis-ci.org/lieff/lvg)
[![Build Status](https://ci.appveyor.com/api/projects/status/69br8ic9dwlfeu48?svg=true)](https://ci.appveyor.com/project/lieff/lvg)

LVG is lightweight replacement for flash. Basic idea: we have some assets in package and sctipt that renders them.
It's designed to use very small runtime (around 200kb without video codecs) for player and can run on android, ios, web and more.
It's uses C scripts natively and javascript for web.

Why C scripts? Because C compiler is small, and runtime written in C, so no other lagnuage needed and script can be compiled in to save the space.

Following assets currently supported:

 * SVG vector images (loaded using [NanoSVG](https://github.com/memononen/nanosvg))
 * Raster images (loaded using [stb_image.h](https://github.com/nothings/stb))
 * MP3 files
 * Flash SWF files (can contain vector, raster, video and audio). All swf video codecs (h263,vp6,flashsv,flashsv2,h264) costs additional ~600kb.

Render can be done using following backends:

 * [NVIDIA Path Rendering](https://developer.nvidia.com/nv-path-rendering)
 * [NanoVG](https://github.com/memononen/nanovg)

## Current Status

Works:

 * SVG, image, mp3 resources with tcc scripting.
 * limited SWF support.
 * SWF: shapes, images, video, audio (mp3, adpcm, pcm) loading and rendering (some errors in converting to SVG shape fornat).
 * SWF: load action script

Not works:

 * SWF: clip layers
 * SWF: interpret action script 3
 * SWF: bitmap filters
 * SWF: network access
 * SWF: audio mixer

## Demos

Basic svg demo (images clickable):

[![SVG Screenshot](images/svg_tiger.png?raw=true)](https://lieff.github.io/lvg_player.html?file=test.lvg)

NanoVG demo incapsulated in lvg:

[![NanoVG Screenshot](images/nanovg.png?raw=true)](https://lieff.github.io/lvg_player.html?file=test_nanovg.lvg)

Nuklear GUI demo:

[![Nuklear Screenshot](images/nuklear.png?raw=true)](https://lieff.github.io/lvg_player.html?file=test_nuklear_gles2.lvg)

Flash swf demo:

[![Flash Screenshot](images/kitty.png?raw=true)](https://lieff.github.io/lvg_player2.html?file=npc_kitty_chicken.swf)

## Building

For now windows executables builds only on linux host using [mingw](https://mingw-w64.org/).
For windows and linux install all build dependencies with the following command:

```
apt-get install -qq -y git mercurial cmake meson yasm libglfw3-dev libsdl2-dev libavcodec-dev mingw-w64 wget
```

For macos brew must be installed:

```
brew install meson upx glfw sdl2 ffmpeg tcc
```

LVG uses meson as build system. Actual building with the following commands in project directory:

```
mkdir build
cd build
meson ..
ninja
```

## Future

 * Use other scripting lanuages like [luajit](http://luajit.org/), [JerryScript](https://github.com/jerryscript-project/jerryscript), [muJS](http://artifex.com/mujs_/), [avian](https://readytalk.github.io/avian/), [ImpalaJIT](https://github.com/Manuel1605/ImpalaJIT) and [AngelScript](http://angelcode.com/angelscript/).
 * Speed up action script with jit like [sljit](http://sljit.sourceforge.net/).
 * Shaders and 3D support.
 * Parallel shape\image decoding and use file compression with parallel decoding support (like pbzip2).
 * More audio\video\codec backends.
 * SWF frame interpolation to get more fps than encoded in file.
 * Better than swf storage format using [draco](https://github.com/google/draco)-like compression.

## Interesting graphics libraries

 * https://github.com/01org/fastuidraw
 * https://skia.org/
 * https://cairographics.org/
 * http://www.antigrain.com/
 * http://scripts.sil.org/cms/scripts/page.php?site_id=projects&item_id=graphite_home
 * https://sourceforge.net/projects/shivavg/
 * https://github.com/micahpearlman/MonkVG
 * https://github.com/KhronosGroup/OpenVG-Registry

## Flash related projects

 * https://github.com/lightspark/lightspark
 * https://github.com/mozilla/shumway
 * https://www.gnu.org/software/gnash/
 * https://swfdec.freedesktop.org/wiki/
 * http://www.swftools.org/
 * http://www.libming.org/
 * http://www.swift-tools.net/Flash/
 * https://github.com/crossbridge-community/crossbridge
 * SWF v19 reference http://wwwimages.adobe.com/content/dam/Adobe/en/devnet/swf/pdf/swf-file-format-spec.pdf
 * AMF v0 referemce http://wwwimages.adobe.com/content/dam/Adobe/en/devnet/amf/pdf/amf0-file-format-specification.pdf
 * AMF v3 reference http://wwwimages.adobe.com/www.adobe.com/content/dam/Adobe/en/devnet/amf/pdf/amf-file-format-spec.pdf
 * AS2 reference http://help.adobe.com/en_US/AS2LCR/Flash_10.0/help.html

LVG System is developed and maintained by https://github.com/lieff
