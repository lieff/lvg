// makeswf -v 7 -s 200x150 -r 15 -o object-resolve.swf object-resolve.as

var o = {};
o.__resolve = function () {
  trace ("__resolve: " + arguments);
  return "resolved";
};

trace ("-- normal");
trace (o.world);
trace (o.wORld);
o.world = "assigned";
trace (o.world);
trace (o.wORld);

trace ("-- with");
with (o) {
  trace (world);
  trace (wORld);
  trace (missing);
}

var o = { __proto__: {} };
o.__proto__.__resolve = function () {
  trace ("__resolve: " + arguments);
  return "resolved";
};

trace ("-- __proto__");
trace (o.world);

var o = { __proto__: {} };
o.__resolve = function () {
  trace ("__resolve: " + arguments);
  return "resolved";
};
o.__proto__.world = "assigned";

trace ("-- variable in __proto__");
trace (o.world);

var o = { __proto__: {} };
o.__resolve = undefined;
o.__proto__.__resolve = function () {
  trace ("__resolve: " + arguments);
  return "resolved";
};

trace ("-- __proto__, undefined in the way");
trace (o.world);

var o = { __proto__: {} };
o.__resolve = 42;
o.__proto__.__resolve = function () {
  trace ("__resolve: " + arguments);
  return "resolved";
};

trace ("-- __proto__, number in the way");
trace (o.world);

var o = { __proto__: {} };
o.__resolve = new Object ();
o.__proto__.__resolve = function () {
  trace ("__resolve: " + arguments);
  return "resolved";
};

trace ("-- __proto__, Object in the way");
trace (o.world);

function get () {
  trace ("get");
  return function () {
    trace ("__resolve: " + arguments);
    return "resolved";
  };
}

function set () {
  trace ("set: " + arguments);
}

var o = {};
o.addProperty ("__resolve", get, set);
trace ("-- addProperty");
trace (o.world);
o.world = "assigned";
trace (o.world);

var o = { __proto__: {} };
o.addProperty ("__resolve", get, set);
o.__proto__.__resolve = function () {
  trace ("__proto__.__resolve: " + arguments);
  return "resolved";
};
trace ("-- addProperty and normal in __proto__");
trace (o.world);
o.world = "assigned";
trace (o.world);

var o = { __proto__: {} };
o.__resolve = function () {
  trace ("__resolve: " + arguments);
  return "resolved";
};
o.addProperty ("__resolve", get, set);
o.__proto__.__resolve = function () {
  trace ("__proto__.__resolve: " + arguments);
  return "resolved";
};
trace ("-- addProperty and normal together");
trace (o.world);
o.world = "assigned";
trace (o.world);

var o = { __proto__: {} };
o.__resolve = new Object ();
o.addProperty ("__resolve", get, set);
o.__proto__.__resolve = function () {
  trace ("__proto__.__resolve: " + arguments);
  return "resolved";
};
trace ("-- yaragh");
trace (o.world);
o.world = "assigned";
trace (o.world);

getURL ("fscommand:quit", "");
