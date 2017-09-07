// makeswf -v 7 -s 200x150 -r 1 -o attachBitmap.swf attachBitmap.as

createEmptyMovieClip ("a", 0);
a.createEmptyMovieClip ("a", 0);
red = new flash.display.BitmapData (100, 100, false, 0xFF0000);
trace (a.a);
a.attachBitmap (red, 0);
trace (a.a);
trace (a.getInstanceAtDepth (0));

trace (a._width);
trace (a._height);

getURL ("fscommand:quit", "");
