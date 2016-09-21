LVG (Lion Vector Graphics)
==========

[nanoVG](https://github.com/memononen/nanovg) + [nanoSVG](https://github.com/memononen/nanosvg) + [glfw3](http://www.glfw.org/) + [libtcc](http://repo.or.cz/tinycc.git) + [Emscripten](https://github.com/kripken/emscripten) =
LVG is lightweight replacement for flash.
It's designed to use very small runtime (around 200kb) for player and can run on android, ios, web and more.
It's uses C scripts natively and javascript for web.

Why C scripts? Because C compiler is small, and runtime written in C, so no other lagnuage needed and script can be compiled in to save space.

[Demo:](https://lieff.github.io/lvg_player.html?file=test.lvg)

![Screenshot](svg_tiger.png?raw=true "demo")

Interesting graphics libraries:

 * https://github.com/01org/fastuidraw
 * https://skia.org/
 * https://cairographics.org/
 * http://www.antigrain.com/
 * http://scripts.sil.org/cms/scripts/page.php?site_id=projects&item_id=graphite_home

LVG System is developed and maintained by https://github.com/lieff
