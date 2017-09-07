// makeswf -v 7 -r 1 -o rectangle-construct-7.swf rectangle-construct.as

#include "values.as"

// enable flash structure for version < 8 too for this test
ASSetPropFlags (_global, "flash", 0, 5248);

var Matrix = flash.geom.Matrix;

// the constructor
trace (new Matrix ());
trace (new Matrix (10));
trace (new Matrix (10, 20));
trace (new Matrix (10, 20, 30));
trace (new Matrix (10, 20, 30, 40));
trace (new Matrix (10, 20, 30, 40, 50));
trace (new Matrix (10, 20, 30, 40, 50, 60));
trace (new Matrix (10, 20, 30, 40, 50, 60, 70));

for (var i = 0; i < values.length; i++) {
  trace (new Matrix (values[i], 20, 30, 40, 50, 60));
}

// clone
trace (new Matrix (10, 20, 30, 40, 50, 60).clone ());
var tmp = { a: 10, b: 20, c: 30, d: 40, tx: 50, ty: 60 };
tmp.clone = Matrix.prototype.clone;
trace (tmp.clone ());
trace (tmp.clone () instanceOf Matrix);

// identity usage in the constructor
Matrix.prototype.identity = function () {
  trace ("identity");
  this.b = 123;
};

trace (new Matrix ());
trace (new Matrix (10, 20, 30, 40, 50, 60));

getURL ("FSCommand:quit", "");
