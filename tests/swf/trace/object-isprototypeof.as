// makeswf -v 7 -r 1 -o test-7.swf test.as

#include "values.as"

var o = new Object ();
var empty = new Object ();
ASSetPropFlags (empty, null, 0, 7);
for (var prop in empty) {
  empty[prop] = "to-be-deleted";
  delete empty[prop];
}

trace (Object.prototype.isPrototypeOf (Object));
trace (Object.prototype.isPrototypeOf (o));
trace (Object.isPrototypeOf (o));

trace (Object.prototype.isPrototypeOf ());
trace (Object.prototype.isPrototypeOf (Object, o));
trace (String.prototype.isPrototypeOf ("test", o));
trace (String.prototype.isPrototypeOf (o, "test"));

trace (o.isPrototypeOf (empty));
empty.prototype = o;
trace (o.isPrototypeOf (empty));
delete empty.prototype;
empty.__proto__ = o;
trace (o.isPrototypeOf (empty));

for (var i = 0; i < values.length; i++) {
  trace ("Testing: " + names[i]);
  trace (Object.prototype.isPrototypeOf (values[i]) + " " +
      String.prototype.isPrototypeOf (values[i]) + " " +
      Number.prototype.isPrototypeOf (values[i]) + " " +
      Boolean.prototype.isPrototypeOf (values[i]) + " " +
      Array.prototype.isPrototypeOf (values[i]));
}


loadMovie ("FSCommand:quit", "");
