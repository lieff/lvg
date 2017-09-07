// makeswf -v 7 -r 1 -o context-menu-item-properties-7.swf context-menu-item-properties.as

#include "trace_properties.as"

var a = new ContextMenuItem ();
var b = a.copy ();

trace_properties (_global.ContextMenuItem, "_global", "ContextMenuItem");
trace_properties (a, "local", "a");
trace_properties (b, "local", "b");

loadMovie ("FSCommand:quit", "");

