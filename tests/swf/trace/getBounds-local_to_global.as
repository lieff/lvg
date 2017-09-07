// makeswf -v 7 -s 200x150 -r 1 -o getBounds-local_to_global.swf getBounds-local_to_global.as

trace ("This exposes a bug in the internal matrix cache update routines, where some matrix conversion function did not ensure the matrix was properly updated. This might explain why the test looks so constructed.");

rectangle = function (mc, color, x, y, w, h)
{
  mc.beginFill (color);
  mc.moveTo (x, y);
  mc.lineTo (x, y + h);
  mc.lineTo (x + w, y + h);
  mc.lineTo (x + w, y);
  mc.lineTo (x, y);
  mc.endFill ();
};

createEmptyMovieClip ("a", 0);
a.createEmptyMovieClip ("a", 0);
rectangle (a.a, 0xFF, 0, 0, 100, 100);
a.getBounds ();
a._xscale = 50;

createEmptyMovieClip ("b", 1);
rectangle (b, 0xFF, 0, 0, 100, 100);
b._xscale = 200;

bounds = a.a.getBounds (b);
trace (a + ".getBounds (" + b + ") = { " + bounds.xMin + ", " + bounds.yMin + ", " + bounds.xMax +", " + bounds.yMax + "}");

getURL ("fscommand:quit", "");
