LVG (Lion Vector Graphics)
==========

[![Build Status](https://travis-ci.org/lieff/lvg.svg)](https://travis-ci.org/lieff/lvg)
[![CircleCI](https://circleci.com/gh/lieff/lvg.svg?style=svg)](https://circleci.com/gh/lieff/lvg)
[![Build Status](https://ci.appveyor.com/api/projects/status/69br8ic9dwlfeu48?svg=true)](https://ci.appveyor.com/project/lieff/lvg)
<a href="https://scan.coverity.com/projects/lieff-lvg">
  <img alt="Coverity Scan Build Status"
       src="https://scan.coverity.com/projects/14454/badge.svg"/>
</a>

LVG is a lightweight flash player replacement. Basic idea: we have some assets in a package and sctipts that renders them.
It's designed to use a very small runtime (around 200kb without video codecs) for player and runs on android, ios, web and more.

Currently it uses C script to load and render assets.
Why C script? Because C compiler is small, and runtime written in C, so no other language is needed and script can be compiled in to a save the space.
Another reason is compatibility: lvg only exposes small API and opengl, with other scripting laguages we must do all from scratch, including GUI.
But there are huge amount of already-written C code.
Usually author prefer C++, but there also some reasons to use pure C for whole project:

 * We do not need to include libstdc++ for Android shared objects, which saves some .apk space (shared objects libs also usually duplicated for each supported architectue in .apk).
 * This reduces size of emscripten compiled WASM code (it's already relatively heavy).
 * Native C++ builds needs some attention for small code too. It's definetly possible to write small code with C++, but we need something like minicrt for each platform/compiler. See [farbrauch](https://github.com/farbrausch/fr_public) code for example or something similar.
 * C++ code produces much more noisy .map files which makes profile/map file analysis bit more complicated. Projects like chromium also uses objects analysis tools (like dump-static-initializers.py), there also less noise with such tools.
 * Code can be ported to specific OS'es more easily.

Following assets currently supported:

 * SVG vector images (loaded using [NanoSVG](https://github.com/memononen/nanosvg))
 * Raster images (loaded using [stb_image.h](https://github.com/nothings/stb))
 * MP3 files (loaded using [minimp3](https://github.com/lieff/minimp3))
 * Flash SWF files (can contain vector, raster, video and audio). All swf video codecs (h263,vp6,flashsv,flashsv2,h264) costs additional ~600kb.
 * Scripts using [tcc](https://bellard.org/tcc/) or [picoc](https://gitlab.com/zsaleeba/picoc)

Render can be done using following backends:

 * [NVIDIA Path Rendering](https://developer.nvidia.com/nv-path-rendering)
 * [NanoVG](https://github.com/memononen/nanovg)

## Current Status

Works:

 * SVG, image, mp3 resources with tcc scripting.
 * SWF: shapes, images, video, audio (mp3, adpcm, pcm), limited action script 2 support, limited morph shapes support.

Not works:

 * SWF: clip layers
 * SWF: interpret action script 3
 * SWF: bitmap filters
 * SWF: network access

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
apt-get install -qq -y git cmake meson yasm libglfw3-dev libsdl2-dev libavcodec-dev mingw-w64 wget
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

## Configuring

LVG can be configured to use different backends for platform, audio and video. You can see current configuration by executing mesonconf (or meson configure for newer meson versions) in build directory:

```
mesonconf
...
Project options:
  Option        Description   Current Value Possible Values
  ------        -----------   ------------- ---------------
  AUDIO_SDL     AUDIO_SDL     true          [True, False]
  ENABLE_AUDIO  ENABLE_AUDIO  true          [True, False]
  ENABLE_SCRIPT ENABLE_SCRIPT true          [True, False]
  ENABLE_VIDEO  ENABLE_VIDEO  true          [True, False]
  PLATFORM_GLFW PLATFORM_GLFW true          [True, False]
  PLATFORM_SDL  PLATFORM_SDL  false         [True, False]
  RENDER_NANOVG RENDER_NANOVG true          [True, False]
  RENDER_NVPR   RENDER_NVPR   true          [True, False]
  SCRIPT_TCC    SCRIPT_TCC    true          [True, False]
  VIDEO_FFMPEG  VIDEO_FFMPEG  true          [True, False]
...
```

You can change configuration using meson commands. Example:

```
mesonconf -DENABLE_SCRIPT=False
```

## Future

 * Use other scripting lanuages like [luajit](http://luajit.org/), [JerryScript](https://github.com/jerryscript-project/jerryscript), [muJS](http://artifex.com/mujs/), [mu](https://github.com/geky/mu), [avian](https://readytalk.github.io/avian/), [ImpalaJIT](https://github.com/Manuel1605/ImpalaJIT), [mochascript](https://github.com/Piot/mochascript) and [AngelScript](http://angelcode.com/angelscript/).
 * Speed up action script with jit like [sljit](http://sljit.sourceforge.net/).
 * Shaders and 3D support.
 * Parallel shape\image decoding and use file compression with parallel decoding support (like pbzip2).
 * More audio\video\codec backends.
 * ~~SWF frame interpolation to get more fps than encoded in file.~~ [Done]
 * Better than swf storage format using [draco](https://github.com/google/draco)-like compression based on [ANS](https://en.wikipedia.org/wiki/Asymmetric_numeral_systems).

## Interesting graphics libraries

 * https://github.com/01org/fastuidraw
 * https://skia.org/
 * https://github.com/blend2d/blend2d/
 * https://github.com/memononen/nanovg
 * https://github.com/mokafolio/Tarp
 * https://github.com/jpbruyere/vkvg
 * https://github.com/jdryg/vg-renderer
 * https://github.com/nical/lyon
 * https://cairographics.org/
 * http://www.antigrain.com/
 * http://scripts.sil.org/cms/scripts/page.php?site_id=projects&item_id=graphite_home
 * https://sourceforge.net/projects/shivavg/
 * https://github.com/micahpearlman/MonkVG
 * https://github.com/KhronosGroup/OpenVG-Registry

## Flash related projects

 * https://github.com/lightspark/lightspark
 * https://github.com/mozilla/shumway
 * https://github.com/ruffle-rs/ruffle
 * https://www.gnu.org/software/gnash/
 * https://swfdec.freedesktop.org/wiki/
 * https://github.com/marmalade/gameswf
 * http://tulrich.com/geekstuff/gameswf.html
 * http://www.swftools.org/
 * http://www.libming.org/
 * http://www.swift-tools.net/Flash/
 * https://github.com/crossbridge-community/crossbridge
 * SWF v19 reference https://wwwimages2.adobe.com/content/dam/acom/en/devnet/pdf/swf-file-format-spec.pdf
 * AMF v0 referemce https://wwwimages2.adobe.com/content/dam/acom/en/devnet/pdf/amf0-file-format-specification.pdf
 * AMF v3 reference https://wwwimages2.adobe.com/content/dam/acom/en/devnet/pdf/amf-file-format-spec.pdf
 * AS2 reference http://homepage.divms.uiowa.edu/~slonnegr/flash/ActionScript2Reference.pdf

LVG is developed and maintained by https://github.com/lieff
