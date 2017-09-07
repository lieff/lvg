// makeswf -v 7 -s 200x150 -r 1 -o localToGlobal-propflags.swf localToGlobal-propflags.as

createEmptyMovieClip ("a", 0);
a._x = 50;
a._y = 50;

for (var i = 0; i < 16384; i += 8) {
  var x = { x: 0, y: 0 };
  ASSetPropFlags (x, null, i);
  a.localToGlobal (x);
  trace (i + ": " + x.x + ", " + x.y);
  ASSetPropFlags (x, null, 0, i);
  trace (i + ": " + x.x + ", " + x.y);
};

loadMovie ("fscommand:quit", "");
