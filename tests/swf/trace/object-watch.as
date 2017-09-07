// makeswf -v 7 -r 1 -o test-7.swf test.as

// create some functions
var watch_test = function (prop, old_value, new_value, user_data) {
  trace (this + "." + prop + ": " + old_value + " -> " + new_value + " (" + user_data + ") : " + arguments.length);
  return new_value;
};

var watch_loop = function (prop, old_value, new_value, user_data) {
  trace (old_value + " -> " + new_value);
  this[prop] = new_value + 1;
  return new_value;
};

var watch_loop_complex1 = function (prop, old_value, new_value, user_data) {
  trace ("1: " + old_value + " -> " + new_value);
  this["loop2"] = new_value + 1;
  return new_value;
};

var watch_loop_complex2 = function (prop, old_value, new_value, user_data) {
  trace ("2: " + old_value + " -> " + new_value);
  this["loop3"] = new_value + 1;
  return new_value;
};

var watch_loop_complex3 = function (prop, old_value, new_value, user_data) {
  trace ("3: " + old_value + " -> " + new_value);
  this["loop1"] = new_value + 1;
  return new_value;
};

var watch_delete = function (prop, old_value, new_value, user_data) {
  trace (old_value + " -> " + new_value);
  delete this[prop];
  return new_value;
};


// create the object
var o = new Object();
o.toString = function () { return "o"; };


trace ("normal");
o.test = 0;
trace (o.watch ("test", watch_test, "user data"));
o.test = 1;
trace (o.unwatch ("test"));
o.test = 2;

o.test = 0;
trace (o.watch ("test", watch_test));
o.test = 1;
trace (o.unwatch ("test"));
o.test = 2;


trace ("deleting the variable");
o.test = 0;
trace (o.watch ("test", watch_test));
o.test = 1;
delete o.test;
trace (o.hasOwnProperty ("test"));
o.test = 2;
delete o.test;
trace (o.hasOwnProperty ("test"));
trace (o.unwatch ("test"));
o.test = 3;


trace ("missing params");
trace (o.watch ());
trace (o.unwatch ());

o.test = 0;
trace (o.watch ("test"));
o.test = 1;
trace (o.unwatch ("test"));
o.test = 2;


trace ("extra params");
o.test = 0;
trace (o.watch ("test", watch_test, "user data", "user data2"));
o.test = 1;
trace (o.unwatch ("test"));
o.test = 2;


trace ("deleting the object");
o.test = 0;
trace (o.watch ("test", watch_test));
o.test = 1;

delete o;
o = new Object();
o.toString = function () { return "o"; };

o.test = 2;
trace (o.unwatch ("test"));
o.test = 3;


trace ("loop");
o.test = 0;
trace (o.watch ("test", watch_loop, "user data"));
o.test = 1;
trace (o.unwatch ("test"));
o.test = 2;

o.loop1 = 0;
trace (o.watch ("loop1", watch_loop_complex1));
trace (o.watch ("loop2", watch_loop_complex2));
trace (o.watch ("loop3", watch_loop_complex3));
o.loop1 = 1;
trace (o.unwatch ("loop1"));
o.loop1 = 2;


trace ("deleting inside watch function");
o.test = 0;
trace (o.watch ("test", watch_delete));
o.test = 1;
trace (o.hasOwnProperty ("test"));
o.test = 2;
trace (o.hasOwnProperty ("test"));
trace (o.unwatch ("test"));
o.test = 3;


trace ("working with native functions");
var test_get = function () { trace ("get"); return "get"; };
var test_set = function (value) { trace ("set " + value); };

o.test = 0;
trace (o.watch ("test", watch_test));
o.test = 1;
o.addProperty ("test", test_get, test_set);
o.test = 2;
trace (o.unwatch ("test"));
o.test = 3;

o.test2 = 0;
o.addProperty ("test2", test_get, test_set);
o.test2 = 1;
trace (o.watch ("test2", watch_test));
o.test2 = 2;
trace (o.unwatch ("test2"));
o.test2 = 3;


loadMovie ("FSCommand:quit", "");
