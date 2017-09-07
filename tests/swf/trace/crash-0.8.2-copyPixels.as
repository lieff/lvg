// makeswf -v 7 -s 200x150 -r 1 -o crash-0.8.2-copyPixels.swf crash-0.8.2-copyPixels.as

bd = new flash.display.BitmapData (50, 50);
bd.copyPixels (bd, {}, {}, null);
bd.copyPixels (bd, {}, {}, bd, {}, null);

getURL ("fscommand:quit", "");
