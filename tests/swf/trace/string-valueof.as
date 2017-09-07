// makeswf -v 7 -s 200x150 -r 15 -o string-valueof.swf string-valueof.as

#include "trace_properties.as"
#include "values.as"

var s = new String ("hello");
s.valueOf = function () {
  trace ("s.valueOf");
  return "s.valueOf";
};
s.toString = function () {
  trace ("s.toString");
  return "s.toString";
};
s.stringValueOf = String.prototype.valueOf;
var v = s.stringValueOf ();
trace (v);
trace (typeof (v));
trace_properties (v, "local", "v");

for (var i = 0; i < values.length; i++) {
  trace ("Testing: " + names[i]);
  var o = values[i];
  o.stringValueOf = String.prototype.valueOf;
  var v = o.stringValueOf ();
  trace (v);
  trace (typeof (v));
  trace_properties (v, "local", "v");
}

getURL ("fscommand:quit", "");
