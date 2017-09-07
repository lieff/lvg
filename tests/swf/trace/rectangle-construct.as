// makeswf -v 7 -r 1 -o rectangle-construct-7.swf rectangle-construct.as

#include "values.as"

// enable flash structure for version < 8 too for this test
ASSetPropFlags (_global, "flash", 0, 5248);

var Rectangle = flash.geom.Rectangle;

// the constructor
trace (new Rectangle ());
trace (new Rectangle (10));
trace (new Rectangle (10, 20));
trace (new Rectangle (10, 20, 30));
trace (new Rectangle (10, 20, 30, 40));
trace (new Rectangle (10, 20, 30, 40, 50));

for (var i = 0; i < values.length; i++) {
  trace (new Rectangle (values[i], 20, 30, 40));
}

// clone
trace (new Rectangle (10, 20, 30, 40).clone ());
var tmp = { x: 10, y: 20, width: 30, height: 40 };
tmp.clone = Rectangle.prototype.clone;
trace (tmp.clone ());
trace (tmp.clone () instanceOf Rectangle);

// setEmpty usage in the constructor
Rectangle.prototype.setEmpty = function () {
  trace ("setEmpty");
  this.y = 123;
};

trace (new Rectangle ());
trace (new Rectangle (10, 20, 30, 40));

loadMovie ("FSCommand:quit", "");
