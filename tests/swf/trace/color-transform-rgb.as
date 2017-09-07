// makeswf -v 7 -s 200x150 -r 1 -o color-transform-multipliers-offsets.swf color-transform-multipliers-offsets.as

#include "values.as"

// enable flash structure for version < 8 too for this test
ASSetPropFlags (_global, "flash", 0, 5248);

var ColorTransform = flash.geom.ColorTransform;

var c = new ColorTransform (0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5);

trace ("0x" + c.rgb.toString (16));

c.redOffset = -5000;
trace ("0x" + c.rgb.toString (16));
c.greenMultiplier = 0.8;
trace ("0x" + c.rgb.toString (16));

trace ("Testing: 0xFFFFFFFFFF");
c = new ColorTransform (0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5);
c.rgb = 0xFFFFFFFFFF;
trace (c);

trace ("Testing: -0xFFFFFFFFFF");
c = new ColorTransform (0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5);
c.rgb = -0xFFFFFFFFFF;
trace (c);

for (var i = 0; i < values.length; i++) {
  trace ("Testing: " + names[i]);
  c = new ColorTransform (0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5);
  c.rgb = values[i];
  trace (c);
  c = new ColorTransform (0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5);
  c.blueOffset = values[i];
  c.redAlpha = values[i];
  trace (c);
  trace (c.rgb);
}

c = new ColorTransform (0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5);

trace ("-- -1");
c.blueOffset = -1;
trace (c);
trace (c.rgb);

trace ("-- -42");
c.blueOffset = -42;
trace (c);
trace (c.rgb);

trace ("-- -1 -2 -3");
c.redOffset = -1;
c.greenOffset = -2;
c.blueOffset = -3;
trace (c);
trace (c.rgb);

trace ("-- -3 -2 -1");
c.redOffset = -3;
c.greenOffset = -2;
c.blueOffset = -1;
trace (c);
trace (c.rgb);

trace ("-- -1 -2 3");
c.redOffset = -1;
c.greenOffset = -2;
c.blueOffset = 3;
trace (c);
trace (c.rgb);

trace ("-- -1 2 3");
c.redOffset = -1;
c.greenOffset = 2;
c.blueOffset = 3;
trace (c);
trace (c.rgb);

trace ("-- 1 -2 3");
c.redOffset = 1;
c.greenOffset = -2;
c.blueOffset = 3;
trace (c);
trace (c.rgb);

getURL ("fscommand:quit", "");
