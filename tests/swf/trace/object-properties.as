// makeswf -v 7 -s 200x150 -r 1 -o string-properties-7.swf string-properties.as

#include "trace_properties.as"

var a = new Object ();
var b = Object ();
var c = {};
// FIXME: doesn't work in Swfdec currently
//var d = new Object ({ x: 1 });
var e = Object ({ x: 1 });
var f = { x: 1 };

trace_properties (_global.Object, "_global", "Object");
trace_properties (a, "local", "a");
trace_properties (b, "local", "b");
trace_properties (c, "local", "c");
//trace_properties (d, "local", "d");
trace_properties (e, "local", "e");
trace_properties (f, "local", "f");

getURL ("FSCommand:quit", "");
