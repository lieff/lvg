// makeswf -v 7 -r 1 -o movieclip-get-bounds-7.swf movieclip-get-bounds.as

// FIXME this test doesn't yet pass on version 8

var keys = ["yMax", "yMin", "xMax", "xMin"];

this.createEmptyMovieClip ("a", 1);
a._x = 10;
a._y = 10;
a.lineStyle (4);
a.moveTo (0, 0);
a.lineTo (100, 0);
a.lineTo (100, 100);
a.lineTo (0, 100);
a.lineTo (0, 0);

trace ("Testing: this");
var o = a.getBounds (this);
trace (o);
for (var i = 0; i < keys.length; i++) {
  trace (keys[i] + ": " + o[keys[i]]);
}
trace ("width, height: " + a._width + ", " + a._height);

trace ("Testing: a");
var o = a.getBounds (a);
trace (o);
for (var i = 0; i < keys.length; i++) {
  trace (keys[i] + ": " + o[keys[i]]);
}
trace ("width, height: " + a._width + ", " + a._height);

trace ("Testing: (no args)");
var o = a.getBounds ();
trace (o);
for (var i = 0; i < keys.length; i++) {
  trace (keys[i] + ": " + o[keys[i]]);
}
trace ("width, height: " + a._width + ", " + a._height);

trace ("Testing: 2");
var o = a.getBounds (2);
trace (o);
for (var i = 0; i < keys.length; i++) {
  trace (keys[i] + ": " + o[keys[i]]);
}
trace ("width, height: " + a._width + ", " + a._height);

loadMovie ("FSCommand:quit", "");
