// makeswf -v 7 -s 200x150 -r 1 -o color-transform-multipliers-offsets.swf color-transform-multipliers-offsets.as

#include "values.as"

// enable flash structure for version < 8 too for this test
ASSetPropFlags (_global, "flash", 0, 5248);

var ColorTransform = flash.geom.ColorTransform;

function test (v) {
  var c = new ColorTransform (0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5);
  var properties = [
    "alphaMultiplier",
    "blueMultiplier",
    "greenMultiplier",
    "redMultiplier",
    "alphaOffset",
    "blueOffset",
    "greenOffset",
    "redOffset"
  ];

  for (var i = 0; i < properties.length; i++) {
    c[properties[i]] = v;
    trace (properties[i] + ": " + c[properties[i]]);
  }
}

trace ("Testing: -5000");
test (-5000);

trace ("Testing: 5000");
test (5000);

for (var i = 0; i < values.length; i++) {
  trace ("Testing: " + names[i]);
  test (values[i]);
}

getURL ("fscommand:quit", "");
