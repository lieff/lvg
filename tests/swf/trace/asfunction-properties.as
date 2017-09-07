// makeswf -v 7 -r 1 -o asfunction-properties-7.swf asfunction-properties.as

#include "trace_properties.as"

var a = ASconstructor (106, 2); // _global.Number
var b = ASnative (106, 2);
var c = ASconstructor (106, 1); // _global.Number.prototype.toString
var d = ASnative (106, 1);

trace_properties (_global.ASconstructor, "_global", "ASconstructor");
trace_properties (_global.ASnative, "_global", "ASnative");
trace_properties (_global.ASSetNativeAccessor, "_global", "ASSetNativeAccessor");
trace_properties (_global.ASSetNative, "_global", "ASSetNative");
trace_properties (_global.ASSetPropFlags, "_global", "ASSetPropFlags");

trace_properties (a, "local", "a");
trace_properties (b, "local", "b");
trace_properties (c, "local", "c");
trace_properties (d, "local", "d");

loadMovie ("FSCommand:quit", "");
