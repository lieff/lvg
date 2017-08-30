LVG (Lion Vector Graphics)
==========

[![Build Status](https://travis-ci.org/lieff/lvg.svg)](https://travis-ci.org/lieff/lvg)
[![Build Status](https://ci.appveyor.com/api/projects/status/69br8ic9dwlfeu48?svg=true)](https://ci.appveyor.com/project/lieff/lvg)

[nanoVG](https://github.com/memononen/nanovg) + [nanoSVG](https://github.com/memononen/nanosvg) + [glfw3](http://www.glfw.org/) + [libtcc](http://repo.or.cz/tinycc.git) + [Emscripten](https://github.com/kripken/emscripten) =
LVG is lightweight replacement for flash.
It's designed to use very small runtime (around 200kb) for player and can run on android, ios, web and more.
It's uses C scripts natively and javascript for web.

Why C scripts? Because C compiler is small, and runtime written in C, so no other lagnuage needed and script can be compiled in to save space.

Current Status:
Works:

 * SVG, image, mp3 resources with tcc scripting.
 * limited SWF support.
 * SWF: shapes, images, video, audio (mp3, adpcm, pcm) loading and rendering (some errors in converting to SVG shape fornat).
 * SWF: load action script

Not works:

 * SWF: morph shapes (only first shape showed).
 * SWF: clip layers
 * SWF: interpret action script
 * SWF: bitmap filters
 * SWF: network access
 * SWF: audio mixer

[Tiger demo](https://lieff.github.io/lvg_player.html?file=test.lvg)

[Nanovg demo](https://lieff.github.io/lvg_player.html?file=test_nanovg.lvg)

[Nuklear demo](https://lieff.github.io/lvg_player.html?file=test_nuklear_gles2.lvg)

![Screenshot](svg_tiger.png?raw=true "demo")

Interesting graphics libraries:

 * https://github.com/01org/fastuidraw
 * https://skia.org/
 * https://cairographics.org/
 * http://www.antigrain.com/
 * http://scripts.sil.org/cms/scripts/page.php?site_id=projects&item_id=graphite_home
 * https://sourceforge.net/projects/shivavg/
 * https://github.com/micahpearlman/MonkVG
 * https://github.com/KhronosGroup/OpenVG-Registry

LVG System is developed and maintained by https://github.com/lieff
