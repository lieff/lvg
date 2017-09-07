// makeswf -v 7 -r 1 -o function-apply-7.swf function-apply.as

#include "values.as"

var i = 0;
function test_function () {
  trace ("this: " + this);
  trace ("arguments: " + arguments.length + ": " + arguments);
  return ++i;
}

var a = new Object ();
a.toString = function () { return "a"; };
a.test = test_function;

var b = new Object ();
b.toString = function () { return "b"; };

var fake_array = new Object ();
fake_array[0] = 1;
fake_array[1] = 2;
fake_array[2] = 3;
fake_array.length = 3;

trace ("Testing: (no arguments)");
trace (a.test.apply ());
trace ("Testing: a");
trace (a.test.apply (a));
trace ("Testing: a, [1, 2, 3]");
trace (a.test.apply (a, [1, 2, 3]));
trace ("Testing: a, [1, 2, 3], [4, 5]");
trace (a.test.apply (a, [1, 2, 3], [4, 5]));
trace ("Testing: a, 1, [2, 3]");
trace (a.test.apply (a, 1, [2, 3]));
trace ("Testing: a, 1, 2, 3");
trace (a.test.apply (a, 1, 2, 3));
trace ("Testing: a, fake_array");
trace (a.test.apply (a, fake_array));
trace ("Testing: b, [1, 2, 3]");
trace (a.test.apply (b, [1, 2, 3]));

for (var i = 0; i < values.length; i++) {
  trace ("Testing: " + names[i] + ", [1, 2, 3]");
  trace (a.test.apply (values[i], [1, 2, 3]));
}

loadMovie ("FSCommand:quit", "");
