// makeswf -v 7 -r 1 -o shared-object-properties-7.swf shared-object-properties.as

#include "trace_properties.as"

var a = new SharedObject ();
// FIXME: test with a proper SharedObject that comes from getLocal

trace_properties (_global.SharedObject, "_global", "SharedObject");
trace_properties (a, "local", "a");

loadMovie ("FSCommand:quit", "");
