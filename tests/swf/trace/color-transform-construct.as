// makeswf -v 7 -s 200x150 -r 1 -o color-transform-construct.swf color-transform-construct.as

#include "values.as"

// enable flash structure for version < 8 too for this test
ASSetPropFlags (_global, "flash", 0, 5248);

var ColorTransform = flash.geom.ColorTransform;

trace (new ColorTransform (0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5));

trace (new ColorTransform ());
trace (new ColorTransform (0.5));
trace (new ColorTransform (0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5));
trace (new ColorTransform (0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5, 8.5));

trace (new ColorTransform (-5.5, 1.5, 2.5, 3.5, -4.5, 5.5, 6.5, 7.5));
trace (new ColorTransform (-500.5, 1.5, 2.5, 3.5, -400.5, 5.5, 6.5, 7.5));
trace (new ColorTransform (500.5, 1.5, 2.5, 3.5, 400.5, 5.5, 6.5, 7.5));

for (var i = 0; i < values.length; i++) {
  trace ("Testing: " + names[i]);
  trace (new ColorTransform (0, values[i], 2, 3, 4, values[i], 6, 7));
}

getURL ("fscommand:quit", "");
