// makeswf -v 7 -s 200x150 -r 1 -o color-transform-concat.swf color-transform-concat.as

// enable flash structure for version < 8 too for this test
ASSetPropFlags (_global, "flash", 0, 5248);

var ColorTransform = flash.geom.ColorTransform;

var c = new ColorTransform (0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5);
trace (c);
trace (c.concat (new ColorTransform (0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5)));
trace (c);
c.concat (new ColorTransform (0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5));
trace (c);
c.concat (new ColorTransform (-1, -2, -3, -4, -5, -6, -7, -8));
trace (c);

getURL ("fscommand:quit", "");
