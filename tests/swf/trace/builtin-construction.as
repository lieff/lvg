// makeswf -v 7 -s 200x150 -r 1 -o builtin-construction.swf builtin-construction.as

f = Function;
_global.Function = function () {
  trace ("Function");
};
x = function () {};
trace (f.prototype == x.__proto__);
trace (Function.prototype == x.__proto__);

m = MovieClip;
_global.MovieClip = function () {
  trace ("MovieClip");
};
createEmptyMovieClip ("a", 0);
trace (m.prototype == a.__proto__);
trace (MovieClip.prototype == a.__proto__);

o = Object;
_global.Object = function () {
  trace ("Object");
};

y = function () {};
trace (f.prototype == y.__proto__);
trace (Function.prototype == y.__proto__);
trace (y.prototype.__proto__ == o.prototype);
trace (y.prototype.__proto__ == Object.prototype);

z = {};
trace (o.prototype == z.__proto__);
trace (Object.prototype == z.__proto__);

_global.Object.prototype = 23;
z = {};
trace (z.__proto__);
y = function () {};
trace (y.prototype);
trace (y.prototype.__proto__);

_global.Object = 42;
z = {};
trace (z.__proto__);
y = function () {};
trace (y.prototype);
trace (y.prototype.__proto__);


getURL ("fscommand:quit", "");
