// makeswf -v 7 -s 200x150 -r 1 -o crash-0.7.5-draw-on-disposed-bitmap.swf crash-0.7.5-draw-on-disposed-bitmap.as

createEmptyMovieClip ("a", 0);
bd = new flash.display.BitmapData (50, 50);
bd.dispose ();
bd.draw (a);

getURL ("fscommand:quit", "");
