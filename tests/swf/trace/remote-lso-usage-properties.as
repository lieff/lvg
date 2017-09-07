// makeswf -v 7 -r 1 -o remote-lso-usage-properties-7.swf remote-lso-usage-properties.as

#include "trace_properties.as"

var a = new RemoteLSOUsage ();

trace_properties (_global.RemoteLSOUsage, "_global", "RemoteLSOUsage");
trace_properties (a, "local", "a");

loadMovie ("FSCommand:quit", "");
