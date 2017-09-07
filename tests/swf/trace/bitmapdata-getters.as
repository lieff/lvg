// makeswf -v 7 -s 200x150 -r 1 -o bitmapdata-getters.swf bitmapdata-getters.as

function show (bd) {
  trace ("displaying BitmapData " + bd);
  trace (bd.width);
  trace (bd.height);
  trace (bd.transparent);
  trace (bd.rectangle);
};

bd = new flash.display.BitmapData (2880, 2880, true, 0xff);
show (bd);
flash.geom.Rectangle = function (x, y, w, h) {
  trace (arguments);
  this.x = x;
  this.y = y;
  this.width = w;
  this.height = h;
};
show (bd);
show (bd);
flash.geom.Rectangle = 42;
show (bd);
_global.flash = 42;
show (bd);
bd.dispose ();
show (bd);

show (new flash.display.BitmapData (0, 10, true, 0xff));
show (new flash.display.BitmapData (-1, 10, false, 0xff));

getURL ("fscommand:quit", "");

