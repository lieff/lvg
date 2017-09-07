// makeswf -v 7 -r 1 -o transform-color-transform-7.swf transform-color-transform.as

// enable flash structure for version < 8 too for this test
ASSetPropFlags (_global, "flash", 0, 5248);

var Transform = flash.geom.Transform;
var ColorTransform = flash.geom.ColorTransform;

this.createEmptyMovieClip ("a", 0);
a.createEmptyMovieClip ("b", 0);

var t = new Transform (this);
trace (t.colorTransform);

trace (a.transform.colorTransform);
a.transform.colorTransform =
  new ColorTransform (0.5, -1.5, 0.35452134151223, 3.5, 4.5, -500, 6.5, 700.5);
trace (a.transform.colorTransform);

trace (a.b.transform.colorTransform);
a.b.transform.colorTransform =
  new ColorTransform (0.75, 0.34, 0.53, 0.1, 34, -25, 103, 230);
trace (a.b.transform.colorTransform);

trace (t.concatenatedColorTransform);
trace (a.transform.concatenatedColorTransform);
trace (a.b.transform.concatenatedColorTransform);

var c = a.b.transform.colorTransform;
c.concat (a.transform.colorTransform);
c.concat (t.colorTransform);
trace (c);

getURL ("FSCommand:quit", "");
