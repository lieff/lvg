// makeswf -v 7 -r 1 -o rectangle-contains-7.swf rectangle-contains.as

#include "values.as"

// enable flash structure for version < 8 too for this test
ASSetPropFlags (_global, "flash", 0, 5248);

var Rectangle = flash.geom.Rectangle;
var Point = flash.geom.Point;

for (var i = 0; i < values.length; i++) {
  var rect = new Rectangle (values[i], 20, 30, 40);
  trace (names[i] + ": " + rect);
  trace (rect.contains (20, 20));
  trace (rect.containsPoint (new Point (20, 20)));
  trace (rect.containsRectangle (new Rectangle (20, 20, 10, 10)));
  trace (rect.containsRectangle (rect));

  rect = new Rectangle (0, 20, 30, 40);
  trace (rect.contains (values[i], 20));
}

var realContains = flash.geom.Rectangle.prototype.contains;
flash.geom.Rectangle.prototype.contains = function () {
  trace ("contains called");
  return realContains.apply (this, arguments);
};

var realContainsPoint = flash.geom.Rectangle.prototype.containsPoint;
flash.geom.Rectangle.prototype.containsPoint = function () {
  trace ("containsPoint called");
  return realContainsPoint.apply (this, arguments);
};

var realContainsRectangle = flash.geom.Rectangle.prototype.containsRectangle;
flash.geom.Rectangle.prototype.containsRectangle = function () {
  trace ("containsRectangle called");
  return realContainsRectangle.apply (this, arguments);
};

var rect = new Rectangle (10, 20, 30, 40);
trace (rect.contains (20, 20));
trace (rect.containsPoint (new Point (20, 20)));
trace (rect.containsRectangle (new Rectangle (20, 20, 10, 10)));

loadMovie ("FSCommand:quit", "");
