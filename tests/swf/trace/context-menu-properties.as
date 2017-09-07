// makeswf -v 7 -r 1 -o context-menu-properties-7.swf context-menu-properties.as

#include "trace_properties.as"

var a = new ContextMenu ();
var b = a.copy ();

trace_properties (_global.ContextMenu, "_global", "ContextMenu");
trace_properties (a, "local", "a");
trace_properties (b, "local", "b");

loadMovie ("FSCommand:quit", "");
