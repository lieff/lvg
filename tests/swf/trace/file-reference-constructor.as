// makeswf -v 7 -r 1 -o file-reference-constructor-7.swf file-reference-constructor.as

#include "trace_properties.as"

// Test's how the function that is part of flash.net.FileReference's
// constructor behaves

var func = ASnative (2204, 200);
var a = {};
var b = new func (a);
var c = {};
func (c);
var d = {};
d.func = func;
d.func ("test");
var e = new func ();

trace_properties (a, "local", "a");
trace_properties (b, "local", "b");
trace_properties (c, "local", "c");
trace_properties (d, "local", "d");
trace_properties (e, "local", "e");

loadMovie ("FSCommand:quit", "");
