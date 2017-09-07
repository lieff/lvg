// makeswf -v 7 -r 1 -o xml-socket-properties-7.swf xml-socket-properties.as

#include "trace_properties.as"

var a = new XMLSocket ();

trace_properties (_global.XMLSocket, "_global", "XMLSocket");
trace_properties (a, "local", "a");

loadMovie ("FSCommand:quit", "");
