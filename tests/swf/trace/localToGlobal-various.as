// makeswf -v 7 -s 200x150 -r 1 -o localToGlobal-various.swf localToGlobal-various.as

l2g = function (mc, x, y) {
  if (typeof (x) != "object")
    return l2g (mc, { x: x, y: y });

  var s = x.x + ", " + x.y + " => ";
  s += mc.localToGlobal (x) + " => ";
  trace (s + x.x + ", " + x.y);
};

g2l = function (mc, x, y) {
  if (typeof (x) != "object")
    return l2g (mc, { x: x, y: y });

  var s = x.x + ", " + x.y + " => ";
  s += mc.globalToLocal (x) + " => ";
  trace (s + x.x + ", " + x.y);
};

createEmptyMovieClip ("a", 0);
a._x = 50;
a._y = 50;

a.createEmptyMovieClip ("a", 0);
a.a._xscale = 50;
a.a._yscale = 50;

trace ("Test empty or semi-empty stuctures");
l2g (a.a, {});
g2l (a.a, {});
l2g (a.a, {x: 10});
l2g (a.a, {y: 10});
g2l (a.a, {x: 10});
g2l (a.a, {y: 10});

trace ("Test various values");
#include "values.as"
for (i = 0; i < values.length; i++) {
  l2g (a.a, {x: values[i], y: values[i]});
  g2l (a.a, {x: values[i], y: values[i]});
}

trace ("Chaining to see accuracy");
x = { x: 0, y: 0 };
for (i = 0; i < 4; i++) {
  l2g (a.a, x);
}
for (i = 0; i < 4; i++) {
  g2l (a.a, x);
}

trace ("Testing values on prototype");
x = { x: 0, y: 0 };
y = { __proto__: x };
l2g (a.a, y);
g2l (a.a, x);

trace ("Testing addProperty");
x = {};
x.addProperty ("x", function () { trace ("x"); return 50; }, null);
x.addProperty ("y", function () { trace ("y"); return 50; }, null);
l2g (a.a, x);
g2l (a.a, x);

loadMovie ("fscommand:quit", "");
