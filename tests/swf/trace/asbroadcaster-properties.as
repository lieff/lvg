// makeswf -v 7 -r 1 -o asbroadcaster-properties-7.swf asbroadcaster-properties.as

#include "trace_properties.as"

var a = new Object ();
AsBroadcaster.initialize (a);

trace_properties (_global.AsBroadcaster, "_global", "AsBroadcaster");
trace_properties (a, "local", "a");

loadMovie ("FSCommand:quit", "");
