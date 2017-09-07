// makeswf -v 7 -r 1 -o point-7.swf point.as

// enable flash structure for version < 8 too for this test
ASSetPropFlags (_global, "flash", 0, 4096);

var points = [];
points.push (new flash.geom.Point ());
points.push (new flash.geom.Point (1, 2));
points.push (new flash.geom.Point (3.234, -9.21));
points.push (new flash.geom.Point (5, -1000000));
points.push (new flash.geom.Point (40, NaN));
points.push (new flash.geom.Point (50, -Infinity));
points.push (new flash.geom.Point (Infinity, 0));

for (var i = 0; i < points.length; i++) {
  trace ("Testing:");
  trace (points[i]);
  trace ("length: " + points[i].length);
  trace ("clone: " + points[i].clone ());
  trace ("normalize: " + points[i].clone ().normalize (34.5));
  trace ("offset: " + points[i].clone ().offset (34.5, -43));
  for (var j = i; j < points.length; j++) {
    trace ("distance " + i + ": " + flash.geom.Point.distance (points[i], points[j]));
    trace ("interpolate " + i + ": " + flash.geom.Point.interpolate (points[i], points[j], 0.68));
    trace ("add " + i + ": " + points[i].add (points[j]));
    trace ("equals " + i + ": " + points[i].equals (points[j]));
    trace ("substract " + i + ": " + points[i].substract (points[j]));
  }
}

loadMovie ("FSCommand:quit", "");
